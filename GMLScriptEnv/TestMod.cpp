#include "stdafx.h"
#include "GMLInternals.h"
#include "DebugTools.h"
#include "SpriteHelper.h"

void TestMod_start() {

}

extern "C" __declspec(dllexport) double catch_error_init_raw() {
	// Replace catch_error init function

	return 0;
}