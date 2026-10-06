#include "common.h"
#include <aclapi.h>
#include <sddl.h>

/* ------------------------------------------------------------------ */
/* Вспомогательное                                                     */
/* ------------------------------------------------------------------ */
static void result_ok(sbuf* r, const char* cmd)
{
	sb_rec_begin(r, "RESULT");
	sb_kv_str(r, "status", "OK");
	sb_kv_str(r, "command", cmd);
	sb_rec_end(r);
}

static char* win_error_text(DWORD code)
{
	wchar_t buf[512];
	DWORD n = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM |
								 FORMAT_MESSAGE_IGNORE_INSERTS,
							 NULL, code, 0, buf, 512, NULL);
	while (n > 0 &&
		   (buf[n - 1] == L'\r' || buf[n - 1] == L'\n' || buf[n - 1] == L' '))
		n--;
	buf[n] = 0;
	if (n == 0)
		return _strdup("unknown error");
	return wide_to_utf8(buf);
}

/* ошибка протокола (source=protocol) */
static int result_proto_error(sbuf* r, int code, const char* msg)
{
	r->len = 0;
	r->data[0] = 0;
	sb_rec_begin(r, "RESULT");
	sb_kv_str(r, "status", "ERROR");
	sb_kv_i64(r, "code", code);
	sb_kv_str(r, "source", "protocol");
	sb_kv_str(r, "message", msg);
	sb_rec_end(r);
	return 0;
}

/* ошибка Win32 (source=win32) */
static int result_win_error(sbuf* r, DWORD code)
{
	char* m = win_error_text(code);
	r->len = 0;
	r->data[0] = 0;
	sb_rec_begin(r, "RESULT");
	sb_kv_str(r, "status", "ERROR");
	sb_kv_i64(r, "code", (__int64)code);
	sb_kv_str(r, "source", "win32");
	sb_kv_str(r, "message", m);
	sb_rec_end(r);
	free(m);
	return 0;
}

static unsigned __int64 filetime_to_unix(const FILETIME* ft)
{
	ULARGE_INTEGER u;
	u.LowPart = ft->dwLowDateTime;
	u.HighPart = ft->dwHighDateTime;
	return (u.QuadPart - 116444736000000000ULL) / 10000000ULL;
}

static int reg_read_sz(const wchar_t* sub, const wchar_t* name, wchar_t* out,
					   DWORD cch)
{
	HKEY k;
	DWORD type = 0, sz = cch * sizeof(wchar_t);
	int ok = 0;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, sub, 0, KEY_READ | KEY_WOW64_64KEY,
					  &k) == ERROR_SUCCESS)
	{
		if (RegQueryValueExW(k, name, NULL, &type, (BYTE*)out, &sz) ==
				ERROR_SUCCESS &&
			(type == REG_SZ))
		{
			out[cch - 1] = 0;
			ok = 1;
		}
		RegCloseKey(k);
	}
	if (!ok)
		out[0] = 0;
	return ok;
}

/* ------------------------------------------------------------------ */
/* GET_OS                                                              */
/* ------------------------------------------------------------------ */
typedef LONG(WINAPI* RtlGetVersion_fn)(RTL_OSVERSIONINFOEXW*);

static const char* friendly_os_name(DWORD maj, DWORD min, DWORD build,
									int workstation)
{
	if (maj == 10)
	{
		if (workstation)
			return build >= 22000 ? "Windows 11" : "Windows 10";
		if (build >= 26100)
			return "Windows Server 2025";
		if (build >= 20348)
			return "Windows Server 2022";
		if (build >= 17763)
			return "Windows Server 2019";
		return "Windows Server 2016";
	}
	if (maj == 6 && min == 3)
		return workstation ? "Windows 8.1" : "Windows Server 2012 R2";
	if (maj == 6 && min == 2)
		return workstation ? "Windows 8" : "Windows Server 2012";
	if (maj == 6 && min == 1)
		return workstation ? "Windows 7" : "Windows Server 2008 R2";
	if (maj == 6 && min == 0)
		return workstation ? "Windows Vista" : "Windows Server 2008";
	if (maj == 5 && min == 2)
		return "Windows Server 2003 / XP x64";
	if (maj == 5 && min == 1)
		return "Windows XP";
	if (maj == 5 && min == 0)
		return "Windows 2000";
	return "Windows (unknown)";
}

