#pragma once

//Contains various geometry-related classes. We may consolidate these later.
namespace geom {


///Helper class: Represent a Point
class Point {
public:
	Point(double x, double y) : x(x), y(y) {}
	double x;
	double y;
};

///Helper class: Represents a rectangle
class Rectangle {
public:
	Rectangle(double x, double y, double width, double height) : x(x), y(y), width(width), height(height) {}

	Point getCenter() const;

	//We can have negative widths/heights
	Point getMin() const;
	Point getMax() const;

	//For zero/negative width OR height.
	bool isEmpty() const;

	//Does this intersect another rectangle?
	bool intersects(const geom::Rectangle& other) const;
	bool intersects(double x, double y, double width, double height) const;

	double x;
	double y;
	double width;
	double height;
};


}
