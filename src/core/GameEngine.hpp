#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <list>

#include "widgets/FpsCounter.hpp"

//Forward declarations
class Slice;

/**
 * Class allowing Slices to control the game engine (e.g., "Set a new Slice")
 */
class GameEngineControl {
public:
	virtual ~GameEngineControl() {}

	///Replace this Slice with the new one. If "stack" is true, we put this Slice "on top".
	/// If newSlice is null, we remove the top slice.
	virtual void YieldToSlice(Slice* newSlice, bool stack) = 0;

	///Get the default monospace font.
	virtual const sf::Font& getMonoFont() const = 0;
};


/**
 * Simple GameEngine class that exposes blocks of SFML functionality.
 */
class GameEngine : public GameEngineControl {
public:
	GameEngine();
	virtual ~GameEngine() {}

	///How to position the window.
	enum class Position {Default, Center};

	//Set the current Slice
	void setSlice(Slice* slice);

	///Create and show the window
	void createGameWindow(const sf::VideoMode& wndSize, const std::string& title, Position wndPos=Position::Default);

	virtual void YieldToSlice(Slice* newSlice, bool stack);

	void runGameLoop();

	///Get the default monospace font.
	virtual const sf::Font& getMonoFont() const;

private:
	void remSlice();

	sf::RenderWindow window;
	sf::Font monoFont;
	FpsCounter fps;

	std::list<Slice*> slices; //The back-most one handles events, but all of them render.
};