static int cmd_os(sbuf* r)
{
	/* GetVersionEx на Windows 8.1+ без манифеста врёт, поэтому берём
	 * RtlGetVersion из ntdll */
	RTL_OSVERSIONINFOEXW v;
	RtlGetVersion_fn f = (RtlGetVersion_fn)GetProcAddress(
		GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
	memset(&v, 0, sizeof(v));
	v.dwOSVersionInfoSize = sizeof(v);
	if (!f || f(&v) != 0)
		return result_proto_error(r, 100, "cannot determine OS version");

	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);
	const char* arch = "unknown";
	switch (si.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_AMD64:
		arch = "x64";
		break;
	case PROCESSOR_ARCHITECTURE_INTEL:
		arch = "x86";
		break;
	case PROCESSOR_ARCHITECTURE_ARM:
		arch = "arm";
		break;
	case 12:
		arch = "arm64";
		break;
	}

	int workstation = (v.wProductType == VER_NT_WORKSTATION);
	const char* ptype =
		workstation
			? "workstation"
			: (v.wProductType == VER_NT_DOMAIN_CONTROLLER ? "domain_controller"
														  : "server");

	wchar_t product[256], dispver[64], comp[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD csz = MAX_COMPUTERNAME_LENGTH + 1;
	const wchar_t* key = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";
	reg_read_sz(key, L"ProductName", product, 256);
	if (!reg_read_sz(key, L"DisplayVersion", dispver, 64))
		reg_read_sz(key, L"ReleaseId", dispver, 64);
	if (!GetComputerNameW(comp, &csz))
		comp[0] = 0;

	result_ok(r, "GET_OS");
	sb_rec_begin(r, "OS");
	sb_kv_str(r, "name",
			  friendly_os_name(v.dwMajorVersion, v.dwMinorVersion,
							   v.dwBuildNumber, workstation));
	sb_kv_wstr(r, "product_name", product);
	sb_kv_wstr(r, "display_version", dispver);
	sb_kv_u64(r, "major", v.dwMajorVersion);
	sb_kv_u64(r, "minor", v.dwMinorVersion);
	sb_kv_u64(r, "build", v.dwBuildNumber);
	sb_kv_u64(r, "platform_id", v.dwPlatformId);
	sb_kv_wstr(r, "service_pack", v.szCSDVersion);
	sb_kv_u64(r, "sp_major", v.wServicePackMajor);
	sb_kv_u64(r, "sp_minor", v.wServicePackMinor);
	sb_kv_str(r, "product_type", ptype);
	sb_kv_str(r, "arch", arch);
	sb_kv_wstr(r, "computer_name", comp);
	sb_rec_end(r);
	return 1;
}

/* ------------------------------------------------------------------ */
/* GET_TIME / GET_UPTIME                                               */
/* ------------------------------------------------------------------ */
static void time_record(sbuf* r, const char* scope, const SYSTEMTIME* t,
						unsigned __int64 unix_s, const char* offset_iso,
						int has_tz, LONG offset_min, const wchar_t* tzname)
{
	char iso[64];
	sprintf(iso, "%04u-%02u-%02uT%02u:%02u:%02u.%03u%s", t->wYear, t->wMonth,
			t->wDay, t->wHour, t->wMinute, t->wSecond, t->wMilliseconds,
			offset_iso);
	sb_rec_begin(r, "TIME");
	sb_kv_str(r, "scope", scope);
	sb_kv_str(r, "iso", iso);
	sb_kv_u64(r, "year", t->wYear);
	sb_kv_u64(r, "month", t->wMonth);
	sb_kv_u64(r, "day", t->wDay);
	sb_kv_u64(r, "hour", t->wHour);
	sb_kv_u64(r, "minute", t->wMinute);
	sb_kv_u64(r, "second", t->wSecond);
	sb_kv_u64(r, "millisecond", t->wMilliseconds);
	sb_kv_u64(r, "day_of_week", t->wDayOfWeek); /* 0 = воскресенье */
	sb_kv_u64(r, "unix", unix_s);
	if (has_tz)
	{
		sb_kv_i64(r, "utc_offset_minutes", offset_min);
		sb_kv_wstr(r, "tz_name", tzname);
	}
	sb_rec_end(r);
}

static int cmd_time(sbuf* r)
{
	SYSTEMTIME ut, lt;
	FILETIME ft;
	GetSystemTime(&ut);
	GetLocalTime(&lt);
	SystemTimeToFileTime(&ut, &ft);
	unsigned __int64 unix_s = filetime_to_unix(&ft);

	TIME_ZONE_INFORMATION tz;
	DWORD tzr = GetTimeZoneInformation(&tz);
	LONG bias = tz.Bias;
	const wchar_t* tzname = tz.StandardName;
	if (tzr == TIME_ZONE_ID_DAYLIGHT)
	{
		bias += tz.DaylightBias;
		tzname = tz.DaylightName;
	}
	else if (tzr != TIME_ZONE_ID_INVALID)
	{
		bias += tz.StandardBias;
	}
	LONG offset = -bias;
	char off_iso[16];
	sprintf(off_iso, "%c%02ld:%02ld", offset < 0 ? '-' : '+', labs(offset) / 60,
			labs(offset) % 60);

	result_ok(r, "GET_TIME");
	time_record(r, "utc", &ut, unix_s, "Z", 0, 0, NULL);
	time_record(r, "local", &lt, unix_s, off_iso, 1, offset, tzname);
	return 1;
}

static int cmd_uptime(sbuf* r)
{
	unsigned __int64 ms = GetTickCount64(); /* в отличие от GetTickCount не
											   переполняется за 49 суток */
	SYSTEMTIME now;
	FILETIME ft;
	GetSystemTime(&now);
	SystemTimeToFileTime(&now, &ft);

	ULARGE_INTEGER u;
	u.LowPart = ft.dwLowDateTime;
	u.HighPart = ft.dwHighDateTime;
	u.QuadPart -= ms * 10000ULL;
	ft.dwLowDateTime = u.LowPart;
	ft.dwHighDateTime = u.HighPart;
	SYSTEMTIME boot;
	FileTimeToSystemTime(&ft, &boot);
	char iso[64];
	sprintf(iso, "%04u-%02u-%02uT%02u:%02u:%02uZ", boot.wYear, boot.wMonth,
			boot.wDay, boot.wHour, boot.wMinute, boot.wSecond);

	result_ok(r, "GET_UPTIME");
	sb_rec_begin(r, "UPTIME");
	sb_kv_u64(r, "milliseconds", ms);
	sb_kv_str(r, "boot_time_utc", iso);
	sb_kv_u64(r, "boot_time_unix", filetime_to_unix(&ft));
	sb_rec_end(r);
	return 1;
}

/* ------------------------------------------------------------------ */
/* GET_MEMORY                                                          */
/* ------------------------------------------------------------------ */
static int cmd_memory(sbuf* r)
{
	MEMORYSTATUSEX m;
	m.dwLength = sizeof(m);
	if (!GlobalMemoryStatusEx(&m))
		return result_win_error(r, GetLastError());

	result_ok(r, "GET_MEMORY");
	sb_rec_begin(r, "MEMORY");
	sb_kv_u64(r, "load_percent", m.dwMemoryLoad);
	sb_kv_u64(r, "total_phys", m.ullTotalPhys);
	sb_kv_u64(r, "avail_phys", m.ullAvailPhys);
	sb_kv_u64(r, "total_pagefile", m.ullTotalPageFile);
	sb_kv_u64(r, "avail_pagefile", m.ullAvailPageFile);
	sb_kv_u64(r, "total_virtual", m.ullTotalVirtual);
	sb_kv_u64(r, "avail_virtual", m.ullAvailVirtual);
	sb_rec_end(r);
	return 1;
}

/* ------------------------------------------------------------------ */
/* GET_DRIVES / GET_FREESPACE                                          */
/* ------------------------------------------------------------------ */
static void drive_type_names(UINT t, const char** type, const char** cls)
{
	switch (t)
	{
	case DRIVE_REMOVABLE:
		*type = "removable";
		*cls = "removable";
		break;
	case DRIVE_FIXED:
		*type = "fixed";
		*cls = "local";
		break;
	case DRIVE_REMOTE:
		*type = "network";
		*cls = "network";
		break;
	case DRIVE_CDROM:
		*type = "cdrom";
		*cls = "removable";
		break;
	case DRIVE_RAMDISK:
		*type = "ramdisk";
		*cls = "local";
		break;
	case DRIVE_NO_ROOT_DIR:
		*type = "no_root";
		*cls = "unknown";
		break;
	default:
		*type = "unknown";
		*cls = "unknown";
		break;
	}
}

static int cmd_drives(sbuf* r)
{
	wchar_t buf[512];
	DWORD n = GetLogicalDriveStringsW(512, buf);
	if (n == 0 || n > 512)
		return result_win_error(r, GetLastError());

	result_ok(r, "GET_DRIVES");
	for (const wchar_t* p = buf; *p; p += wcslen(p) + 1)
	{
		const char *type, *cls;
		wchar_t label[MAX_PATH + 1] = L"", fs[MAX_PATH + 1] = L"";
		DWORD serial = 0, maxc = 0, flags = 0;
		UINT dt = GetDriveTypeW(p);
		drive_type_names(dt, &type, &cls);
		int ready = GetVolumeInformationW(p, label, MAX_PATH + 1, &serial,
										  &maxc, &flags, fs, MAX_PATH + 1);
		sb_rec_begin(r, "DRIVE");
		sb_kv_wstr(r, "root", p);
		sb_kv_str(r, "type", type);
		sb_kv_str(r, "class", cls);
		sb_kv_wstr(r, "fs", ready ? fs : L"");
		sb_kv_wstr(r, "label", ready ? label : L"");
		sb_kv_str(r, "ready", ready ? "1" : "0");
		sb_rec_end(r);
	}
	return 1;
}

static int cmd_freespace(sbuf* r)
{
	wchar_t buf[512];
	DWORD n = GetLogicalDriveStringsW(512, buf);
	if (n == 0 || n > 512)
		return result_win_error(r, GetLastError());

	result_ok(r, "GET_FREESPACE");
	for (const wchar_t* p = buf; *p; p += wcslen(p) + 1)
	{
		if (GetDriveTypeW(p) != DRIVE_FIXED)
			continue; /* только локальные диски */
		ULARGE_INTEGER avail, total, freeb;
		sb_rec_begin(r, "FREESPACE");
		sb_kv_wstr(r, "root", p);
		if (GetDiskFreeSpaceExW(p, &avail, &total, &freeb))
		{
			sb_kv_u64(r, "total_bytes", total.QuadPart);
			sb_kv_u64(r, "free_bytes", freeb.QuadPart);
			sb_kv_u64(r, "free_to_user_bytes", avail.QuadPart);
		}
		else
		{
			sb_kv_i64(r, "error", (__int64)GetLastError());
		}
		sb_rec_end(r);
	}
	return 1;
}

/* ------------------------------------------------------------------ */
/* Права доступа и владелец                                            */
/* ------------------------------------------------------------------ */
typedef enum
{
	OBJ_FILE,
	OBJ_DIR,
	OBJ_REG
} objkind;

/* Разбор типа и пути; возвращает имя для GetNamedSecurityInfo (malloc) */
static wchar_t* resolve_object(const char* kind, const char* path,
							   SE_OBJECT_TYPE* se, objkind* ok_kind,
							   const char** err)
{
	wchar_t* w = utf8_to_wide(path);
	if (_stricmp(kind, "FILE") == 0)
	{
		*se = SE_FILE_OBJECT;
		DWORD a = GetFileAttributesW(w);
		*ok_kind =
			(a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY))
				? OBJ_DIR
				: OBJ_FILE;
		return w;
	}
	if (_stricmp(kind, "REG") == 0)
	{
		static const struct
		{
			const wchar_t* in;
			const wchar_t* out;
		} roots[] = {
			{L"HKLM", L"MACHINE"},
			{L"HKEY_LOCAL_MACHINE", L"MACHINE"},
			{L"MACHINE", L"MACHINE"},
			{L"HKCU", L"CURRENT_USER"},
			{L"HKEY_CURRENT_USER", L"CURRENT_USER"},
			{L"CURRENT_USER", L"CURRENT_USER"},
			{L"HKCR", L"CLASSES_ROOT"},
			{L"HKEY_CLASSES_ROOT", L"CLASSES_ROOT"},
			{L"CLASSES_ROOT", L"CLASSES_ROOT"},
			{L"HKU", L"USERS"},
			{L"HKEY_USERS", L"USERS"},
			{L"USERS", L"USERS"},
		};
		wchar_t* rest = wcschr(w, L'\\');
		size_t rootlen = rest ? (size_t)(rest - w) : wcslen(w);
		for (size_t i = 0; i < sizeof(roots) / sizeof(roots[0]); i++)
		{
			if (wcslen(roots[i].in) == rootlen &&
				_wcsnicmp(w, roots[i].in, rootlen) == 0)
			{
				size_t cap =
					wcslen(roots[i].out) + (rest ? wcslen(rest) : 0) + 1;
				wchar_t* res = (wchar_t*)malloc(cap * sizeof(wchar_t));
				wcscpy(res, roots[i].out);
				if (rest)
					wcscat(res, rest);
				free(w);
				*se = SE_REGISTRY_KEY;
				*ok_kind = OBJ_REG;
				return res;
			}
		}
		free(w);
		*err = "unknown registry root (use HKLM, HKCU, HKCR, HKU)";
		return NULL;
	}
	free(w);
	*err = "object type must be FILE or REG";
	return NULL;
}

