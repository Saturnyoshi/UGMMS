#pragma once
#include "gml.h"
#include <string>

typedef GMLVar& (*GMLScriptPtr)(GMLInstance* self, GMLInstance* other, GMLVar& out, int argCount, GMLVar** args);
// Contains methods for dealing with YYC GML on a low level
namespace GMLInternals {
	std::string __InitialSetup();
	// Returns the numerical ID of a built-in function, taking its name
	// Returns -1 if the function doesn't exist
	int getFunctionID(std::string name);
    int getFunctionID(const char* name);
	// Call a built-in GM function using its numerical ID
	// Arguments are passed by array
	// Set the noReturn parameter to true if you don't need the function return value, otherwise it must be manually freed
	GMLVar* callGMLFunction(int functionID, int argCount, GMLVar** args, GMLVar* return_value);
	//GMLScriptPtr getScriptPtr(int scriptID);
}