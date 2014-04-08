/*
 * main.cpp
 */

#include <stdexcept>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "widgets/FpsCounter.hpp"
#include "core/GameEngine.hpp"

GameEngine engine;

int get_num_args(int n1, int n2, int n3, int n4)
{
	return n4;
}

GameEngine* get_game_engine()
{
	return &engine;
}


int main( int argc, const char* argv[] )
{
	//A GameEngine encapsulates our sfml calls.
	engine.createGameWindow({800, 600}, "Portentia", GameEngine::Position::Center);

    //Game loop; update, etc.
    engine.runGameLoop();

	return 0;
}




