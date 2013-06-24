#include "EuclideanMenuSlice.hpp"

#include <iostream>

EuclideanMenuSlice::EuclideanMenuSlice() : Slice(), window(nullptr)
{

}

void EuclideanMenuSlice::load(const std::string& file)
{
	//TODO
}

void EuclideanMenuSlice::save(const std::string& file)
{
}

void EuclideanMenuSlice::activated(sf::RenderWindow& window)
{
	//Save
	this->window = &window;

	//Size the view appropriately.
	resizeViews();
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

void EuclideanMenuSlice::processEvents(const std::list<sf::Event>& events, const sf::Time& elapsed)
{
	for (const sf::Event& ev : events) {
		//Switch based on event type.
	}
}

void EuclideanMenuSlice::render()
{
	//Nothing to draw.
	if (!window) { return; }

	//Temp
	sf::CircleShape circ(100, 10.0);
	circ.setFillColor(sf::Color::Blue);
	circ.setPosition(100, 100);


	//First, draw everything to the main view.
	window->setView(mainView);
	window->draw(circ);

	//Draw a background for the minimap.
	window->setView(window->getDefaultView());
	sf::RectangleShape underlay;
	underlay.setPosition(minimapView.getViewport().left*window->getSize().x, minimapView.getViewport().top*window->getSize().y);
	underlay.setSize(sf::Vector2f(minimapView.getViewport().width*window->getSize().x, minimapView.getViewport().height*window->getSize().y));
	underlay.setFillColor(sf::Color::White);
	window->draw(underlay);

	//Now, draw the minimap
	window->setView(minimapView);
	circ.setFillColor(sf::Color(0x00, 0x00, 0xFF, 0x70)); //Semi-transparent.
	window->draw(circ);


}
