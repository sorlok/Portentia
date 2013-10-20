#pragma once

#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>

#include "geom/Geom.hpp"



/**
 * An AbstractGameObject provides all the elements necessary for interaction:
 *    * It (may) have a name that can be used to identify it.
 *    * It can save/load itself from a file.
 *    * It has a location and size (bounds/position).
 *    * It has instructions for drawing itself.
 */
class AbstractGameObject {
public:
	AbstractGameObject(const std::string& name="") : name(name)
	{}
	virtual ~AbstractGameObject()
	{}

	virtual void save(std::ofstream& file, int tabLevel) const = 0;
	virtual geom::Rectangle getBounds() const = 0;

	virtual void draw(sf::RenderWindow& window) const = 0;

	std::string getName() const {
		return name;
	}

private:
	std::string name;
};

