#include "stdafx.h"
#include "luainclude.h"
#include "GMLuaInterface.h"
#include "GMLInternals.h"
#include "DebugTools.h"
#include <vector>
#include "EventHooks.h"

#define MAX_ARGS 32
GMLVar* gm_return_value;    // return value of last function call
GMLVar* gm_args[MAX_ARGS];  //
int gm_args_num;            // Current arg count

GMLVar* gm_next_arg() {
    if (gm_args_num >= MAX_ARGS - 1) {
        // uh oh
        ShowMessage("Max GML call args exceeded!!!");
        return gm_return_value;
    }
    return gm_args[gm_args_num++];
}

API_EXPORT int gml_getFunctionID(const char* name) {
    return GMLInternals::getFunctionID(name);
}

API_EXPORT int gml_callFunction(int id) {
    GMLInternals::callGMLFunction(id, gm_args_num, gm_args, gm_return_value);
    gm_args_num = 0;
    switch (gm_return_value->type) {
        case GML_TYPE_BOOL:
        case GML_TYPE_INT32:
        case GML_TYPE_INT64:
        case GML_TYPE_REAL:
        case GML_TYPE_POINTER:
            // Numeric return value
            return 0;
        case GML_TYPE_STRING:
            // String value
            return 1;
        case GML_TYPE_UNDEFINED:
            // Undefined
            return 2;
        default:
            // Unhandled type
            return -1;
    }
}

API_EXPORT void gml_pushStringArg(const char* value) {
    gm_next_arg()->setString(value);
}
API_EXPORT void gml_pushRealArg(double value) {
    gm_next_arg()->setReal(value);
}
API_EXPORT void gml_pushUndefinedArg() {
    gm_next_arg()->setUndefined();
}

API_EXPORT double gml_getRealResult() {
    return gm_return_value->getReal();
}

API_EXPORT const char* gml_getStringResult() {
    return gm_return_value->getCString();
}

API_EXPORT bool gml_hook_globalStep(void hook_callback (int event_subtype)) {
    return GMLHookGlobalStep(hook_callback);
}


void init_gml_lua_interface() {
    // Initialize empty argument array
    for (int i = 0; i < MAX_ARGS; i++) {
        gm_args[i] = new GMLVar();
    }
    gm_args_num = 0;
    gm_return_value = new GMLVar();
}
