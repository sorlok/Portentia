#include "CircleGameObject.hpp"

#include <boost/lexical_cast.hpp>

CircleGameObject::CircleGameObject(float radius, unsigned int pointCount, const std::string& name) :
	CircleShape(radius, pointCount), AbstractGameObject(name)
{}

void CircleGameObject::save(std::ofstream& file, int tabLevel) const
{
	const std::string tab(tabLevel, ' ');
	const std::string Q("\"");

	file <<tab <<"{\n";
	file <<tab <<"  " <<Q <<"radius" <<Q <<" : " <<this->getRadius() <<",\n";
	file <<tab <<"  " <<Q <<"point-count" <<Q <<" : " <<this->getPointCount() <<",\n";

	sf::Color color = this->getFillColor();
	file <<tab <<"  " <<Q <<"fill-color" <<Q <<" : " <<"{";
	file <<Q <<"r" <<Q <<" : " <<static_cast<unsigned int>(color.r) <<",";
	file <<Q <<"g" <<Q <<" : " <<static_cast<unsigned int>(color.g) <<",";
	file <<Q <<"b" <<Q <<" : " <<static_cast<unsigned int>(color.b) <<",";
	file <<Q <<"a" <<Q <<" : " <<static_cast<unsigned int>(color.a);
	file <<"]" <<"\n";
	file <<tab <<"},\n";

	sf::Vector2f pos = this->getPosition();
	file <<tab <<"  " <<Q <<"xpos" <<Q <<" : " <<pos.x <<",\n";
	file <<tab <<"  " <<Q <<"ypos" <<Q <<" : " <<pos.y <<",\n";

	//Name is always last.
	file <<tab <<"  " <<Q <<"name" <<Q <<" : " <<this->getName() <<"\n";
}


geom::Rectangle CircleGameObject::getBounds() const
{
	sf::FloatRect bounds = getGlobalBounds();
	return geom::Rectangle(bounds.left, bounds.top, bounds.width, bounds.height);
}


void CircleGameObject::draw(sf::RenderWindow& window) const
{
	window.draw(*this);
}


