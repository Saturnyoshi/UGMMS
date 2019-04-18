#pragma once

namespace HelperHelper {
	// Used to calculate the built-in number of a resource
	// Takes a GML function id used to check whether an ID exists (like object_exists)
	int countResource(int checkFunc);
}