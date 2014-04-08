#pragma once

extern "C" {
	#include "lua.h"
}

///Register all game-related Lua functions.
lua_State* NewLuaState();
