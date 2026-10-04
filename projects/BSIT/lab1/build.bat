@echo off
rem Сборка в "x64 Native Tools Command Prompt for VS" (или x86 - тогда получатся 32-битные exe).
rem Флаг /utf-8 нужен, чтобы русские строки в исходниках не портились.

cl /nologo /O2 /W3 /utf-8 /MT server.c sysinfo.c common.c /Fe:server.exe ws2_32.lib mswsock.lib advapi32.lib
if errorlevel 1 exit /b 1

cl /nologo /O2 /W3 /utf-8 /MT client.c common.c /Fe:client.exe ws2_32.lib advapi32.lib
if errorlevel 1 exit /b 1

echo.
echo Готово: server.exe, client.exe
