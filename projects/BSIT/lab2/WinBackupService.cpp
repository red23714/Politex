#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsvc.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cwctype>
#include <iostream>
#include <mutex>

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shlwapi.lib")

namespace fs = std::filesystem;
static const wchar_t* SERVICE_NAME = L"WinBackupService";
static const wchar_t* SERVICE_DISPLAY = L"Windows ZIP Backup Service";

static SERVICE_STATUS_HANDLE g_statusHandle = nullptr;
static SERVICE_STATUS g_status{};
static HANDLE g_stopEvent = nullptr;
static std::wstring g_configPath;
static std::wstring g_logPath;
static std::mutex g_archiveMutex;

struct Config
{
	std::wstring source;
	std::wstring archive;
	std::vector<std::wstring> patterns;
	DWORD debounceMs = 1500;
	DWORD rescanSeconds = 60;
};

static std::wstring trim(std::wstring s)
{
	const wchar_t* ws = L" \t\r\n";
	size_t a = s.find_first_not_of(ws);
	if (a == std::wstring::npos)
		return L"";
	size_t b = s.find_last_not_of(ws);
	return s.substr(a, b - a + 1);
}
static std::wstring lower(std::wstring s)
{
	std::transform(s.begin(), s.end(), s.begin(),
				   [](wchar_t c) { return (wchar_t)towlower(c); });
	return s;
}
static std::wstring exeDir()
{
	std::wstring p(32768, L'\0');
	DWORD n = GetModuleFileNameW(nullptr, p.data(), (DWORD)p.size());
	p.resize(n);
	fs::path x(p);
	return x.parent_path().wstring();
}
static std::wstring joinPath(const std::wstring& a, const std::wstring& b)
{
	return (fs::path(a) / fs::path(b)).wstring();
}
static void logLine(const std::wstring& msg)
{
	if (g_logPath.empty())
		g_logPath = joinPath(exeDir(), L"WinBackupService.log");
	SYSTEMTIME t;
	GetLocalTime(&t);
	std::wofstream f(fs::path(g_logPath), std::ios::app);
	if (f)
		f << L"[" << t.wYear << L"-" << t.wMonth << L"-" << t.wDay << L" "
		  << t.wHour << L":" << t.wMinute << L":" << t.wSecond << L"] " << msg
		  << L"\n";
}
static bool parseConfig(const std::wstring& path, Config& c)
{
	std::wifstream f{fs::path(path)};
	if (!f)
	{
		logLine(L"Cannot open configuration: " + path);
		return false;
	}
	std::wstring line;
	while (std::getline(f, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == L'#' || line[0] == L';')
			continue;
		size_t eq = line.find(L'=');
		if (eq == std::wstring::npos)
			continue;
		std::wstring key = lower(trim(line.substr(0, eq)));
		std::wstring value = trim(line.substr(eq + 1));
		if (key == L"source_dir")
			c.source = value;
		else if (key == L"archive_path")
			c.archive = value;
		else if (key == L"patterns")
		{
			std::wstringstream ss(value);
			std::wstring item;
			while (std::getline(ss, item, L';'))
				if (!(item = trim(item)).empty())
					c.patterns.push_back(item);
		}
		else if (key == L"debounce_ms")
		{
			try
			{
				c.debounceMs = (DWORD)std::stoul(value);
			}
			catch (...)
			{
			}
		}
		else if (key == L"rescan_seconds")
		{
			try
			{
				c.rescanSeconds = (DWORD)std::stoul(value);
			}
			catch (...)
			{
			}
		}
	}
	if (c.source.empty() || c.archive.empty() || c.patterns.empty())
	{
		logLine(L"Config requires source_dir, archive_path and patterns.");
		return false;
	}
	if (c.debounceMs < 100)
		c.debounceMs = 100;
	if (c.rescanSeconds < 5)
		c.rescanSeconds = 5;
	return true;
}
static bool wildcardMatch(const wchar_t* pattern, const wchar_t* text)
{
	// '*' matches any sequence; '?' matches one character; case-insensitive.
	while (*pattern)
	{
		if (*pattern == L'*')
		{
			while (*pattern == L'*')
				++pattern;
			if (!*pattern)
				return true;
			for (const wchar_t* p = text;; ++p)
			{
				if (wildcardMatch(pattern, p))
					return true;
				if (!*p)
					break;
			}
			return false;
		}
		if (!*text)
			return false;
		if (*pattern != L'?' && towlower(*pattern) != towlower(*text))
			return false;
		++pattern;
		++text;
	}
	return *text == L'\0';
}
static bool matches(const Config& c, const fs::path& p)
{
	const std::wstring name = p.filename().wstring();
	const std::wstring rel = p.filename().wstring();
	for (const auto& pat : c.patterns)
	{
		if (wildcardMatch(pat.c_str(), name.c_str()) ||
			wildcardMatch(pat.c_str(), rel.c_str()))
			return true;
		// A pattern containing a relative subpath is matched against the
		// source-relative path below.
	}
	return false;
}
static uint32_t crc32(const unsigned char* data, size_t n)
{
	uint32_t c = 0xFFFFFFFFu;
	for (size_t i = 0; i < n; ++i)
	{
		c ^= data[i];
		for (int k = 0; k < 8; ++k)
			c = (c >> 1) ^ (0xEDB88320u & (0u - (c & 1u)));
	}
	return ~c;
}
static void put16(std::ofstream& f, uint16_t v)
{
	char b[2] = {(char)(v & 255), (char)((v >> 8) & 255)};
	f.write(b, 2);
}
static void put32(std::ofstream& f, uint32_t v)
{
	char b[4] = {(char)(v & 255), (char)((v >> 8) & 255),
				 (char)((v >> 16) & 255), (char)((v >> 24) & 255)};
	f.write(b, 4);
}
static std::string utf8(const std::wstring& w)
{
	if (w.empty())
		return {};
	int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr,
								0, nullptr, nullptr);
	std::string s(n, '\0');
	WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), s.data(), n,
						nullptr, nullptr);
	return s;
}
struct ZipEntry
{
	std::string name;
	uint32_t crc, size, offset;
	uint16_t time, date;
};
static void dosTimeDate(const fs::file_time_type& ft, uint16_t& tm,
						uint16_t& dt)
{
	(void)ft;
	SYSTEMTIME st;
	GetLocalTime(&st);
	tm = (uint16_t)(((st.wHour & 31) << 11) | ((st.wMinute & 63) << 5) |
					((st.wSecond / 2) & 31));
	dt = (uint16_t)((((st.wYear - 1980) & 127) << 9) | ((st.wMonth & 15) << 5) |
					(st.wDay & 31));
}
static bool addFileToZip(std::ofstream& out, const fs::path& file,
						 const fs::path& source, std::vector<ZipEntry>& entries)
{
	std::ifstream in(file, std::ios::binary);
	if (!in)
	{
		logLine(L"Cannot read file: " + file.wstring());
		return false;
	}
	std::vector<unsigned char> data((std::istreambuf_iterator<char>(in)),
									std::istreambuf_iterator<char>());
	if (data.size() > 0xFFFFFFFFull)
	{
		logLine(L"Skipping file larger than 4 GiB: " + file.wstring());
		return true;
	}
	std::error_code ec;
	fs::path rel = fs::relative(file, source, ec);
	if (ec)
		rel = file.filename();
	std::wstring relw = rel.wstring();
	std::replace(relw.begin(), relw.end(), L'\\', L'/');
	std::string name = utf8(relw);
	if (name.size() > 65535)
		return false;
	ZipEntry e{};
	e.name = name;
	e.crc = crc32(data.data(), data.size());
	e.size = (uint32_t)data.size();
	e.offset = (uint32_t)out.tellp();
	dosTimeDate(fs::last_write_time(file, ec), e.time, e.date);
	put32(out, 0x04034b50);
	put16(out, 20);
	put16(out, 0x0800);
	put16(out, 0); // UTF-8, STORE
	put16(out, e.time);
	put16(out, e.date);
	put32(out, e.crc);
	put32(out, e.size);
	put32(out, e.size);
	put16(out, (uint16_t)e.name.size());
	put16(out, 0);
	out.write(e.name.data(), (std::streamsize)e.name.size());
	if (!data.empty())
		out.write((const char*)data.data(), (std::streamsize)data.size());
	if (!out)
		return false;
	entries.push_back(std::move(e));
	return true;
}
static bool createZip(const Config& c)
{
	std::lock_guard<std::mutex> lock(g_archiveMutex);
	fs::path source(c.source), archive(c.archive);
	std::error_code ec;
	if (!fs::is_directory(source, ec))
	{
		logLine(L"Source directory is unavailable: " + c.source);
		return false;
	}
	if (!archive.parent_path().empty())
		fs::create_directories(archive.parent_path(), ec);
	fs::path temp = archive;
	temp += L".tmp";
	std::ofstream out(temp, std::ios::binary | std::ios::trunc);
	if (!out)
	{
		logLine(L"Cannot create archive: " + temp.wstring());
		return false;
	}
	std::vector<ZipEntry> entries;
	for (fs::recursive_directory_iterator
			 it(source, fs::directory_options::skip_permission_denied, ec),
		 end;
		 it != end; it.increment(ec))
	{
		if (ec)
		{
			ec.clear();
			continue;
		}
		if (!it->is_regular_file(ec))
			continue;
		fs::path file = it->path();
		bool selected = false;
		std::wstring name = file.filename().wstring();
		std::error_code relEc;
		std::wstring rel = fs::relative(file, source, relEc).wstring();
		std::replace(rel.begin(), rel.end(), L'\\', L'/');
		for (const auto& pat : c.patterns)
		{
			if (wildcardMatch(pat.c_str(), name.c_str()) ||
				wildcardMatch(pat.c_str(), rel.c_str()))
			{
				selected = true;
				break;
			}
		}
		if (!selected)
			continue;
		if (lower(fs::absolute(file, ec).wstring()) ==
			lower(fs::absolute(archive, ec).wstring()))
		{
			ec.clear();
			continue;
		}
		if (!addFileToZip(out, file, source, entries))
		{
			out.close();
			fs::remove(temp, ec);
			logLine(L"Failed while writing ZIP.");
			return false;
		}
	}
	// End of central directory records.
	uint32_t centralOffset = (uint32_t)out.tellp();
	for (const auto& e : entries)
	{
		put32(out, 0x02014b50);
		put16(out, 20);
		put16(out, 20);
		put16(out, 0x0800);
		put16(out, 0);
		put16(out, e.time);
		put16(out, e.date);
		put32(out, e.crc);
		put32(out, e.size);
		put32(out, e.size);
		put16(out, (uint16_t)e.name.size());
		put16(out, 0);
		put16(out, 0);
		put16(out, 0);
		put16(out, 0);
		put32(out, 0);
		put32(out, e.offset);
		out.write(e.name.data(), (std::streamsize)e.name.size());
	}
	uint32_t centralSize = (uint32_t)out.tellp() - centralOffset;
	put32(out, 0x06054b50);
	put16(out, 0);
	put16(out, 0);
	put16(out, (uint16_t)entries.size());
	put16(out, (uint16_t)entries.size());
	put32(out, centralSize);
	put32(out, centralOffset);
	put16(out, 0);
	out.close();
	if (!out)
	{
		fs::remove(temp, ec);
		logLine(L"Failed to finish ZIP.");
		return false;
	}
	fs::remove(archive, ec);
	ec.clear();
	fs::rename(temp, archive, ec);
	if (ec)
	{
		logLine(L"Cannot replace archive.");
		fs::remove(temp, ec);
		return false;
	}
	logLine(L"Archive refreshed; files included: " +
			std::to_wstring(entries.size()));
	return true;
}
static DWORD WINAPI watchWorker(LPVOID param)
{
	Config* c = (Config*)param;
	HANDLE hDir =
		CreateFileW(c->source.c_str(), FILE_LIST_DIRECTORY,
					FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
					nullptr, OPEN_EXISTING,
					FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
	if (hDir == INVALID_HANDLE_VALUE)
	{
		logLine(L"Cannot watch source directory (check path and service "
				L"account permissions): " +
				c->source);
		return 1;
	}
	HANDLE event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
	if (!event)
	{
		CloseHandle(hDir);
		return 1;
	}
	createZip(*c); // initial backup
	std::vector<BYTE> buffer(64 * 1024);
	while (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0)
	{
		OVERLAPPED ov{};
		ov.hEvent = event;
		ResetEvent(event);
		DWORD bytes = 0;
		BOOL ok = ReadDirectoryChangesW(
			hDir, buffer.data(), (DWORD)buffer.size(), TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
				FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE |
				FILE_NOTIFY_CHANGE_CREATION,
			nullptr, &ov, nullptr);
		if (!ok && GetLastError() != ERROR_IO_PENDING)
		{
			logLine(L"ReadDirectoryChangesW failed; retrying.");
			Sleep(1000);
			continue;
		}
		HANDLE waits[2] = {g_stopEvent, event};
		DWORD w = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
		if (w == WAIT_OBJECT_0)
		{
			CancelIoEx(hDir, &ov);
			break;
		}
		if (w == WAIT_OBJECT_0 + 1)
		{
			DWORD transferred = 0;
			if (GetOverlappedResult(hDir, &ov, &transferred, FALSE) &&
				transferred > 0)
			{
				// Debounce a burst of file changes.
				if (WaitForSingleObject(g_stopEvent, c->debounceMs) ==
					WAIT_TIMEOUT)
					createZip(*c);
			}
		}
	}
	CancelIoEx(hDir, nullptr);
	CloseHandle(event);
	CloseHandle(hDir);
	return 0;
}
static bool runWorker()
{
	Config c;
	if (!parseConfig(g_configPath, c))
		return false;
	g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
	if (!g_stopEvent)
		return false;
	HANDLE thread = CreateThread(nullptr, 0, watchWorker, &c, 0, nullptr);
	if (!thread)
	{
		CloseHandle(g_stopEvent);
		g_stopEvent = nullptr;
		return false;
	}
	// Periodic rescans ensure recovery from missed notifications.
	while (WaitForSingleObject(g_stopEvent, 1000) != WAIT_OBJECT_0)
	{
		static DWORD ticks = 0;
		ticks += 1000;
		if (ticks >= c.rescanSeconds * 1000)
		{
			createZip(c);
			ticks = 0;
		}
	}
	CancelSynchronousIo(thread);
	SetEvent(g_stopEvent);
	WaitForSingleObject(thread, 5000);
	CloseHandle(thread);
	CloseHandle(g_stopEvent);
	g_stopEvent = nullptr;
	return true;
}
static void setStatus(DWORD state, DWORD win32Exit = NO_ERROR,
					  DWORD waitHint = 0)
{
	g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_status.dwCurrentState = state;
	g_status.dwWin32ExitCode = win32Exit;
	g_status.dwServiceSpecificExitCode = 0;
	g_status.dwWaitHint = waitHint;
	g_status.dwControlsAccepted =
		(state == SERVICE_RUNNING)
			? (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN)
			: 0;
	g_status.dwCheckPoint =
		(state == SERVICE_START_PENDING || state == SERVICE_STOP_PENDING) ? 1
																		  : 0;
	if (g_statusHandle)
		SetServiceStatus(g_statusHandle, &g_status);
}
static DWORD WINAPI controlHandlerEx(DWORD control, DWORD, LPVOID, LPVOID)
{
	switch (control)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		setStatus(SERVICE_STOP_PENDING, NO_ERROR, 5000);
		if (g_stopEvent)
			SetEvent(g_stopEvent);
		return NO_ERROR;
	case SERVICE_CONTROL_INTERROGATE:
		if (g_statusHandle)
			SetServiceStatus(g_statusHandle, &g_status);
		return NO_ERROR;
	default:
		return ERROR_CALL_NOT_IMPLEMENTED;
	}
}
static void WINAPI serviceMain(DWORD, LPWSTR*)
{
	g_statusHandle =
		RegisterServiceCtrlHandlerExW(SERVICE_NAME, controlHandlerEx, nullptr);
	if (!g_statusHandle)
		return;
	setStatus(SERVICE_START_PENDING, NO_ERROR, 5000);
	logLine(L"Service starting.");
	Config initialConfig;
	if (!parseConfig(g_configPath, initialConfig) ||
		!fs::is_directory(fs::path(initialConfig.source)))
	{
		logLine(L"Startup validation failed; check backup.ini and source_dir.");
		setStatus(SERVICE_STOPPED, ERROR_INVALID_DATA);
		return;
	}
	setStatus(SERVICE_RUNNING);
	bool ok = runWorker();
	logLine(ok ? L"Service stopped." : L"Service failed.");
	setStatus(SERVICE_STOPPED, ok ? NO_ERROR : ERROR_SERVICE_SPECIFIC_ERROR);
}
static bool manageService(const std::wstring& action)
{
	SC_HANDLE scm = OpenSCManagerW(
		nullptr, nullptr, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
	if (!scm)
	{
		std::wcerr << L"OpenSCManager failed: " << GetLastError() << L"\n";
		return false;
	}
	SC_HANDLE svc = OpenServiceW(scm, SERVICE_NAME,
								 SERVICE_QUERY_STATUS | SERVICE_START |
									 SERVICE_STOP | DELETE);
	if (action == L"install")
	{
		if (svc)
		{
			CloseServiceHandle(svc);
			CloseServiceHandle(scm);
			std::wcerr << L"Service already exists.\n";
			return false;
		}
		std::wstring exe = L"\"" + std::wstring(GetCommandLineW()) +
						   L"\""; // replaced below with actual executable path
		std::wstring currentExe(32768, L'\0');
		DWORD exeLen = GetModuleFileNameW(nullptr, currentExe.data(),
										  (DWORD)currentExe.size());
		currentExe.resize(exeLen);
		exe = L"\"" + currentExe + L"\" --service";
		svc = CreateServiceW(scm, SERVICE_NAME, SERVICE_DISPLAY,
							 SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
							 SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
							 exe.c_str(), nullptr, nullptr, nullptr, nullptr,
							 nullptr);
		if (!svc)
		{
			std::wcerr << L"CreateService failed: " << GetLastError() << L"\n";
			CloseServiceHandle(scm);
			return false;
		}
		CloseServiceHandle(svc);
		CloseServiceHandle(scm);
		std::wcout << L"Service installed.\n";
		return true;
	}
	if (!svc)
	{
		std::wcerr << L"OpenService failed: " << GetLastError() << L"\n";
		CloseServiceHandle(scm);
		return false;
	}
	bool ok = true;
	if (action == L"remove")
	{
		SERVICE_STATUS ss{};
		ControlService(svc, SERVICE_CONTROL_STOP, &ss);
		if (!DeleteService(svc))
		{
			std::wcerr << L"DeleteService failed: " << GetLastError() << L"\n";
			ok = false;
		}
		else
			std::wcout << L"Service marked for deletion.\n";
	}
	else if (action == L"start")
	{
		if (!StartServiceW(svc, 0, nullptr))
		{
			DWORD e = GetLastError();
			if (e != ERROR_SERVICE_ALREADY_RUNNING)
			{
				std::wcerr << L"StartService failed: " << e << L"\n";
				ok = false;
			}
		}
		if (ok)
			std::wcout << L"Start requested.\n";
	}
	else if (action == L"stop")
	{
		SERVICE_STATUS ss{};
		if (!ControlService(svc, SERVICE_CONTROL_STOP, &ss))
		{
			DWORD e = GetLastError();
			if (e != ERROR_SERVICE_NOT_ACTIVE)
			{
				std::wcerr << L"Stop failed: " << e << L"\n";
				ok = false;
			}
		}
		if (ok)
			std::wcout << L"Stop requested.\n";
	}
	else if (action == L"restart")
	{
		SERVICE_STATUS ss{};
		ControlService(svc, SERVICE_CONTROL_STOP, &ss);
		for (int i = 0; i < 30; ++i)
		{
			SERVICE_STATUS_PROCESS ssp{};
			DWORD needed = 0;
			if (QueryServiceStatusEx(svc, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp,
									 sizeof(ssp), &needed) &&
				ssp.dwCurrentState == SERVICE_STOPPED)
				break;
			Sleep(500);
		}
		if (!StartServiceW(svc, 0, nullptr))
		{
			std::wcerr << L"Restart failed: " << GetLastError() << L"\n";
			ok = false;
		}
		else
			std::wcout << L"Restart requested.\n";
	}
	else if (action == L"status")
	{
		SERVICE_STATUS_PROCESS ssp{};
		DWORD needed = 0;
		if (QueryServiceStatusEx(svc, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp,
								 sizeof(ssp), &needed))
			std::wcout << L"State code: " << ssp.dwCurrentState << L"\n";
		else
			ok = false;
	}
	CloseServiceHandle(svc);
	CloseServiceHandle(scm);
	return ok;
}
int wmain(int argc, wchar_t** argv)
{
	g_configPath = joinPath(exeDir(), L"backup.ini");
	g_logPath = joinPath(exeDir(), L"WinBackupService.log");
	std::wstring action = argc > 1 ? lower(argv[1]) : L"";
	if (action == L"install" || action == L"remove" || action == L"start" ||
		action == L"stop" || action == L"restart" || action == L"status")
		return manageService(action) ? 0 : 1;
	if (action == L"debug")
	{
		std::wcout << L"Debug worker mode. Stop with Ctrl+C.\n";
		SetConsoleCtrlHandler(
			[](DWORD t) -> BOOL
			{
				if (t == CTRL_C_EVENT || t == CTRL_BREAK_EVENT ||
					t == CTRL_CLOSE_EVENT)
				{
					if (g_stopEvent)
						SetEvent(g_stopEvent);
					return TRUE;
				}
				return FALSE;
			},
			TRUE);
		// runWorker creates its own stop event; Ctrl+C is primarily useful for
		// manual termination.
		return runWorker() ? 0 : 1;
	}
	SERVICE_TABLE_ENTRYW table[] = {
		{const_cast<LPWSTR>(SERVICE_NAME), serviceMain}, {nullptr, nullptr}};
	if (!StartServiceCtrlDispatcherW(table))
	{
		DWORD e = GetLastError();
		std::wcerr << L"Not started by SCM (error " << e
				   << L"). Use install/start or debug.\n";
		return 1;
	}
	return 0;
}
