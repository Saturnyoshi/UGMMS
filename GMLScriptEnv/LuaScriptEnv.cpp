#include "stdafx.h"
#include "LuaScriptEnv.h"
#include "DebugTools.h"
#include "luainclude.h"
#include "GMLuaInterface.h"


void ShowError(lua_State* state) {
    const char* message = lua_tostring(state, -1);
    ShowMessage(message);
    lua_pop(state, 1);
}

namespace LuaScriptEnv {
    lua_State* state;
    void startWithDefaultFile(const char* filename) {
        init_gml_lua_interface();

        state = luaL_newstate();
        luaL_openlibs(state);

        int result = luaL_dofile(state, filename);
        if (result != LUA_OK) {
            ShowError(state);
        }
    }
}
