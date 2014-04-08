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

	//Header text.
	reset();
}


void ConsoleSlice::reset()
{
	//Reset our buffer and initialize it.
	out_buffer.clear();
	currLine.str("");

	//Separate by newline
	boost::split(out_buffer, headerText, boost::is_any_of("\n"));

	//Text shared to all Consoles.
	out_buffer.push_front("This is a simple Console. Use Tab, Enter, and Esc.");
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


YieldAction ConsoleSlice::activated(GameEngineControl& geControl, Slice* prevSlice, sf::RenderWindow& window)
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

	//The last line is always the start of a new command.
	//out_buffer.push_back("$ ");
	refreshText();
	return YieldAction();
}


void ConsoleSlice::appendCurrCommand(bool clearCmd)
{
	std::stringstream newLine;
	newLine <<"$ " <<currLine.str();
	out_buffer.push_back(newLine.str());
	if (clearCmd) {
		currLine.str("");
	}
}


void ConsoleSlice::matchCommands()
{
	//Match any commands that start with currLine.
	std::string pre = currLine.str();
	std::list<std::string> found;
	for (const std::string& line : commands) {
		if (line.find(pre)==0) {
			found.push_back(line);
		}
	}

	//If there's only one command, we auto-complete. Else, we list the commands.
	if (found.size()==1) {
		currLine.str("");
		currLine <<found.front();
	} else if (!found.empty()) {
		//Put "$ command"
		appendCurrCommand(false);
		std::stringstream newLine;

		//Put the options.
		for (const auto& item : found) {
			newLine <<"   " <<item;
		}
		out_buffer.push_back(newLine.str());
		refreshText();
	}
}


std::list<std::string> ConsoleSlice::getCurrCommand()
{
	//Split by spaces, remove duplicates (multiple spaces).
	std::list<std::string> res;
	std::string line = currLine.str();
	boost::split(res, line, boost::is_any_of(" "), boost::token_compress_on);

	return res;
}


void ConsoleSlice::appendCommandErrorMessage(const std::string& line)
{
	appendCurrCommand(false);
	out_buffer.push_back(line);
}



bool ConsoleSlice::processCurrCommand()
{
	//Match built-in commands first. A command "counts" if the first word (before a space) matches.
	std::list<std::string>  cmds = getCurrCommand();


	std::string word = currLine.str();
	word = word.substr(0, word.find(' '));

	//Avoid the insanity.
	if (word.empty()) {
		appendCurrCommand();
		return true;
	}

	//Else, match it.
	for (const auto& cmd : commands) {
		if (cmd == word) {
			//TODO: Communicate this command back to the parent slice.
			return false;
		}
	}

	//We also have some console-level commands. For now, we allow user-level commands to take precedence.
	if (word=="exit") {
		currLine.str(""); //Force reset.
		return false;
	}

	//If not, inform the user that their command is invalid.
	appendCurrCommand(false);
	out_buffer.push_back("Unknown command!");

	return true;
}


YieldAction ConsoleSlice::processEvent(const sf::Event& event, const sf::Time& elapsed)
{
	switch (event.type) {
		case sf::Event::KeyPressed:
			if (event.key.code==sf::Keyboard::Return) {
				if (NoModifiers(event.key)) {
					bool keep = processCurrCommand();
					if (!keep) {
						return YieldAction(YieldAction::Remove);
					}
				}
			} else if (event.key.code==sf::Keyboard::Tab) {
				if (NoModifiers(event.key)) {
					matchCommands();
				}
			} else if (event.key.code==sf::Keyboard::Escape) {
				if (NoModifiers(event.key)) {
					return YieldAction(YieldAction::Remove);
				}
			} else if (event.key.code==sf::Keyboard::BackSpace) {
				if (NoModifiers(event.key)) {
					if (!currLine.str().empty()) {
						std::string prev = currLine.str();
						prev = prev.substr(0, prev.length()-1);
						currLine.str("");
						currLine <<prev;
					}
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

		//Else, don't handle.
		default: break;
	}

	return YieldAction();
}


void ConsoleSlice::update(const sf::Time& elapsed)
{

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
