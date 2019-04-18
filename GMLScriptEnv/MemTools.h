
#include <string>
#include <vector>
#include <windows.h>

namespace MemTools {
	BYTE* scan(const std::vector<BYTE>& bytes_to_find);
	BYTE* readPointer(BYTE* start);
	BYTE* scanLocal(const std::vector<BYTE>& bytes_to_find, BYTE* begin, int count);
}