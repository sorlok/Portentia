#pragma once

#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "AbstractGameObject.hpp"
#include "geom/Geom.hpp"


/**
 * Wraps a CircleShape from SFML. This is really only for Debug purposes.
 */
class CircleGameObject : public sf::CircleShape, public AbstractGameObject {
public:
	CircleGameObject(float radius=0, unsigned int pointCount=30, const std::string& name="");

	virtual void save(std::ofstream& file, int tabLevel) const;

	virtual geom::Rectangle getBounds() const;

	virtual void draw(sf::RenderWindow& window) const;

};

