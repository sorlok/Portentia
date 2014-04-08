#include "LuaBindings.hpp"

extern "C" {
	#include <lualib.h>
}

#include <luabind/luabind.hpp>

//From main.cpp
int get_num_args(int n1, int n2, int n3, int n4);


lua_State* NewLuaState()
{
	//Lua state.
	lua_State* L = luaL_newstate();

	//Bind normal Lua API calls.
	luaL_openlibs(L);

	//Connect luabind to our lua state.
	luabind::open(L);

	//Register our functions.
	luabind::module(L, "game") [
		luabind::def("get_num_args", get_num_args)
	];

	return L;
}
