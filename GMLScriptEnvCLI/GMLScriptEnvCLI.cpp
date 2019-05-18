#include "pch.h"
#include <stdio.h>
#include <string>
#include <Windows.h>
#include "detours.h"

using std::wstring;
int wmain(int argc, wchar_t* argv[])
{
	if (argc <= 1) {
		wprintf(L"Hello! Please drag your desired executable on this one,\n"
				L"or run as `GMLScriptEnvCLI your.exe arguments`!\n"
				L"For now, press any key to exit.");
		getchar();
	} else {
		STARTUPINFO startinfo = { sizeof(startinfo) };
		PROCESS_INFORMATION procinfo = {};
		//
		wchar_t modulePathW[MAX_PATH];
		GetModuleFileName(NULL, modulePathW, MAX_PATH);
		wstring modulePath = modulePathW;
		auto slashPos = modulePath.find_last_of(L"\\/");
		wstring moduleDir = modulePath.substr(0, slashPos);
		//
		wstring targetPath = argv[1];
		slashPos = targetPath.find_last_of(L"\\/");
		wstring targetDir = targetPath.substr(0, slashPos);
		//
		wstring dllPath0 = moduleDir; dllPath0.append(L"\\GMLScriptEnv.dll");
		wstring dllPath1 = targetDir; dllPath1.append(L"\\GMLScriptEnv.dll");
		if (!CopyFile(dllPath0.c_str(), dllPath1.c_str(), false)) {
			wprintf(L"Failed to copy DLL from %s to %s", dllPath0.c_str(), dllPath1.c_str());
			return 1;
		}
		//
		auto ok = DetourCreateProcessWithDllEx(
			targetPath.c_str(), // path
			nullptr, // command-line
			NULL, // process attributes
			NULL, // thread attributes
			FALSE, // inherit handles
			0, // creation flags
			NULL, // environment
			NULL, // current directory
			&startinfo, // startup info
			&procinfo, // process information
			"GMLScriptEnv.dll",
			NULL
		);
		if (!ok) {
			wprintf(L"Error %d\n", GetLastError());
		} else wprintf(L"Starting up!\n");
	}
	return 0;
}