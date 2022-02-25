#include "stdafx.h"
#include <string>
#include <fstream>

void ShowMessage(const char* str) {
	MessageBoxA(GetActiveWindow(), str, "MsgTitle", MB_OK);
}

void WriteMemFile(std::string filename, BYTE* data, size_t bytes) {
	FILE *out;
	errno_t err;
	const size_t size = bytes;
	err = fopen_s(&out, filename.c_str(), "wb");
	if (out != NULL) {
		size_t to_go = size;
		while (to_go > 0) {
			const size_t wrote = fwrite((void*)data, to_go, 1, out);
			if (wrote == 0)
				break;
			to_go -= wrote;
		}
		fclose(out);
	}
}