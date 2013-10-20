#include "RectangleGameObject.hpp"

#include <boost/lexical_cast.hpp>

RectangleGameObject::RectangleGameObject(double width, double height, const std::string& name) :
	RectangleShape(sf::Vector2f(width, height)), AbstractGameObject(name)
{}

void RectangleGameObject::save(std::ofstream& file, int tabLevel) const
{
	const std::string tab(tabLevel, ' ');
	const std::string Q("\"");

	file <<tab <<"{\n";

	sf::Vector2f sz = this->getSize();
	file <<tab <<"  " <<Q <<"width" <<Q <<" : " <<sz.x <<",\n";
	file <<tab <<"  " <<Q <<"height" <<Q <<" : " <<sz.y <<",\n";

	sf::Color color = this->getFillColor();
	file <<tab <<"  " <<Q <<"fill-color" <<Q <<" : " <<"{";
	file <<Q <<"r" <<Q <<" : " <<static_cast<unsigned int>(color.r) <<",";
	file <<Q <<"g" <<Q <<" : " <<static_cast<unsigned int>(color.g) <<",";
	file <<Q <<"b" <<Q <<" : " <<static_cast<unsigned int>(color.b) <<",";
	file <<Q <<"a" <<Q <<" : " <<static_cast<unsigned int>(color.a);
	file <<"},\n";

	sf::Vector2f pos = this->getPosition();
	file <<tab <<"  " <<Q <<"xpos" <<Q <<" : " <<pos.x <<",\n";
	file <<tab <<"  " <<Q <<"ypos" <<Q <<" : " <<pos.y <<",\n";

	//Name is always last.
	file <<tab <<"  " <<Q <<"name" <<Q <<" : " <<Q <<this->getName() <<Q <<"\n";
	file <<tab <<"}";
}


geom::Rectangle RectangleGameObject::getBounds() const
{
	sf::FloatRect bounds = getGlobalBounds();
	return geom::Rectangle(bounds.left, bounds.top, bounds.width, bounds.height);
}


void RectangleGameObject::draw(sf::RenderWindow& window) const
{
	window.draw(*this);
}
