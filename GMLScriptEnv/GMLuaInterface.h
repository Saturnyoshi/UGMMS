#pragma once

#define API_EXPORT extern "C" __declspec(dllexport)

void init_gml_lua_interface();

// Get the ID of a function by name (eg. "instance_destroy") to call it with callFunction
API_EXPORT int gml_getFunctionID(const char* name);

// Passes values pushed with push*Arg functions as arguments
// Returns a number corresponding to the return value type, use get*Result functions to get the actual value
// 0 -> numeric value
// 1 -> string value
// 2 -> undefined
// -1 -> unhandled value (eg. arrays)
API_EXPORT int gml_callFunction(int id);

// Push arguments for calls to callFunction
// Args are pushed left to right
API_EXPORT void gml_pushStringArg(const char* value);
API_EXPORT void gml_pushRealArg(double value);
API_EXPORT void gml_pushUndefinedArg();

// Get the return value of the last callFunction
API_EXPORT double gml_getRealResult();
API_EXPORT const char* gml_getStringResult();

// Hooks the gamemaker global step event
// Called 3 times per frame with a different event_subtype for begin step, step, end step
API_EXPORT bool gml_hook_globalStep(void hook_callback(int event_subtype));
