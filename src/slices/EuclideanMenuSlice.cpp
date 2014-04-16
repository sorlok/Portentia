#include "EuclideanMenuSlice.hpp"

#include <set>
#include <algorithm>
#include <utility>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "core/GameEngine.hpp"
#include "slices/ConsoleSlice.hpp"
#include "widgets/AbstractGameObject.hpp"
#include "widgets/CircleGameObject.hpp"
#include "widgets/RectangleGameObject.hpp"


EuclideanMenuSlice::EuclideanMenuSlice() : Slice(), window(nullptr), geControl(nullptr),
	console(new ConsoleSlice("Add menu items with \"additem\".", {"additem", "save", "clear"}))
{
	//TEMP
	CircleGameObject* circ = new CircleGameObject(100, 10.0);
	circ->setFillColor(sf::Color::Blue);
	circ->setPosition(100, 100);
	addItem(circ, circ->getBounds());
}

void EuclideanMenuSlice::load(const std::string& file)
{
	//TODO
}

void EuclideanMenuSlice::save(const std::string& file)
{
	const std::string Q = "\"";

	std::ofstream out(file);
	out <<"{\n";
	out <<"  " <<Q <<"filename" <<Q <<" : " <<Q <<file <<Q <<",\n";
	out <<"  " <<Q <<"shapes" <<Q <<" : " <<"\n";

	//Use our comma trick to print items.
	out <<"    [";
	std::string comma = "";
	items_sp.forAllItems([&out,&Q,&comma](AbstractGameObject* item) {
		out <<comma <<"\n";
		item->save(out, 6);
		comma = ",";
	});
	out <<"\n    ]\n";

	//Done
	out <<"}\n";
}


YieldAction EuclideanMenuSlice::addNewMenuItem(const std::list<std::string>& params)
{
	//TEMP
	RectangleGameObject* rect = new RectangleGameObject(150, 50);
	rect->setFillColor(sf::Color::Green);
	rect->setPosition(-600, 80);
	addItem(rect, rect->getBounds());
	resizeViews();


	//No params, so this always succeeds.
	return YieldAction();
}

YieldAction EuclideanMenuSlice::saveToFile(const std::list<std::string>& params)
{
	//Determine the name of this file. The user may also provide a name.
	if (params.empty() && currFileName.empty()) {
		console->appendCommandErrorMessage("Error: \"save\" requires a file name, no default exists for this map.");
		return YieldAction(YieldAction::Stack, console);
	}

	if (!params.empty()) {
		currFileName = params.front();
	}
	save(currFileName);

	//Done.
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
	} else if (cmd == "save") {
		return saveToFile(line);
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
	std::pair<double, double> xRng(-500, 500); //min/max
	std::pair<double, double> yRng(-500, 500); //min/max

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
	items_sp.forAllItems([&xRng, &yRng](AbstractGameObject* item) {
//	for (sf::Drawable* item : items) {
		geom::Rectangle bounds = item->getBounds();
		xRng.first = std::min(xRng.first, bounds.x);
		xRng.second = std::max(xRng.second, bounds.x+bounds.width);
		yRng.first = std::min(yRng.first, bounds.y);
		yRng.second = std::max(yRng.second, bounds.y+bounds.height);
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

void EuclideanMenuSlice::update(const sf::Time& elapsed, const std::vector<sf::Event::KeyEvent>& typed)
{
	for (const auto& key : typed) {
		processKeyPress(key);
	}
}

/*YieldAction EuclideanMenuSlice::processEvent(const sf::Event& event, const sf::Time& elapsed)
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
}*/

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
	items_sp.forAllItems([this](AbstractGameObject* item) {
	//for (sf::Drawable* item : items) {
		item->draw(*window);
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
	items_sp.forAllItems([this](AbstractGameObject* item) {
	//for (sf::Drawable* item : items) {
		item->draw(*window);
	});
}


void EuclideanMenuSlice::addItem(AbstractGameObject* item, const geom::Rectangle& bounds)
{
	//Add to both.
	items.push_back(item);
	items_sp.addItem(item, bounds);
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

const AbstractGameObject* EuclideanMenuSlice::get_first_item() const {
	const AbstractGameObject* it1 = items.front();
	const AbstractGameObject* it2 = nullptr;
	items_sp.forAllItems([it1, &it2](const AbstractGameObject* item) {
		if (item==it1) { it2=it1; return; }
	});
	if (it2) {
		return it2;
	}
	throw std::runtime_error("get_first_item() not contained in items_sp.");
}

void EuclideanMenuSlice::check_all_items() const {
	std::set<const AbstractGameObject*> items1;
	for (AbstractGameObject* item : items) {
		items1.insert(item);
	}

	std::set<const AbstractGameObject*> items2;
	items_sp.forAllItems([&items2](AbstractGameObject* item) {
		items2.insert(item);
	});

	if (items1.size() != items2.size()) {
		throw std::runtime_error("Sets do not contain the same number of elements.");
	}
	if (!std::equal(items1.begin(), items1.end(), items2.begin())) {
		throw std::runtime_error("Sets are not equal!");
	}
}


