#include "stdafx.h"
#include "LuaScriptEnv.h"
/*#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "DebugTools.h"

void ShowError(lua_State* state) {
    const char* message = lua_tostring(state, -1);
    ShowMessage(message);
    lua_pop(state, 1);
}*/

namespace LuaScriptEnv {
//    lua_State* state;
    void startWithDefaultFile(const char* filename) {
/*
        state = luaL_newstate();
        luaL_openlibs(state);
        int result = luaL_dofile(state, filename);
        if (result != LUA_OK) {
            ShowError(state);
        }
*/
    }
}
