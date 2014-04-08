#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <list>

extern "C" {
	#include "lua.h"
}

#include "widgets/FpsCounter.hpp"

//Forward declarations
class Slice;
struct YieldAction;


/**
 * Class allowing Slices to control the game engine (e.g., "Set a new Slice")
 */
class GameEngineControl {
public:
	virtual ~GameEngineControl() {}

	///Replace this Slice with the new one. If "stack" is true, we put this Slice "on top".
	/// If newSlice is null, we remove the top slice.
	//virtual void YieldToSlice(Slice* newSlice, bool stack) = 0;

	///Get the default monospace font.
	virtual const sf::Font& getMonoFont() const = 0;
};


/**
 * Simple GameEngine class that exposes blocks of SFML functionality.
 */
class GameEngine : public GameEngineControl {
public:
	GameEngine();
	virtual ~GameEngine();

	///How to position the window.
	enum class Position {Default, Center};

	//Set the current Slice; replace all others in the stack (call once, at the game's start).
	void setSlice(Slice* slice);

	///Create and show the window
	void createGameWindow(const sf::VideoMode& wndSize, const std::string& title, Position wndPos=Position::Default);

	//virtual void YieldToSlice(Slice* newSlice, Slice* parent, bool stack);

	void runGameLoop();

	///Get the default monospace font.
	virtual const sf::Font& getMonoFont() const;

	///Get the current Lua state.
	lua_State* lua();

private:
	//Portions of the game update loop
	void processEvents(const sf::Time& elapsed);
	void repaintGame() const;
	YieldAction addRemMoveSlices(const YieldAction& next, Slice* currSlice);

	bool addSlice(Slice* slice); //Add a Slice to the stack.
	bool remSlice(); //Remove. Does *not* free the associated memory.

	mutable sf::RenderWindow window;

	sf::Font monoFont;
	FpsCounter fps;

	//Every engine maintains the current Lua state.
	lua_State* L;

	std::list<Slice*> slices; //The back-most one handles events, but all of them render.
};

