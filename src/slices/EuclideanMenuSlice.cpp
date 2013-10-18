#include "EuclideanMenuSlice.hpp"

#include <set>
#include <algorithm>
#include <utility>
#include <iostream>
#include <stdexcept>

#include "core/GameEngine.hpp"
#include "slices/ConsoleSlice.hpp"


EuclideanMenuSlice::EuclideanMenuSlice() : Slice(), window(nullptr), geControl(nullptr),
	console(new ConsoleSlice("Add menu items with \"additem\".", {"additem", "clear"}))
{
	//TEMP
	sf::CircleShape* circ = new sf::CircleShape(100, 10.0);
	circ->setFillColor(sf::Color::Blue);
	circ->setPosition(100, 100);
	addItem(circ, circ->getGlobalBounds());
}

void EuclideanMenuSlice::load(const std::string& file)
{
	//TODO
}

void EuclideanMenuSlice::save(const std::string& file)
{
}


YieldAction EuclideanMenuSlice::addNewMenuItem(const std::list<std::string>& params)
{
	//TEMP
	sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(150, 50));
	rect->setFillColor(sf::Color::Green);
	rect->setPosition(-600, 80);
	addItem(rect, rect->getGlobalBounds());
	resizeViews();


	//No params, so this always succeeds.
	return YieldAction();
}


YieldAction EuclideanMenuSlice::handleConsoleResults()
{
	std::list<std::string> line = console->getCurrCommand();

	//No command means the Console killed itself.
	if (line.empty() || line.front().empty()) { return YieldAction(); }

	//Else, the first item in the line is the command.
	std::string cmd = line.front();
	line.erase(line.begin());

	//Switch on this command.
	if (cmd == "clear") {
		//For now, we treat this as an error.
		console->appendCommandErrorMessage("Error: \"clear\" is not yet implemented.");
		return YieldAction(YieldAction::Stack, console);
	} else if (cmd == "additem") {
		return addNewMenuItem(line);
	}

	//Else, throw the command back to the terminal.
	std::stringstream msg;
	msg <<"Error, unexpected command: \"" <<cmd <<"\"";
	console->appendCommandErrorMessage(msg.str());
	return YieldAction(YieldAction::Stack, console);
}

YieldAction EuclideanMenuSlice::activated(GameEngineControl& geControl, Slice* prevSlice, sf::RenderWindow& window)
{
	//Save
	this->window = &window;
	this->geControl = &geControl;

	//Size the view appropriately.
	resizeViews();

	//Are we returning from a Console?
	if (prevSlice==console) {
		return handleConsoleResults();
	}

	//Done.
	return YieldAction();
}

void EuclideanMenuSlice::resizeViews()
{
	if (!window) { return; }

	//The main view is the size of the RenderWindow, always.
	sf::Vector2u sz = window->getSize();
	mainView.setCenter(0, 0);
	mainView.setSize(sz.x, sz.y);

	//The minimap view is the size of the entire 2-D map area to render, and is centered on it. It's also off-center
	std::pair<float, float> xRng(-500, 500); //min/max
	std::pair<float, float> yRng(-500, 500); //min/max

	//
	// TODO: We need a spatial index to return the bounds. We *also* need to include the size of the object,
	//       not just its position. We *also* need to subclass Drawable (since we aren't always adding Shapes).
	//

	//Better init.
	if (!isItemsEmpty()) {
		//TEMP: casting...
		xRng.first = xRng.second = ((sf::Shape*)get_first_item())->getPosition().x;
		yRng.first = yRng.second = ((sf::Shape*)get_first_item())->getPosition().y;
	}

	//Measure it.
	check_all_items();
	items_sp.forAllItems([&xRng, &yRng](sf::Drawable* item) {
//	for (sf::Drawable* item : items) {
		//TEMP: casting...
		float x = ((sf::Shape*)item)->getPosition().x;
		float y = ((sf::Shape*)item)->getPosition().y;
		xRng.first = std::min(xRng.first, x);
		xRng.second = std::max(xRng.second, x);
		yRng.first = std::min(yRng.first, y);
		yRng.second = std::max(yRng.second, y);
//	}
	});

	//Set it.
	float xDiff = xRng.second-xRng.first;
	float yDiff = yRng.second-yRng.first;

	//TEMP: Widen it
	xRng.first -= 300;
	yRng.first -= 300;
	xDiff += 600;
	yDiff += 600;

	minimapView.setCenter(xRng.first+xDiff/2.0, yRng.first+yDiff/2.0);
	minimapView.setSize(xDiff, yDiff);
	minimapView.setViewport(sf::FloatRect(0.79, 0.01, 0.2, 0.2));
}

