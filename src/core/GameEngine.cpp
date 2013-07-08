#include "GameEngine.hpp"

#include <vector>
#include <iostream>
#include <stdexcept>

#include "platform/Fonts.hpp"
#include "slices/Slice.hpp"
#include "slices/EuclideanMenuSlice.hpp"


GameEngine::GameEngine() : fps(100), oldSlice(nullptr)
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
	slices.push_back(slice);
	slices.back()->activated(*this, window);
}


void GameEngine::remSlice() {
	//Remove the old one (we can't delete it yet; it's this pointer is still valid).
	oldSlice = slices.back();
	slices.pop_back();
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


void GameEngine::YieldToSlice(Slice* newSlice, bool stack)
{
	//The GameEngine isn't threaded, so we can just replace the current slice on the fly.
	if (newSlice) {
		//Adding a Slice.
		if (!stack && !slices.empty()) {
			remSlice();
		}
		setSlice(newSlice);
	} else {
		//Removing a Slice.
		if (slices.empty()) {
			throw std::runtime_error("Can't remove the current Slice; nothing underneath!");
		}
		remSlice();
	}

}


void GameEngine::runGameLoop()
{
	//NOTE: Be aware that Slices have full access to every function in GameEngineControl, esp.
	//      YieldToSlice(). So don't do anything stupid like caching the current Slice unless
	//      you keep it consistent in both places.
    sf::Clock clock;
    while (window.isOpen()) {
    	//Time elapsed
    	sf::Time elapsed = clock.restart();

    	//Process all events.
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else {
                //Ask the slice to handle this event.
                if (!slices.empty()) {
                	slices.back()->processEvent(event, elapsed);
                }

                //The slice may have Yielded
                if (oldSlice) {
                	//TODO: We need to ask the parent slice "should we save this slice?". Basically,
                	//      if anything calls the Console (and it returns a credible-but-wrong command)
                	//      then we want to immediately restore the Console with an error message from the
                	//      parent slice.
                	//TODO: This might be better done using the "Yield()" style syntax shown earlier.

                	//Finally delete it.
                	delete oldSlice;
                	oldSlice = nullptr;
                }
            }
        }

        //Now ask the slice to draw.
        window.clear();
        for (Slice* sl : slices) {
        	sl->render();
        	window.setView(window.getDefaultView());
        }

        //Update the FPS counter, external to the actual Slice.
        fps.update(elapsed);
        window.draw(fps);

        //Draw
        window.display();
    }
}

const sf::Font& GameEngine::getMonoFont() const
{
	return monoFont;
}