/* SID -> строка и имя */
static void sid_describe(PSID sid, char** sidstr, char** name, const char** use)
{
	LPWSTR s = NULL;
	*sidstr = ConvertSidToStringSidW(sid, &s) ? wide_to_utf8(s) : _strdup("");
	if (s)
		LocalFree(s);

	wchar_t nm[256], dom[256];
	DWORD nl = 256, dl = 256;
	SID_NAME_USE u;
	if (LookupAccountSidW(NULL, sid, nm, &nl, dom, &dl, &u))
	{
		wchar_t full[600];
		if (dom[0])
			swprintf(full, 600, L"%ls\\%ls", dom, nm);
		else
			swprintf(full, 600, L"%ls", nm);
		*name = wide_to_utf8(full);
		switch (u)
		{
		case SidTypeUser:
			*use = "user";
			break;
		case SidTypeGroup:
			*use = "group";
			break;
		case SidTypeDomain:
			*use = "domain";
			break;
		case SidTypeAlias:
			*use = "alias";
			break;
		case SidTypeWellKnownGroup:
			*use = "well_known_group";
			break;
		case SidTypeDeletedAccount:
			*use = "deleted_account";
			break;
		case SidTypeInvalid:
			*use = "invalid";
			break;
		case SidTypeComputer:
			*use = "computer";
			break;
		case SidTypeLabel:
			*use = "label";
			break;
		default:
			*use = "unknown";
			break;
		}
	}
	else
	{
		*name = _strdup("");
		*use = "unresolved";
	}
}

