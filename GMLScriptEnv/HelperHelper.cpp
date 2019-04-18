#include "stdafx.h"
#include "HelperHelper.h"
#include "GMLInternals.h"

namespace HelperHelper {
	int countResource(int checkFunc) {
		int val = -1;
		// Call sprite_exists for every id until it returns false
		for (int i = 0; true; i++) {
			GMLVar nextID = GMLVar(i);
			GMLVar* args[] = { &nextID };
			GMLVar* exists = GMLInternals::callGMLFunction(checkFunc, 1, args, false);
			if (!exists->truthy()) {
				val = i;
				delete exists;
				break;
			}
			delete exists;
		}
		return val;
	}
}
