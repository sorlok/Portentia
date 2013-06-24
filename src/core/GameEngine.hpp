#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "widgets/FpsCounter.hpp"
#include "slices/Slice.hpp"

/**
 * Simple GameEngine class that exposes blocks of SFML functionality.
 */
class GameEngine {
public:
	GameEngine();

	///How to position the window.
	enum class Position {Default, Center};

	//Set the current Slice
	void setSlice(Slice* slice);

	///Create and show the window
	void createGameWindow(const sf::VideoMode& wndSize, const std::string& title, Position wndPos=Position::Default);

	void runGameLoop();

	///Get the default monospace font.
	const sf::Font& getMonoFont() const;

private:
	sf::RenderWindow window;
	sf::Font monoFont;
	FpsCounter fps;
	Slice* currSlice;
};