static int cmd_owner(sbuf* r, const char* kind, const char* path)
{
	SE_OBJECT_TYPE se;
	objkind ok;
	const char* err = NULL;
	wchar_t* obj = resolve_object(kind, path, &se, &ok, &err);
	if (!obj)
		return result_proto_error(r, 2, err);

	PSID owner = NULL;
	PSECURITY_DESCRIPTOR sd = NULL;
	DWORD e = GetNamedSecurityInfoW(obj, se, OWNER_SECURITY_INFORMATION, &owner,
									NULL, NULL, NULL, &sd);
	free(obj);
	if (e != ERROR_SUCCESS)
		return result_win_error(r, e);
	if (!owner)
	{
		LocalFree(sd);
		return result_proto_error(r, 101, "object has no owner");
	}

	char *sidstr, *name;
	const char* use;
	sid_describe(owner, &sidstr, &name, &use);
	result_ok(r, "GET_OWNER");
	sb_rec_begin(r, "OWNER");
	sb_kv_str(r, "object_type",
			  ok == OBJ_REG ? "registry"
							: (ok == OBJ_DIR ? "directory" : "file"));
	sb_kv_str(r, "path", path);
	sb_kv_str(r, "sid", sidstr);
	sb_kv_str(r, "name", name);
	sb_kv_str(r, "name_use", use);
	sb_rec_end(r);
	free(sidstr);
	free(name);
	LocalFree(sd);
	return 1;
}

