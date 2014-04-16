#pragma once

#include "Slice.hpp"

#include <string>
#include <list>

#include <SFML/Graphics.hpp>

#include "index/LazySpatialIndex.hpp"

class ConsoleSlice;
class AbstractGameObject;

/**
 * A "map" that supports a hero walking around.
 */
class WalkableMapSlice : public Slice {
public:
	WalkableMapSlice();
	virtual ~WalkableMapSlice() {}

	void load(const std::string& file);

	void save(const std::string& file);

	virtual YieldAction activated(GameEngineControl& geControl, Slice* prevSlice, sf::RenderWindow& window);

	//virtual YieldAction processEvent(const sf::Event& event, const sf::Time& elapsed);

	virtual void update(const sf::Time& elapsed, const std::vector<sf::Event::KeyEvent>& typed);

	virtual void render();

	//Temporary, for testing Lua.
	void changeBgColor(long elapsedMs);

private:
	//The console for this Slice.
	ConsoleSlice* console;

	YieldAction processKeyPress(const sf::Event::KeyEvent& key, const sf::Time& elapsed);

	//Properties.
	sf::Color bkgrdColor;
	std::map<std::string, sf::Texture*> tiles;
	std::vector<sf::Sprite> tmap;
	std::string onupdate; //Lua script

	GameEngineControl* geControl;
	sf::RenderWindow* window;
};

