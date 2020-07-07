#include "stdafx.h"
#include <vector>
#include "GMLInternals.h"
#include "MemTools.h"
#include "DebugTools.h"
#include "GMLInternals.h"
#include <cstdint>
#include "detours.h"
#include <sstream>

struct GMEventInfo {
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
bool (*GMLMainEventHandler)(uint32_t, uint32_t, uint32_t, GMEventInfo*, uint32_t);
// What we're hooking it with
bool EventHandlerHook(uint32_t a1, uint32_t a2, uint32_t a3, GMEventInfo* a4, uint32_t a5) {
    // This function handles dispatching events to *individual* instances...
    // Figure out global event handler
    /*
    uint8_t event_type = a4->event_type;
    uint8_t event_subtype = a4->event_subtype;
    */
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
	GMLMainEventHandler = (bool(*)(uint32_t, uint32_t, uint32_t, GMEventInfo*, uint32_t))addr;

	// Attach with Detours
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)GMLMainEventHandler, EventHandlerHook);
	DetourTransactionCommit();

	return "";
}