/*
 * main.cpp
 */

#include <stdexcept>
#include <iostream>

#include <SFML/Graphics.hpp>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}


#include "widgets/FpsCounter.hpp"
#include "core/GameEngine.hpp"

//Called from Lua
int get_num_args(lua_State *L)
{
	//Get the number of arguments
	int n = lua_gettop(L); //Should be zero in this case.

	//Push our result.
	lua_pushnumber(L, n);

	//Only 1 result this time.
	return 1;
}

int call_func(lua_State* L, int x, int y)
{
	//func_name
	lua_getglobal(L, "generous_add");

	//args
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);

	//Call it.
	lua_call(L, 2, 1);

	//Retrieve the result.
	int sum = (int)lua_tointeger(L, -1);
	lua_pop(L, 1);

	return sum;
}


int main( int argc, const char* argv[] )
{
	//Lua state.
	lua_State* L;

	//Initialize Lua
	L = luaL_newstate();

	//Load base libraries.
	luaL_openlibs(L);

	//Register our function!
	lua_register(L, "get_num_args", get_num_args);

	//Load the script.
	if (luaL_dofile(L, "script/hello.lua") != 0) {
		std::cout <<"Error: " <<lua_tostring(L,-1) <<"\n";
		return 1;
	}

	//Call the function.
	int res = call_func(L, 3,7);
	std::cout <<"TEST: " <<res <<"\n";

	//A GameEngine encapsulates our sfml calls.
	GameEngine engine;
	engine.createGameWindow({800, 600}, "Portentia", GameEngine::Position::Center);

    //Game loop; update, etc.
    engine.runGameLoop();

	return 0;
}




