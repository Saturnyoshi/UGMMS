#include "stdafx.h"
#include <vector>
#include "GMLInternals.h"
#include "MemTools.h"
#include "DebugTools.h"
#include "GMLInternals.h"
#include <cstdint>
#include "detours.h"
#include <sstream>

struct GMEventDef {
    int32_t UNK1;
    int32_t UNK2;
    int32_t UNK3;
    int32_t UNK4;
    int32_t UNK5;
    int32_t UNK6;
    int32_t UNK7;
    int32_t UNK8;
    int32_t UNK9;
    int32_t UNK10;
    int32_t UNK11;
    int32_t UNK12;
    int32_t UNK13;
    int32_t UNK14;
    int32_t UNK15;
    int32_t UNK16;
    int32_t UNK17;
    int32_t UNK18;
    int32_t UNK19;
    int32_t UNK20;
    uint8_t event_subtype;
    uint8_t event_type;
    // ...
};

// The original function
bool (*GMLMainEventHandler)(int32_t, GMEventDef*, int32_t, int32_t, int32_t);
// What we're hooking it with
int i = 0;
bool EventHandlerHook(int32_t a1, GMEventDef* a2, int32_t a3, int32_t a4, int32_t a5) {
	// Figure out how to tell events apart
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
	GMLMainEventHandler = (bool(*)(int32_t, GMEventDef*, int32_t, int32_t, int32_t))addr;

	// Attach with Detours
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)GMLMainEventHandler, EventHandlerHook);
	DetourTransactionCommit();

	return "";
}