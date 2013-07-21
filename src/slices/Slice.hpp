#pragma once

#include <list>
#include <SFML/Graphics.hpp>

//Forward-declarations
class GameEngineControl;

//TODO: Something better.
namespace {
//Helper: No modifiers
bool NoModifiers(const sf::Event::KeyEvent& key) {
	return !(key.alt || key.control || key.shift || key.system);
}
} //End anon namespace


/**
 * A "slice" is an element of gameplay. It receives all input events for a given time tick, and
 *   handles all drawing updates. Slices can stack on top of each other, but are otherwise
 *   very loosely defined.
 * TODO: I really, really want to get these running as "plugins" and then just statically
 *   compile all plugins into the Release version.
 */
class Slice {
public:
	///Intent to Yield to another slice.
	struct YieldAction {
		enum Action {
			Nothing, //No change.
			Replace, //Replace the top-level slice with the returned one.
			Stack,   //Stack the returned slice on top.
			Remove,  //Remove this slice from the stack.
		};

		Action action; //What to do now that we've returned.
		Slice* slice;  //The Slice to replace this one with.
		YieldAction(Action action=Nothing, Slice* slice=nullptr) : action(action), slice(slice) {}
	};

	virtual ~Slice() {}

	///Is called when a view is activated (either it becomes active for the first time, or it
	/// is re-activated by canceling out of a sub-view). Views are gerroruaranteed to have this
	/// function called before any events are sent its way, so it can be used to initialize resources.
	///The RenderWindow passed in here is guaranteed to be valid until another call to activated (so save it!).
	///The prevSlice, if non-null, contains the Slice which was active and which gave control to this slice.
	///The return value can be used to switch out the active slice.
	virtual void activated(GameEngineControl& gEngine, Slice* prevSlice, sf::RenderWindow& window) = 0;

	///Process a pending event.
	virtual YieldAction processEvent(const sf::Event& event, const sf::Time& elapsed) = 0;

	///Render.
	///NOTE: Do NOT call window.display()
	virtual void render() = 0;
};