/* названия битов маски доступа (MSDN: File / Registry Key Security and Access
 * Rights) */
static const struct
{
	int bit;
	const char *file, *dir, *reg;
} g_bits[] = {
	{0, "FILE_READ_DATA", "FILE_LIST_DIRECTORY", "KEY_QUERY_VALUE"},
	{1, "FILE_WRITE_DATA", "FILE_ADD_FILE", "KEY_SET_VALUE"},
	{2, "FILE_APPEND_DATA", "FILE_ADD_SUBDIRECTORY", "KEY_CREATE_SUB_KEY"},
	{3, "FILE_READ_EA", "FILE_READ_EA", "KEY_ENUMERATE_SUB_KEYS"},
	{4, "FILE_WRITE_EA", "FILE_WRITE_EA", "KEY_NOTIFY"},
	{5, "FILE_EXECUTE", "FILE_TRAVERSE", "KEY_CREATE_LINK"},
	{6, "FILE_DELETE_CHILD", "FILE_DELETE_CHILD", NULL},
	{7, "FILE_READ_ATTRIBUTES", "FILE_READ_ATTRIBUTES", NULL},
	{8, "FILE_WRITE_ATTRIBUTES", "FILE_WRITE_ATTRIBUTES", NULL},
	{16, "DELETE", "DELETE", "DELETE"},
	{17, "READ_CONTROL", "READ_CONTROL", "READ_CONTROL"},
	{18, "WRITE_DAC", "WRITE_DAC", "WRITE_DAC"},
	{19, "WRITE_OWNER", "WRITE_OWNER", "WRITE_OWNER"},
	{20, "SYNCHRONIZE", "SYNCHRONIZE", "SYNCHRONIZE"},
	{24, "ACCESS_SYSTEM_SECURITY", "ACCESS_SYSTEM_SECURITY",
	 "ACCESS_SYSTEM_SECURITY"},
	{25, "MAXIMUM_ALLOWED", "MAXIMUM_ALLOWED", "MAXIMUM_ALLOWED"},
	{28, "GENERIC_ALL", "GENERIC_ALL", "GENERIC_ALL"},
	{29, "GENERIC_EXECUTE", "GENERIC_EXECUTE", "GENERIC_EXECUTE"},
	{30, "GENERIC_WRITE", "GENERIC_WRITE", "GENERIC_WRITE"},
	{31, "GENERIC_READ", "GENERIC_READ", "GENERIC_READ"},
};

