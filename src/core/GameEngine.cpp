#include "GameEngine.hpp"

#include <vector>
#include <iostream>
#include <stdexcept>

#include "platform/Fonts.hpp"
#include "slices/EuclideanMenuSlice.hpp"


GameEngine::GameEngine() : fps(100), currSlice(nullptr)
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
	currSlice = slice;
	currSlice->activated(window);
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

void GameEngine::runGameLoop()
{
    sf::Clock clock;
    while (window.isOpen()) {
    	//Process all events, or save for later.
        sf::Event event;
        std::list<sf::Event> pending;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else {
            	//TODO: We can pull off debug keys here.
            	pending.push_back(event);
            }
        }

        //Ask the slice to update.
        sf::Time elapsed = clock.restart();
        if (currSlice) {
        	currSlice->processEvents(pending, elapsed);
        }

        //Now ask the slice to draw.
        window.clear();
        if (currSlice) {
        	currSlice->render();
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



