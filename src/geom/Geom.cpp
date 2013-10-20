#include "Geom.hpp"

#include <algorithm>

using namespace geom;

Point geom::Rectangle::getCenter() const
{
	return Point(
		x + width/2,
		y + height/2
	);
}


Point geom::Rectangle::getMin() const
{
	return Point(
		std::min(x, x+width),
		std::min(y, y+height)
	);
}

Point geom::Rectangle::getMax() const
{
	return Point(
		std::max(x, x+width),
		std::max(y, y+height)
	);
}

bool geom::Rectangle::isEmpty() const
{
	return width<=0 || height<=0;
}

bool geom::Rectangle::intersects(double x, double y, double width, double height) const
{
	return intersects(Rectangle(x, y, width, height));
}

bool geom::Rectangle::intersects(const geom::Rectangle& other) const
{
	return     this->getMin().x <= other.getMax().x
			&& other.getMin().x <= this->getMax().x
			&& this->getMin().y <= other.getMax().y
			&& other.getMin().y <= this->getMax().y;
}
