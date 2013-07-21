#pragma once

#include "Slice.hpp"

#include <string>
#include <list>

#include <SFML/Graphics.hpp>

class ConsoleSlice;

/**
 * A "menu" slice with various objects arranged in 2-D space and a "character" who walks around and
 *   can choose a single item with "Enter".
 * Debug commands allow you to "lift" each menu item and move it into position.
 */
class EuclideanMenuSlice : public Slice {
public:
	EuclideanMenuSlice();

	void load(const std::string& file);

	void save(const std::string& file);

	virtual ~EuclideanMenuSlice() {}

	virtual YieldAction activated(GameEngineControl& geControl, Slice* prevSlice, sf::RenderWindow& window);

	virtual YieldAction processEvent(const sf::Event& event, const sf::Time& elapsed);

	virtual void render();

private:
	void resizeViews();

	//React to the results from a returned Console (possibly re-establishing it if there's an error).
	YieldAction handleConsoleResults();

	//The console for this Slice.
	ConsoleSlice* console;

	YieldAction processKeyPress(const sf::Event::KeyEvent& key);

	std::list<sf::Drawable*> items; //Temp

	GameEngineControl* geControl;
	sf::RenderWindow* window;
	sf::View mainView;
	sf::View minimapView;
};

