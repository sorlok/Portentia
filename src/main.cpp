/*
 * main.cpp
 */

#include <stdexcept>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "widgets/FpsCounter.hpp"
#include "core/GameEngine.hpp"


int main( int argc, const char* argv[] )
{
	//A GameEngine encapsulates our sfml calls.
	GameEngine engine;
	engine.createGameWindow({800, 600}, "Portentia", GameEngine::Position::Center);

    //Game loop; update, etc.
    engine.runGameLoop();

	return 0;
}