YieldAction EuclideanMenuSlice::processEvent(const sf::Event& event, const sf::Time& elapsed)
{
	YieldAction res;
	switch (event.type) {
		case sf::Event::KeyPressed:
			res = processKeyPress(event.key);
			break;

		//Else, don't handle
		default: break;
	}

	return res;
}

YieldAction EuclideanMenuSlice::processKeyPress(const sf::Event::KeyEvent& key)
{
	//Failsafe
	if (!geControl) { throw std::runtime_error("Can't process events without a GameEngineControl"); }

	switch (key.code) {
		case sf::Keyboard::Escape:
			if (NoModifiers(key)) {
				//TODO: Open a "Console"
				console->reset();
				return YieldAction(YieldAction::Stack, console);
			}
			break;

		//Else, don't handle
		default: break;
	}

	return YieldAction();
}

void EuclideanMenuSlice::render()
{
	//Nothing to draw.
	if (!window) { return; }

	//First, draw everything to the main view.
	window->setView(mainView);
	check_all_items();
	items_sp.forAllItems([this](sf::Drawable* item) {
	//for (sf::Drawable* item : items) {
		window->draw(*item);
	});

	//Draw a background for the minimap.
	window->setView(window->getDefaultView());
	sf::RectangleShape underlay;
	underlay.setPosition(minimapView.getViewport().left*window->getSize().x, minimapView.getViewport().top*window->getSize().y);
	underlay.setSize(sf::Vector2f(minimapView.getViewport().width*window->getSize().x, minimapView.getViewport().height*window->getSize().y));
	underlay.setFillColor(sf::Color::White);
	window->draw(underlay);

	//Now, draw the minimap
	window->setView(minimapView);
	check_all_items();
	items_sp.forAllItems([this](sf::Drawable* item) {
	//for (sf::Drawable* item : items) {
		window->draw(*item);
	});
}


void EuclideanMenuSlice::addItem(sf::Drawable* item, const sf::FloatRect& bounds)
{
	//Convert to SFML
	LazySpatialIndex<sf::Drawable*>::Rectangle boundsR(bounds.left, bounds.top, bounds.width, bounds.height);

	//Add to both.
	items.push_back(item);
	items_sp.addItem(item, boundsR);
}

bool EuclideanMenuSlice::isItemsEmpty() const
{
	//Check both:
	if (items.size() != items_sp.totalItems) {
		throw std::runtime_error("isItemsEmpty() size mismatch.");
	}

	//Always return the operation on the new set.
	return items_sp.totalItems==0;
}

const sf::Drawable* EuclideanMenuSlice::get_first_item() const {
	const sf::Drawable* it1 = items.front();
	const sf::Drawable* it2 = nullptr;
	items_sp.forAllItems([it1, &it2](const sf::Drawable* item) {
		if (item==it1) { it2=it1; return; }
	});
	if (it2) {
		return it2;
	}
	throw std::runtime_error("get_first_item() not contained in items_sp.");
}

void EuclideanMenuSlice::check_all_items() const {
	std::set<const sf::Drawable*> items1;
	for (sf::Drawable* item : items) {
		items1.insert(item);
	}

	std::set<const sf::Drawable*> items2;
	items_sp.forAllItems([&items2](sf::Drawable* item) {
		items2.insert(item);
	});

	if (items1.size() != items2.size()) {
		throw std::runtime_error("Sets do not contain the same number of elements.");
	}
	if (!std::equal(items1.begin(), items1.end(), items2.begin())) {
		throw std::runtime_error("Sets are not equal!");
	}
}


