#include "GameEngine.hpp"

#include <vector>
#include <iostream>
#include <stdexcept>

#include "platform/Fonts.hpp"
#include "slices/Slice.hpp"
#include "slices/EuclideanMenuSlice.hpp"


GameEngine::GameEngine() : fps(100)
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

	//Initialize our fps counter.
    fps.setColor(sf::Color::Red);
    fps.setPosition(10, 10);
    fps.setCharacterSize(20);
    fps.setFont(getMonoFont());
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
	delete oldSlice;
	return true;
}


void GameEngine::createGameWindow(const sf::VideoMode& wndSize, const std::string& title, Position wndPos)
{
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

    //TEMP
    setSlice(new EuclideanMenuSlice());
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
    	sf::Time elapsed = clock.restart();

    	//Process all events.
    	processEvents(elapsed);

    	//Paint everything
    	repaintGame();
    }
}


void GameEngine::processEvents(const sf::Time& elapsed)
{
	//Update the FPS counter.
	fps.update(elapsed);

	//Update based on events.
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			window.close();
		} else {
			YieldAction res;
			Slice* currSlice = slices.empty() ? nullptr : slices.back();

			//Ask the slice to handle this event.
			if (currSlice) {
				res = currSlice->processEvent(event, elapsed);
			}

			//The slice may have Yielded
			while (res.action != YieldAction::Nothing) {
				//We are either replacing, removing, or stacking. This can trigger additional replaces/removes stacks.
				//Therefore, forward to the next Slice.
				res = addRemMoveSlices(res, currSlice);
			}
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


const sf::Font& GameEngine::getMonoFont() const
{
	return monoFont;
}



