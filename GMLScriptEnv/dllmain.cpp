// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <windows.h>
#include "GMLInternals.h"
#include "detours.h"
#include "LuaScriptEnv.h"

static bool hookIsReady = false;
static bool hookHasRan = false;
HWND(WINAPI* CreateWindowExW_base)(
	DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
	int X, int Y, int nWidth, int nHeight,
	HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam
	);
HWND WINAPI CreateWindowExW_hook(
	DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
	int X, int Y, int nWidth, int nHeight,
	HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam
) {
	if (!hookIsReady) do {
		if (lpWindowName == nullptr) continue; // GM window gets a name, even if a blank one
		if (lpClassName == nullptr) continue; // GM window gets a class name
		if ((UINT32)lpClassName < 0x10000) continue; // that's an atom, not a class name string
		if (std::wstring(lpClassName) != L"YYGameMakerYY") continue; // finally, check the class
		hookIsReady = true;
		//
		MessageBoxA(GetActiveWindow(), "You can pick Debug > Attach to Process in VS now!", "GMLScriptEnv", MB_OK | MB_ICONINFORMATION);
        if (!hookHasRan) {
            hookHasRan = true;
		    std::string result = GMLInternals::__InitialSetup();
		    if (result != "") {
			    // Failed to load, for whatever reason
			    MessageBoxA(GetActiveWindow(), ("Failed to initialize YYC hooking DLL:\n" + result).c_str(), "Failed to start YYCHook", MB_OK | MB_ICONERROR);
                continue;
		    } else {
                LuaScriptEnv::startWithDefaultFile("startup.lua");
            }
        }
	} while (false);
	return CreateWindowExW_base(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  reason, LPVOID lpReserved) {
	if (DetourIsHelperProcess()) {
		return TRUE;
	}
	if (reason == DLL_PROCESS_ATTACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		CreateWindowExW_base = CreateWindowExW;
		DetourAttach(&(PVOID&)CreateWindowExW_base, CreateWindowExW_hook);
		if (DetourTransactionCommit() == NO_ERROR) {
			// OK!
            if (!hookHasRan) {
                hookHasRan = true;
                std::string result = GMLInternals::__InitialSetup();
                if (result != "") {
                    // Failed to load, for whatever reason
                    MessageBoxA(GetActiveWindow(), ("Failed to initialize YYC hooking DLL:\n" + result).c_str(), "Failed to start YYCHook", MB_OK | MB_ICONERROR);
                } else {
                    LuaScriptEnv::startWithDefaultFile("startup.lua");
                }
            }
		} else MessageBoxA(NULL, "Failed to detour CreateWindowExW!", "GMLScriptEnv", MB_OK | MB_ICONERROR);
	}
    return TRUE;
}

/// Required for DetourCreateProcessWithDllEx
BOOL APIENTRY Detours_1() {
	return TRUE;
}