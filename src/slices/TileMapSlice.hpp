#pragma once

#include "Slice.hpp"

#include <string>
#include <list>

#include <SFML/Graphics.hpp>

#include "index/LazySpatialIndex.hpp"

class ConsoleSlice;
class AbstractGameObject;

/**
 * A "map" that supports tiles and other objects in a variety of formats.
 */
class TileMapSlice : public Slice {
public:
	TileMapSlice();
	virtual ~TileMapSlice() {}

	void load(const std::string& file);

	void save(const std::string& file);

	virtual YieldAction activated(GameEngineControl& geControl, Slice* prevSlice, sf::RenderWindow& window);

	virtual YieldAction processEvent(const sf::Event& event, const sf::Time& elapsed);

	virtual void update(const sf::Time& elapsed);

	virtual void render();

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

