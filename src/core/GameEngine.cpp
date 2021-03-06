#include "GameEngine.hpp"

#include <vector>
#include <iostream>
#include <stdexcept>

#include <luabind/luabind.hpp>

#include "core/LuaBindings.hpp"
#include "platform/Fonts.hpp"
#include "slices/Slice.hpp"
#include "slices/WalkableMapSlice.hpp"


//Temp: We don't perform memory management of slices.
namespace {
WalkableMapSlice FirstSlice;
} //End un-named namespace.


GameEngine::GameEngine() : fps(100), L(nullptr)
{
}

GameEngine::~GameEngine()
{
	//Close the Lua state.
	if (L) {
		lua_close(L);
	}
}


void GameEngine::setSlice(Slice* slice) {
	slices.clear();
	if (addSlice(slice)) {
		slices.back()->activated(*this, nullptr, window); //TODO: Duplicate code...
	}
}


bool GameEngine::addSlice(Slice* slice) {
	if (!slice) { return false; }

	slices.push_back(slice);
	return true;
}


bool GameEngine::remSlice() {
	//Nothing?
	if (slices.empty()) { return false; }

	//Remove the old one (we can't delete it yet; it's this pointer is still valid).
	Slice* oldSlice = slices.back();
	slices.pop_back();

	//TODO: Check with the "parent" slice first. (We might use smart pointers to avoid this entirely).
	//TODO: We can't *quite* delete these, since the parent may point to a static memory address.
	//delete oldSlice;
	return true;
}


void GameEngine::createGameWindow(const sf::VideoMode& wndSize, const std::string& title, Position wndPos)
{
	//Load a default "mono" font, for helpful debugging.
	bool foundFont = false;
	std::vector<std::string> sampleFonts = {"scp.otf", platform::GuessMonoFont()};
	for (std::string font : sampleFonts) {
		if (monoFont.loadFromFile(font)) {
			foundFont = true;
			break;
		}
	}

	//Nothing?
	if (!foundFont) {
		throw std::runtime_error("Can't find a suitable font; exiting.");
    }

	//Initialize and bind the Lua state.
	L = NewLuaState();

	//Initialize our fps counter.
    fps.setColor(sf::Color::Red);
    fps.setPosition(10, 10);
    fps.setCharacterSize(20);
    fps.setFont(getMonoFont());

	//Calculate the center position first, since getDesktopMode() seems to delay window movement otherwise.
	sf::VideoMode deskMode = sf::VideoMode::getDesktopMode();
	sf::Vector2i centerPos = {
		static_cast<int>(deskMode.width-wndSize.width)/2,
		static_cast<int>(deskMode.height-wndSize.height)/2
	};

    //Create and immediately center the window, if requested. (Seems to avoid flickering this way.)
    window.create(wndSize, title);
    if (wndPos==Position::Center) {
    	window.setPosition(centerPos);
    }

    //vsync
    window.setVerticalSyncEnabled(true);

    //TEMP
    FirstSlice.load("res/map_tavern.json");
    setSlice(&FirstSlice);
}


YieldAction GameEngine::addRemMoveSlices(const YieldAction& next, Slice* currSlice)
{
	//Remove a Slice if we have to. Failure to do so returns "Nothing".
	if ((next.action==YieldAction::Replace) || (next.action==YieldAction::Remove)) {
		if (!remSlice()) { return YieldAction(); }
	}

	//Add a Slice if we have to. Failure to do so returns "Nothing".
	if ((next.action==YieldAction::Replace) || (next.action==YieldAction::Stack)) {
		if (!addSlice(next.slice)) { return YieldAction(); }
	}

	//At this point, we've successfully modified the Slice stack. Notify the top-most Slice either way.
	return slices.back()->activated(*this, currSlice, window);
}


void GameEngine::runGameLoop()
{
    sf::Clock clock;
    while (window.isOpen()) {
    	//Time elapsed
    	elapsed = clock.restart();
    	luabind::globals(L)["elapsed"] = elapsed.asMilliseconds();

    	//Process all events.
    	std::vector<sf::Event::KeyEvent> typed;
    	processEvents(typed);

    	//Update the current slice.
    	if (!slices.empty()) {
    		slices.back()->update(elapsed, typed);
    	}

    	//Paint everything
    	repaintGame();
    }
}


void GameEngine::processEvents(std::vector<sf::Event::KeyEvent>& typed)
{
	//Update the FPS counter.
	fps.update(elapsed);

	//Update based on events.
	sf::Event event;
	while (window.pollEvent(event)) {
		switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::KeyPressed:
				//Accumulate key presses.
				typed.push_back(event.key);
				break;

			default: //Else, don't handle
				break;
		}
	}
}


void GameEngine::repaintGame() const
{
	//Now ask the slice to draw.
	window.clear();
	for (Slice* sl : slices) {
		sl->render();
		window.setView(window.getDefaultView());
	}

	//Paint the FPS counter over all slices.
	window.draw(fps);

	//Draw
	window.display();
}


lua_State* GameEngine::lua()
{
	return L;
}


float GameEngine::getElapsedMs() const
{
	return elapsed.asSeconds();
}


const sf::Font& GameEngine::getMonoFont() const
{
	return monoFont;
}



