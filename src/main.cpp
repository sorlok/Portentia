/*
 * main.cpp
 */

#include <stdexcept>
#include <iostream>

#include <SFML/Graphics.hpp>

extern "C" {
	#include "lua.h"
}

#include <luabind/luabind.hpp>

#include "widgets/FpsCounter.hpp"
#include "core/GameEngine.hpp"

int get_num_args(int n1, int n2, int n3, int n4)
{
	return n4;
}


int main( int argc, const char* argv[] )
{
	//Lua state.
	lua_State* L;

	//Initialize Lua
	L = luaL_newstate();

	//Connect luabind to our lua state.
	luabind::open(L);

	//Register our function!
	//lua_register(L, "get_num_args", get_num_args);
	luabind::module(L, "game") [
		luabind::def("get_num_args", get_num_args)
	];

	//Load the script.
	if (luaL_dofile(L, "script/hello.lua") != 0) {
		std::cout <<"Error: " <<lua_tostring(L,-1) <<"\n";
		return 1;
	}

	//Call the function.
	//int res = call_func(L, 3,7);
	int res = luabind::call_function<int>(L, "generous_add", 3, 7);
	std::cout <<"TEST: " <<res <<"\n";

	//Close the Lua state.
	lua_close(L);


	//A GameEngine encapsulates our sfml calls.
	GameEngine engine;
	engine.createGameWindow({800, 600}, "Portentia", GameEngine::Position::Center);

    //Game loop; update, etc.
    engine.runGameLoop();

	return 0;
}




