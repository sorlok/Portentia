#include "EuclideanMenuSlice.hpp"

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
	items.push_back(circ);
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
	//TODO
	minimapView.setCenter(0, 0);
	minimapView.setSize(1000,1000);
	minimapView.setViewport(sf::FloatRect(0.79, 0.01, 0.2, 0.2));
}

YieldAction EuclideanMenuSlice::processEvent(const sf::Event& event, const sf::Time& elapsed)
{
	YieldAction res;
	switch (event.type) {
		case sf::Event::KeyPressed:
			res = processKeyPress(event.key);
			break;
	}

	return res;
}

YieldAction EuclideanMenuSlice::processKeyPress(const sf::Event::KeyEvent& key)
{
	//Failsafe
	if (!geControl) { throw std::runtime_error("Can't process events without a GameEngineControl"); }

	switch (key.code) {
		case sf::Keyboard::Return:
			if (NoModifiers(key)) {
				//TODO: Open a "Console"
				console->reset();
				return YieldAction(YieldAction::Stack, console);
			}
			break;
	}

	return YieldAction();
}

void EuclideanMenuSlice::render()
{
	//Nothing to draw.
	if (!window) { return; }

	//First, draw everything to the main view.
	window->setView(mainView);
	for (sf::Drawable* item : items) {
		window->draw(*item);
	}

	//Draw a background for the minimap.
	window->setView(window->getDefaultView());
	sf::RectangleShape underlay;
	underlay.setPosition(minimapView.getViewport().left*window->getSize().x, minimapView.getViewport().top*window->getSize().y);
	underlay.setSize(sf::Vector2f(minimapView.getViewport().width*window->getSize().x, minimapView.getViewport().height*window->getSize().y));
	underlay.setFillColor(sf::Color::White);
	window->draw(underlay);

	//Now, draw the minimap
	window->setView(minimapView);
	for (sf::Drawable* item : items) {
		window->draw(*item);
	}


}
