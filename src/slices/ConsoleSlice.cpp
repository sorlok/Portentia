#include "ConsoleSlice.hpp"

#include <locale>
#include <iostream>
#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include "core/GameEngine.hpp"


ConsoleSlice::ConsoleSlice(const std::string& text, const std::list<std::string>& commands) : Slice(), window(nullptr), geControl(nullptr), headerText(text), commands(commands)
{
	//Background color.
	bkgrd.setFillColor(sf::Color(0x00, 0x00, 0x66, 0x66));
	bkgrd.setOutlineColor(sf::Color::Blue);
	bkgrd.setOutlineThickness(1.0);
}


void ConsoleSlice::resizeConsole()
{
	//Sanity check
	if (!window) { return; }

	//Our console always takes up the lower half of the Window.
	sf::Vector2u size = window->getSize();
	bkgrd.setSize(sf::Vector2f(size.x-3, size.y/2-2));
	bkgrd.setPosition(1, size.y/2);

	//Our text is anchored to the upper-left of the console
	text.setPosition(1, size.y/2);
}

void ConsoleSlice::refreshText()
{
	//First, get the last X items.
	const size_t Num = 5;
	std::list<std::string> toShow;
	for (auto it=out_buffer.rbegin(); it!=out_buffer.rend(); it++) {
		toShow.push_back(*it);
		if (toShow.size()>=Num) { break; }
	}

	//We've got our last X messages.
	std::stringstream msg;
	for (auto it=toShow.rbegin(); it!=toShow.rend(); it++) {
		msg <<*it <<"\n";
	}

	//And now our current line.
	msg <<"$ " <<currLine.str();
	text.setString(msg.str());
}


void ConsoleSlice::activated(GameEngineControl& geControl, sf::RenderWindow& window)
{
	//Save
	this->window = &window;
	this->geControl = &geControl;

	//Resize our background
	resizeConsole();

	//Set our font
	text.setFont(geControl.getMonoFont());
	text.setColor(sf::Color::White);
	text.setCharacterSize(15);

	//Reset our buffer and initialize it.
	out_buffer.clear();
	currLine.str("");

	//Separate by newline
	boost::split(out_buffer, headerText, boost::is_any_of("\n"));

	//Text shared to all Consoles.
	out_buffer.push_front("This is a simple Console. Use Tab, Enter, and Esc.");

	//The last line is always the start of a new command.
	//out_buffer.push_back("$ ");
	refreshText();
}


void ConsoleSlice::processEvent(const sf::Event& event, const sf::Time& elapsed)
{
	switch (event.type) {
		case sf::Event::KeyPressed:
			if (event.key.code==sf::Keyboard::Return) {
				if (NoModifiers(event.key)) {
					std::cout <<"Return\n";
				}
			} else if (event.key.code==sf::Keyboard::Tab) {
				if (NoModifiers(event.key)) {
					std::cout <<"Tab\n";
				}
			} else if (event.key.code==sf::Keyboard::Escape) {
				if (NoModifiers(event.key)) {
					std::cout <<"Escape\n";
				}
			}
			refreshText();
			break;
		case sf::Event::TextEntered:
			if (std::isprint(event.text.unicode)) {
				currLine <<std::string(1, event.text.unicode);
			}
			refreshText();
			break;
	}
}

void ConsoleSlice::render()
{
	//Nothing to draw.
	if (!window) { return; }

	//Draw the background.
	window->draw(bkgrd);

	//Draw the text.
	window->draw(text);
}
