#include "LuaBindings.hpp"

#include "core/GameEngine.hpp"
#include "slices/WalkableMapSlice.hpp"

extern "C" {
	#include <lualib.h>
}

#include <luabind/luabind.hpp>

using namespace luabind;

//From main.cpp
int get_num_args(int n1, int n2, int n3, int n4);
GameEngine* get_game_engine();


lua_State* NewLuaState()
{
	//Lua state.
	lua_State* L = luaL_newstate();

	//Bind normal Lua API calls.
	luaL_openlibs(L);

	//Connect luabind to our lua state.
	luabind::open(L);

	//Register our game module.
	module(L) [
		namespace_("game") [
			//Register our functions.
			def("get_num_args", get_num_args),
			def("engine", get_game_engine),

			//Register our classes.
			class_<GameEngine>("GameEngine")
				.def("elapsed", &GameEngine::getElapsedMs),

			class_<WalkableMapSlice>("WalkableMapSlice")
				.def("modify", &WalkableMapSlice::changeBgColor)
		]
	];


	return L;
}
