// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <windows.h>
#include "GMLInternals.h"
#include "SpriteHelper.h"
#include "RoomHelper.h"
#include "detours.h"

void TestMod_start();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  reason, LPVOID lpReserved) {
	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	if (reason == DLL_PROCESS_ATTACH) {
		std::string result = GMLInternals::__InitialSetup();
		if (result != "") {
			// Failed to load, for whatever reason
			MessageBoxA(GetActiveWindow(), ("Failed to initialize YYC hooking DLL:\n" + result).c_str(), "Failed to start YYCHook", MB_OK | MB_ICONERROR);
		} else {

			SpriteHelper::__InitialSetup();
			RoomHelper::__InitialSetup();

			SpriteHelper::spriteLoaderMod();

			TestMod_start();
		}
	}

    return TRUE;
}

