#include "stdafx.h"
#include <vector>
#include "GMLInternals.h"
#include "MemTools.h"
#include "DebugTools.h"
#include "GMLInternals.h"
#include <cstdint>
#include "detours.h"
#include <cmath>

// Helper function for installing hooks,
// returns null if the hook failed, otherwise returns the original hooked method
void* InstallHook(std::vector<BYTE> dat, void* hookFunction) {
    BYTE* addr = MemTools::scan(dat);

    if (addr == nullptr) {
        // Failed to find function to hook
        return NULL;
    }

    // Store it
    void* methodOrig = addr;

    // Attach with Detours
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&methodOrig, hookFunction);
    DetourTransactionCommit();

    return methodOrig;
}

/*
// This hook is called for per-instance events,
// as of now there is no way to tell the calling instance, so its largely useless
struct GMRawEventInfo {
    uint8_t event_type;
    uint8_t UNK1;
    uint8_t UNK2;
    uint8_t UNK3;
    uint8_t UNK4;
    uint8_t UNK5;
    uint8_t UNK6;
    uint8_t UNK7;
    uint8_t event_subtype;
};
// The original function
bool (*GMLMainEventHandler)(void*, void*, void*, GMRawEventInfo*, uint32_t);
// What we're hooking it with
bool EventHandlerHook(void* a1, void* a2, void* a3, GMRawEventInfo* a4, INT32 a5) {
    // This function handles dispatching events to *individual* instances...
    // Figure out global event handler
	// Call the original function
	return GMLMainEventHandler(a1, a2, a3, a4, a5);
}

// Setup
std::string InitGMLHook() {
	// Search for the function we need
	std::vector<BYTE> dat{
		0x83, 0xEC, 0x0C,
		0x80, 0x3D, '?', '?', '?', '?', 0x00,
		0x53,
		0x57,
		0x0F, 0x84, '?', '?', '?', '?',
		0x56,
		0x8B, 0x74, 0x24, 0x24,
		0xC6, 0x05, '?', '?', '?', '?', 0x00,
		0x8B, 0x46, 0x08,
		0x85, 0xC0,
		0x0F, 0x8E, '?', '?', '?', '?',
		0x83, 0xF8, 0x02,
		0x0F, 0x8E, '?', '?', '?', '?',
		0x83, 0xF8, 0x03,
		0x0F, 0x85, '?', '?', '?', '?',
		0x8B, 0x7C, 0x24, 0x28,
		0x8D, 0x5E, 0x40
	};
	BYTE* addr = MemTools::scan(dat);
	if (addr == nullptr) {
		return "Failed to hook GML event handler.";
	}

	// Store it
	GMLMainEventHandler = (bool(*)(void*, void*, void*, GMRawEventInfo*, INT32))addr;

	// Attach with Detours
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)GMLMainEventHandler, EventHandlerHook);
	DetourTransactionCommit();

	return "";
}
//*/

// The original function
bool(*GMLStepEventDispatcherOrig)(INT32);
// User hook
void(*UserStepEventDispatcherHook)(int);
// What we're hooking it with
bool GMLStepEventDispatcherHook(INT32 event_subtype) {
    UserStepEventDispatcherHook(event_subtype);
    return GMLStepEventDispatcherOrig(event_subtype);
}

// Hooks the GameMaker step event using the passed function.
// The function is passed the step event subtype:
// 0 - Normal step
// 1 - Begin step
// 2 - End step
// Returns whether the hook was successful.
bool GMLHookGlobalStep(void(* hook)(int event_subtype)) {
    std::vector<BYTE> hookData {
        0x55,
            0x8B, 0xEC,
            0x6A, 0xFF,
            0x68, '?', '?', '?', '?',
            0x64, 0xA1, 0x00, 0x00, 0x00, 0x00,
            0x50,
            0x83, 0xEC, 0x10,
            0x53,
            0x56,
            0x57,
            0xA1, '?', '?', '?', '?',
            0x33, 0xC5,
            0x50,
            0x8D, 0x45, 0xF4,
            0x64, 0xA3, 0x00, 0x00, 0x00, 0x00,
            0x80, 0x3D, '?', '?', '?', '?', 0x00,
            0x74, 0x0E,
            0x6A, 0x04,
            0x6A, 0x06,
            0xB9, '?', '?', '?', '?',
            0xE8, '?', '?', '?', '?'
    };
    GMLStepEventDispatcherOrig = (bool(*)(INT32))InstallHook(hookData, (void*)GMLStepEventDispatcherHook);
    UserStepEventDispatcherHook = hook;
    return GMLStepEventDispatcherOrig != NULL;
}