static void mask_bits(DWORD mask, objkind k, sbuf* bits, sbuf* names)
{
	int first = 1;
	for (int b = 0; b < 32; b++)
	{
		if (!(mask & (1UL << b)))
			continue;
		const char* nm = NULL;
		for (size_t i = 0; i < sizeof(g_bits) / sizeof(g_bits[0]); i++)
		{
			if (g_bits[i].bit == b)
			{
				nm = (k == OBJ_FILE)
						 ? g_bits[i].file
						 : (k == OBJ_DIR ? g_bits[i].dir : g_bits[i].reg);
				break;
			}
		}
		char tmp[48];
		if (!nm)
		{
			sprintf(tmp, "UNKNOWN_BIT_%d", b);
			nm = tmp;
		}
		if (!first)
		{
			sb_append(bits, ",");
			sb_append(names, ",");
		}
		sb_printf(bits, "%d", b);
		sb_append(names, nm);
		first = 0;
	}
}

static const char* g_ace_types[] = {
	"ACCESS_ALLOWED_ACE_TYPE",
	"ACCESS_DENIED_ACE_TYPE",
	"SYSTEM_AUDIT_ACE_TYPE",
	"SYSTEM_ALARM_ACE_TYPE",
	"ACCESS_ALLOWED_COMPOUND_ACE_TYPE",
	"ACCESS_ALLOWED_OBJECT_ACE_TYPE",
	"ACCESS_DENIED_OBJECT_ACE_TYPE",
	"SYSTEM_AUDIT_OBJECT_ACE_TYPE",
	"SYSTEM_ALARM_OBJECT_ACE_TYPE",
	"ACCESS_ALLOWED_CALLBACK_ACE_TYPE",
	"ACCESS_DENIED_CALLBACK_ACE_TYPE",
	"ACCESS_ALLOWED_CALLBACK_OBJECT_ACE_TYPE",
	"ACCESS_DENIED_CALLBACK_OBJECT_ACE_TYPE",
	"SYSTEM_AUDIT_CALLBACK_ACE_TYPE",
	"SYSTEM_ALARM_CALLBACK_ACE_TYPE",
	"SYSTEM_AUDIT_CALLBACK_OBJECT_ACE_TYPE",
	"SYSTEM_ALARM_CALLBACK_OBJECT_ACE_TYPE",
	"SYSTEM_MANDATORY_LABEL_ACE_TYPE",
	"SYSTEM_RESOURCE_ATTRIBUTE_ACE_TYPE",
	"SYSTEM_SCOPED_POLICY_ID_ACE_TYPE",
};

