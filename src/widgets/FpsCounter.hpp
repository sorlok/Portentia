#pragma once

#include <SFML/Graphics.hpp>
#include <list>


/**
 * A Text field that averages the frames-per-second over a given number of past measurements.
 * Performs well enough if performance doesn't fluctuate wildly.
 */
class FpsCounter : public sf::Text {
public:
	FpsCounter(int numMeasurements);

	void update(const sf::Time& elapsed);

private:
	double delay;
	int maxMeasurements;
	std::list<double> measurements;
	double runningTotal;
};

