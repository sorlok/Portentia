#pragma once

#include "Slice.hpp"

#include <string>
#include <list>

#include <SFML/Graphics.hpp>

#include "index/LazySpatialIndex.hpp"

class ConsoleSlice;
class AbstractGameObject;

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
	//Helper: keep our two spatial indexes in sync. "Checks" will fail if they are not true for both.
	void addItem(AbstractGameObject* item, const geom::Rectangle& bounds);
	bool isItemsEmpty() const;
	const AbstractGameObject* get_first_item() const;
	void check_all_items() const;

	void resizeViews();

	//React to the results from a returned Console (possibly re-establishing it if there's an error).
	YieldAction handleConsoleResults();

	//Add an item to the menu (at the center of the screen). Params do nothing for now.
	YieldAction addNewMenuItem(const std::list<std::string>& params);

	//Save this layout to a file.
	YieldAction saveToFile(const std::list<std::string>& params);

	//The console for this Slice.
	ConsoleSlice* console;

	YieldAction processKeyPress(const sf::Event::KeyEvent& key);

	LazySpatialIndex<AbstractGameObject*> items_sp;
	std::list<AbstractGameObject*> items; //Temp

	//The name of the file which this Slice was loaded from.
	std::string currFileName;

	GameEngineControl* geControl;
	sf::RenderWindow* window;
	sf::View mainView;
	sf::View minimapView;
};