/* Где в ACE лежит SID. Возвращает NULL, если формат не поддерживается. */
static PSID ace_sid(const BYTE* ace, DWORD* mask)
{
	const ACE_HEADER* h = (const ACE_HEADER*)ace;
	if (h->AceSize < 8)
		return NULL;
	*mask = *(const DWORD*)(ace + 4);
	switch (h->AceType)
	{
	case 0:
	case 1:
	case 2:
	case 3: /* ALLOWED / DENIED / AUDIT / ALARM */
	case 9:
	case 10:
	case 13:
	case 14: /* CALLBACK-варианты */
	case 17:
	case 18:
	case 19:
		return (PSID)(ace + 8);
	case 5:
	case 6:
	case 7:
	case 8: /* OBJECT-варианты */
	case 11:
	case 12:
	case 15:
	case 16:
	{
		if (h->AceSize < 12)
			return NULL;
		DWORD flags = *(const DWORD*)(ace + 8);
		DWORD off = 12;
		if (flags & ACE_OBJECT_TYPE_PRESENT)
			off += 16;
		if (flags & ACE_INHERITED_OBJECT_TYPE_PRESENT)
			off += 16;
		if (off >= h->AceSize)
			return NULL;
		return (PSID)(ace + off);
	}
	default:
		return NULL;
	}
}

/* Область действия ACE (как в диалоге "Применяется к") */
static const char* ace_scope(BYTE fl)
{
	int oi = (fl & OBJECT_INHERIT_ACE) != 0;
	int ci = (fl & CONTAINER_INHERIT_ACE) != 0;
	int io = (fl & INHERIT_ONLY_ACE) != 0;
	if (oi && ci)
		return io ? "subfolders_and_files_only"
				  : "this_folder_subfolders_and_files";
	if (ci)
		return io ? "subfolders_only" : "this_folder_and_subfolders";
	if (oi)
		return io ? "files_only" : "this_folder_and_files";
	return "this_object_only";
}

static void ace_flag_names(BYTE fl, sbuf* s)
{
	static const struct
	{
		BYTE bit;
		const char* n;
	} t[] = {
		{OBJECT_INHERIT_ACE, "OBJECT_INHERIT_ACE"},
		{CONTAINER_INHERIT_ACE, "CONTAINER_INHERIT_ACE"},
		{NO_PROPAGATE_INHERIT_ACE, "NO_PROPAGATE_INHERIT_ACE"},
		{INHERIT_ONLY_ACE, "INHERIT_ONLY_ACE"},
		{INHERITED_ACE, "INHERITED_ACE"},
	};
	int first = 1;
	for (size_t i = 0; i < sizeof(t) / sizeof(t[0]); i++)
	{
		if (fl & t[i].bit)
		{
			if (!first)
				sb_append(s, ",");
			sb_append(s, t[i].n);
			first = 0;
		}
	}
}

static int cmd_acl(sbuf* r, const char* kind, const char* path)
{
	SE_OBJECT_TYPE se;
	objkind ok;
	const char* err = NULL;
	wchar_t* obj = resolve_object(kind, path, &se, &ok, &err);
	if (!obj)
		return result_proto_error(r, 2, err);

	PACL dacl = NULL;
	PSECURITY_DESCRIPTOR sd = NULL;
	DWORD e = GetNamedSecurityInfoW(obj, se, DACL_SECURITY_INFORMATION, NULL,
									NULL, &dacl, NULL, &sd);
	free(obj);
	if (e != ERROR_SUCCESS)
		return result_win_error(r, e);

	SECURITY_DESCRIPTOR_CONTROL ctl = 0;
	DWORD rev;
	GetSecurityDescriptorControl(sd, &ctl, &rev);

	DWORD count = 0;
	if (dacl)
	{
		ACL_SIZE_INFORMATION info;
		if (GetAclInformation(dacl, &info, sizeof(info), AclSizeInformation))
			count = info.AceCount;
	}

	result_ok(r, "GET_ACL");
	sb_rec_begin(r, "ACL");
	sb_kv_str(r, "object_type",
			  ok == OBJ_REG ? "registry"
							: (ok == OBJ_DIR ? "directory" : "file"));
	sb_kv_str(r, "path", path);
	sb_kv_str(r, "null_dacl",
			  dacl ? "0" : "1"); /* NULL DACL = полный доступ всем */
	sb_kv_str(r, "dacl_protected", (ctl & SE_DACL_PROTECTED) ? "1" : "0");
	sb_kv_u64(r, "ace_count", count);
	sb_rec_end(r);

	for (DWORD i = 0; i < count; i++)
	{
		void* pace = NULL;
		if (!GetAce(dacl, i, &pace) || !pace)
			continue;
		const ACE_HEADER* h = (const ACE_HEADER*)pace;
		DWORD mask = 0;
		PSID sid = ace_sid((const BYTE*)pace, &mask);

		sb_rec_begin(r, "ACE");
		sb_kv_u64(r, "index", i);
		if (sid && IsValidSid(sid))
		{
			char *sidstr, *name;
			const char* use;
			sid_describe(sid, &sidstr, &name, &use);
			sb_kv_str(r, "sid", sidstr);
			sb_kv_str(r, "name", name);
			sb_kv_str(r, "name_use", use);
			free(sidstr);
			free(name);
		}
		else
		{
			sb_kv_str(r, "sid", "");
			sb_kv_str(r, "name", "");
			sb_kv_str(r, "name_use", "unsupported");
		}
		sb_kv_str(r, "ace_type",
				  h->AceType < sizeof(g_ace_types) / sizeof(g_ace_types[0])
					  ? g_ace_types[h->AceType]
					  : "UNKNOWN_ACE_TYPE");
		sb_kv_u64(r, "ace_type_id", h->AceType);
		sb_kv_hex(r, "ace_flags", h->AceFlags);
		sbuf fl;
		sb_init(&fl);
		ace_flag_names(h->AceFlags, &fl);
		sb_kv_str(r, "ace_flag_names", fl.data);
		sb_free(&fl);
		sb_kv_str(r, "scope", ace_scope(h->AceFlags));
		sb_kv_hex(r, "mask", mask);
		sbuf bits, names;
		sb_init(&bits);
		sb_init(&names);
		mask_bits(mask, ok, &bits, &names);
		sb_kv_str(r, "bits", bits.data);
		sb_kv_str(r, "bit_names", names.data);
		sb_free(&bits);
		sb_free(&names);
		sb_rec_end(r);
	}
	LocalFree(sd);
	return 1;
}

