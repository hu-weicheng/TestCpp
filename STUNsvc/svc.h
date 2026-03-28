#pragma once
#include <windows.h>

//LPCWSTR SERVICE_NAME = LPCWSTR("STUNMANService");
#define SERVICE_NAME L"STUNMANService"

extern void WINAPI ServiceHandler(DWORD fdwControl);
extern void WINAPI ServiceMain(int argc, char** argv);
extern BOOL InstallService();
extern BOOL UninstallService();
extern void LogEvent(LPCTSTR pFormat, ...);
