#include "FpsCounter.hpp"

#include <sstream>
#include <iomanip>
#include <iostream>

FpsCounter::FpsCounter(int numMeasurements) : maxMeasurements(numMeasurements), runningTotal(0.0), delay(0)
{
	setString("N/A fps");
}

void FpsCounter::update(const sf::Time& elapsed)
{
	//First frame tick.
	if (elapsed.asSeconds()==0) { return; }

	//Account for this
	double currMeasure = 1.0/elapsed.asSeconds();
	runningTotal += currMeasure/maxMeasurements;
	measurements.push_back(currMeasure);

	//Remove any outstanding values.
	if (measurements.size()>maxMeasurements) {
		currMeasure = measurements.front();
		measurements.pop_front();
		runningTotal -= currMeasure/maxMeasurements;
	}

	//Account for the delay.
	bool update = false;
	delay -= elapsed.asSeconds();
	if (delay<=0) {
		delay += 0.75; //Every 3/4 second, update
		update = true;
	}

	//Update text?
	if (update && measurements.size()==maxMeasurements) {
	    std::stringstream msg;
	    msg <<std::fixed <<std::setprecision(1); //1 point after the decimal.
	    msg <<runningTotal <<" fps";
		setString(msg.str());
	}
}