/* ------------------------------------------------------------------ */
/* Разбор запроса                                                      */
/* ------------------------------------------------------------------ */
#define MAX_ARGS 3

/* Разбить запрос на токены и снять экранирование. Возвращает копию (free),
 * токены указывают в неё. */
static char* split_request(const char* req, char* tok[MAX_ARGS], int* ntok)
{
	char* copy = _strdup(req);
	*ntok = 0;
	if (!copy)
		return NULL;
	size_t l = strlen(copy);
	while (l > 0 && (copy[l - 1] == '\n' || copy[l - 1] == '\r'))
		copy[--l] = 0;
	char* p = copy;
	while (p && *ntok < MAX_ARGS)
	{
		char* next = strchr(p, '\t');
		if (next)
			*next++ = 0;
		unescape_inplace(p);
		tok[(*ntok)++] = p;
		p = next;
	}
	return copy;
}

void request_summary(const char* req, char* out, size_t outsz)
{
	char* tok[MAX_ARGS];
	int n;
	char* copy = split_request(req, tok, &n);
	out[0] = 0;
	if (!copy)
		return;
	for (int i = 0; i < n; i++)
	{
		if (i)
			strncat(out, " ", outsz - strlen(out) - 1);
		strncat(out, tok[i], outsz - strlen(out) - 1);
	}
	free(copy);
}

int process_request(const char* req, sbuf* r)
{
	char* tok[MAX_ARGS];
	int n;
	char* copy = split_request(req, tok, &n);
	int res;
	if (!copy || n == 0 || tok[0][0] == 0)
	{
		free(copy);
		return result_proto_error(r, 1, "empty request");
	}

	const char* cmd = tok[0];
	if (_stricmp(cmd, "GET_OS") == 0)
		res = cmd_os(r);
	else if (_stricmp(cmd, "GET_TIME") == 0)
		res = cmd_time(r);
	else if (_stricmp(cmd, "GET_UPTIME") == 0)
		res = cmd_uptime(r);
	else if (_stricmp(cmd, "GET_MEMORY") == 0)
		res = cmd_memory(r);
	else if (_stricmp(cmd, "GET_DRIVES") == 0)
		res = cmd_drives(r);
	else if (_stricmp(cmd, "GET_FREESPACE") == 0)
		res = cmd_freespace(r);
	else if (_stricmp(cmd, "GET_ACL") == 0 || _stricmp(cmd, "GET_OWNER") == 0)
	{
		if (n < 3 || tok[2][0] == 0)
		{
			res =
				result_proto_error(r, 2, "usage: <command> <FILE|REG> <path>");
		}
		else if (_stricmp(cmd, "GET_ACL") == 0)
		{
			res = cmd_acl(r, tok[1], tok[2]);
		}
		else
		{
			res = cmd_owner(r, tok[1], tok[2]);
		}
	}
	else
	{
		res = result_proto_error(r, 1, "unknown command");
	}
	free(copy);
	return res;
}
