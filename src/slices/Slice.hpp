#pragma once

#include <list>
#include <SFML/Graphics.hpp>

//Forward-declarations
class GameEngineControl;


/**
 * A "slice" is an element of gameplay. It receives all input events for a given time tick, and
 *   handles all drawing updates. Slices can stack on top of each other, but are otherwise
 *   very loosely defined.
 * TODO: I really, really want to get these running as "plugins" and then just statically
 *   compile all plugins into the Release version.
 */
class Slice {
public:
	virtual ~Slice() {}

	///Is called when a view is activated (either it becomes active for the first time, or it
	/// is re-activated by canceling out of a sub-view). Views are guaranteed to have this
	/// function called before any events are sent its way, so it can be used to initialize resources.
	///The RenderWindow passed in here is guaranteed to be valid until another call to activated (so save it!).
	virtual void activated(GameEngineControl& geControl, sf::RenderWindow& window) = 0;

	///Process a pending event.
	virtual void processEvent(const sf::Event& event, const sf::Time& elapsed) = 0;

	///Render.
	///NOTE: Do NOT call window.display()
	virtual void render() = 0;
};
