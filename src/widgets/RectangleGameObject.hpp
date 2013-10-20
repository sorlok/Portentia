#pragma once

#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "AbstractGameObject.hpp"
#include "geom/Geom.hpp"


/**
 * Wraps a RectangleShape from SFML. This is really only for Debug purposes.
 */
class RectangleGameObject : public sf::RectangleShape, public AbstractGameObject {
public:
	RectangleGameObject(double width=0, double height=0, const std::string& name="");

	virtual void save(std::ofstream& file, int tabLevel) const;

	virtual geom::Rectangle getBounds() const;

	virtual void draw(sf::RenderWindow& window) const;

};

