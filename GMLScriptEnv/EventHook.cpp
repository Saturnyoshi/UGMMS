#include "stdafx.h"
#include <vector>
#include "gml.h"
#include "GMLInternals.h"
#include "MemTools.h"
#include "DebugTools.h"
#include <unordered_map>
#include "GMLInternals.h"
#include <cstdint>

/*int8_t(*TrueStep)(int32_t, int32_t);
int8_t EVHook_Step(int32_t a1, int32_t a2) {
	Bawks("hello");
	return TrueStep(a1, a2);
}*/

/*void HookASMFunction(std::string name, std::vector<BYTE> dat, int8_t(*func)(int32_t, int32_t), int copyBytes) {
	BYTE* addr = MemTools::scan(dat);
	if (addr == nullptr) {
		Bawks("Failed to hook " + name);
		return;
	}
	TrueStep = (int8_t(*)(int32_t, int32_t))addr;
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)TrueStep, func);
	DetourTransactionCommit();
}*/

/*void HookStepEvent() {
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

	HookASMFunction("step", dat, EVHook_Step, 10);
}*/


void InitGMLHooks() {

	//HookStepEvent();
	
}