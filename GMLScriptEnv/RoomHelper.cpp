#include "stdafx.h"
#include "GMLInternals.h"
#include "RoomHelper.h"
#include "HelperHelper.h"

namespace RoomHelper {
	int roomCount = -1;

	void __InitialSetup() {
		if (roomCount != -1) return;
		roomCount = HelperHelper::countResource(GMLInternals::getFunctionID("room_exists"));
	}

	int getRoomCount() {
		return roomCount;
	}
}
