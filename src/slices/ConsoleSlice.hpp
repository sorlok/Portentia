#pragma once

#include "Slice.hpp"

#include <sstream>
#include <string>
#include <list>

#include <SFML/Graphics.hpp>


/**
 * A "console" slice is almost always used on top of another Slice in "Debug" mode to add new
 *   elements or edit the current world. "Enter" typically confirms a command. Simple tab completion
 *   exists.
 */
class ConsoleSlice : public Slice {
public:
	ConsoleSlice(const std::string& text, const std::list<std::string>& commands);

	virtual ~ConsoleSlice() {}

	virtual void activated(GameEngineControl& geControl, sf::RenderWindow& window);

	virtual void processEvent(const sf::Event& event, const sf::Time& elapsed);

	virtual void render();

private:
	void resizeConsole();
	void refreshText();

	std::string headerText;
	std::list<std::string> commands;

	GameEngineControl* geControl;
	sf::RenderWindow* window;

	std::stringstream currLine; //Current input line, sans the $
	sf::RectangleShape bkgrd;
	sf::Text text;

	std::list<std::string> out_buffer; //Recently printed lines.
};

