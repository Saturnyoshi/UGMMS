#include "stdafx.h"
#include <vector>
#include "gml.h"
#include "GMLInternals.h"
#include "MemTools.h"
#include "DebugTools.h"
#include <unordered_map>
#include "GMLInternals.h"

// Used for getting and setting variables
/*bool(*GMLVarGetDirect)(GMLInstanceBase* inst, int variableID, int arrayIndex, GMLVarBase* out);
bool(*GMLVarSetDirect)(GMLInstanceBase* inst, int variableID, int arrayIndex, GMLVarBase* out);
void(*GMLVarGetGlobal)(int variableID, int arrayIndex, GMLVarBase* out);
bool(*GMLVarSetGlobal)(int variableID, int arrayIndex, GMLVarBase* out);
bool(*GMLVarGet)(int obj, int variableID, int arrayIndex, GMLVarBase* out);
bool(*GMLVarSet)(int obj, int variableID, int arrayIndex, GMLVarBase* out);*/

namespace GMLInternals {
	// Map of function name -> id
	std::unordered_map<std::string, int>* functionIDMap = new std::unordered_map<std::string, int>();
	// Function to call built-in GML functions
	GMLVar* (*GMLLegacyCall)(GMLInstance* instSelf, GMLInstance* instOther, GMLVar& gmlResult, int argumentNumber, int functionID, GMLVar** arguments) = NULL;

	//////////////////////////////////////////////////////////////////////////////
	// SETUP /////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	// Sets up functionIDMap
	std::string readFunctionInfo(BYTE* ptr) {
		// Observing a near empty compiled GM project, each function definition is stored as:
		// 64 bytes: function name (remaining bytes are 0'd out)
		// 4 bytes: address of function?
		// 1 byte: argument count?
		// 3 bytes: padding (0)
		// 1 byte: unknown
		// 3 bytes: padding (0)
		// 4 bytes of 0xFF
		// For a total of 80 bytes each
		// However, the size is not consistent in some games
		// Function list ends when a zero length name is hit.
		BYTE* funcArray = *(BYTE**)ptr;

		// Automatically determine size of a function entry
		// Different between test project and Rivals of Aether, can't be hardcoded
		std::vector<BYTE> entryTerminator{ 0xFF, 0xFF, 0xFF, 0xFF };
		BYTE* ptrEntryTerminator = MemTools::scanLocal(entryTerminator, funcArray, 120);
		if (ptrEntryTerminator == NULL) {
			// Failed to find entry terminator
			return "Malformed function list";
		}
		int entrySize = (int)(ptrEntryTerminator + 4 - funcArray);

		int index = 0; // Current index of function being read
		while (true) {
			// Read the next name
			const char* name = (const char*)(funcArray + entrySize * index);
			if (strlen(name) == 0) {
				// No name - End of function list
				break;
			}
			else {
				// Add the name to the function map
				functionIDMap->insert({ std::string(name), index });
				index += 1;
			}
		}

		return "";
	}

	//GMLScriptPtr (*GetScriptAddress)(INT32 index) = nullptr;
	std::string __InitialSetup() {
		// Returns a string if loading failed
		if (GMLLegacyCall != NULL) return "";

		////// Find address of callLegacyFunction
		// Contains the ASM of the function with inconsistent bits replaced with '?'
		std::vector<BYTE> binLegacyCallFirst{
			0x55, 0x8B, 0xEC, 0x6A, 0xFF, '?', '?', '?', '?', '?', 0x64, 0xA1, 0x00, 0x00, 0x00, 0x00,
			0x50, 0x83, 0xEC, 0x14, 0xA1, '?', '?', '?', '?', 0x33, 0xC5, 0x89, 0x45, 0xF0, 0x53, 0x56,
			0x57, 0x50, 0x8D, 0x45, 0xF4, 0x64, 0xA3, 0x00, 0x00, 0x00, 0x00, '?', 0x45, 0x18
		};
		// Search for the function
		BYTE* legacyCallAddr = MemTools::scan(binLegacyCallFirst);
		if (legacyCallAddr == NULL) {
			// Failed to find it
			return "Failed to find CallLegacyFunction";
		}
		GMLLegacyCall = (GMLVar* (*)(GMLInstance*, GMLInstance*, GMLVar&, int, int, GMLVar**))legacyCallAddr;

		////// Search for second ASM chunk containing pointer to pointer to function array
		// '?'d out bit here is the pointer we need
		std::vector<BYTE> binLegacyCallSecond{
			0x03, 0x05, '?', '?', '?', '?', 0x89, 0x45,
			0xE4, 0x8B, 0xC7, 0xC1, 0xE0, 0x04, 0xE8
		};
		// Search for it starting from the end of the first chunk
		BYTE* legacyCallChunkAddr = MemTools::scanLocal(binLegacyCallSecond, legacyCallAddr + binLegacyCallFirst.size(), 64);
		if (GMLLegacyCall == nullptr) {
			// Failed to find second half
			GMLLegacyCall = NULL;
			return "Malformed CallLegacyFunction";
		}
		// Pull the address from the ASM
		BYTE* funcArrayAddrBase = MemTools::readPointer(legacyCallChunkAddr + 2);
		// Initialize function array
		std::string readFuncResult = readFunctionInfo(funcArrayAddrBase);
		if (readFuncResult != "") {
			// Failed to load function list
			GMLLegacyCall = NULL;
			return readFuncResult;
		}

		// Finds function used to get script pointers from id
		// This function might return some struct, needs more research before it can be used
		/*std::vector<BYTE> getScrPtrDat{
			 0x8B, 0x4C, 0x24, 0x04,
			 0x85, 0xC9,
			 0x78, 0x11,
			 0x3B, 0x0D, '?', '?', '?', '?',
			 0x7D, 0x09,
			 0xA1, '?', '?', '?', '?',
			 0x8B, 0x04, 0x88,
			 0xC3,
			 0x33, 0xC0,
			 0xC3
		};
		GetScriptAddress = (GMLScriptPtr(*)(int))MemTools::scan(getScrPtrDat);*/

		return "";
	}
	
	//////////////////////////////////////////////////////////////////////////////
	// METHODS ///////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	int getFunctionID(std::string name) {
		auto t = functionIDMap->find(name);
		if (t == functionIDMap->end()) return -1;
		return t->second;
	}

	GMLVar* GMLRetDummy = new GMLVar();
	void callGMLFunctionRet(int functionID, int argCount, GMLVar** args, GMLVar* out) {
		GMLLegacyCall(NULL, NULL, *out, argCount, functionID, args);
	}
	GMLVar* callGMLFunction(int functionID, int argCount, GMLVar** args, bool noReturn) {
		if (GMLLegacyCall == NULL) return NULL;
		GMLVar* out = noReturn ? GMLRetDummy : new GMLVar();
		GMLLegacyCall(NULL, NULL, *out, argCount, functionID, args);
		return noReturn ? NULL : out;
	}

	/*GMLScriptPtr getScriptPtr(int id) {
		if (GetScriptAddress != nullptr) {
			return GetScriptAddress(id);
		} else {
			return nullptr;
		}
	}*/
}
