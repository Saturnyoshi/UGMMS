#include "stdafx.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <windows.h>
#include <psapi.h>

namespace MemTools {
	// Credit for the code this is based on goes to JLBorges on the cplusplus.com forum.
	// Taken from this thread: http://www.cplusplus.com/forum/general/202725/
	const void* scan_memory(void* address_low, std::size_t nbytes, const std::vector<BYTE>& bytes_to_find) {
		// all readable pages: adjust this as required
		const DWORD pmask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE |
			PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

		::MEMORY_BASIC_INFORMATION mbi{};

		BYTE* address = static_cast<BYTE*>(address_low);
		BYTE* address_high = address + nbytes;

		while (address < address_high && ::VirtualQuery(address, std::addressof(mbi), sizeof(mbi))) {
			if ((mbi.State == MEM_COMMIT) && (mbi.Protect&pmask) && !(mbi.Protect&PAGE_GUARD)) {
				const BYTE* begin = static_cast<const BYTE*>(mbi.BaseAddress);
				const BYTE* end = begin + mbi.RegionSize;

				const BYTE* found = std::search(begin, end, bytes_to_find.begin(), bytes_to_find.end(), [](char fromText, char fromPattern) {
					return fromPattern == '?' || fromPattern == fromText;
				});
				if (found != end) {
					return found;
				}
			}

			address += mbi.RegionSize;
			mbi = {};
		}
		return nullptr;
	}

	BYTE* scan(const std::vector<BYTE>& bytes_to_find) {
		auto base = ::GetModuleHandle(NULL);
		MODULEINFO minfo{};
		::GetModuleInformation(GetCurrentProcess(), base, std::addressof(minfo), sizeof(minfo));
		return (BYTE*)scan_memory(base, minfo.SizeOfImage, bytes_to_find);
	}

	BYTE* scanLocal(const std::vector<BYTE>& bytes_to_find, BYTE* begin, int count) {
		// Scan count bytes from begin for the data in bytes_to_find
		BYTE* end = begin + count;
		const BYTE* found = std::search(begin, end, bytes_to_find.begin(), bytes_to_find.end(), [](char fromText, char fromPattern) {
			return fromPattern == '?' || fromPattern == fromText;
		});
		if (found != end) {
			return (BYTE*)found;
		}
		return nullptr;
	}

	BYTE* readPointer(BYTE* start) {
		// Returns a pointer read from the first 4 bytes of memory starting at start
		BYTE* address = 0;
		for (int i = 0; i < 4; i++) {
			address += (INT64)(*(start + i)) * (INT64)pow(16, i * 2);
		}
		return address;
	}
}
