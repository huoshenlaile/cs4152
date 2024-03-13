#include "DPInput.h"
using namespace cugl;

/** The key for the event handlers */
#define LISTENER_KEY      1

#pragma mark -
#pragma mark Input Controller

void PlatformInput::dispose() {
	if (_active) {
#ifndef CU_TOUCH_SCREEN
		Input::deactivate<Keyboard>();
#else
		Touchscreen* touch = Input::get<Touchscreen>();
		touch->removeBeginListener(LISTENER_KEY);
		touch->removeEndListener(LISTENER_KEY);
#endif
		_active = false;
	}
}

bool PlatformInput::init(const Rect bounds) {
	bool success = true;
	_sbounds = bounds;
	_tbounds = Application::get()->getDisplayBounds();

#ifndef CU_TOUCH_SCREEN
	success = Input::activate<Keyboard>();
#else
	Touchscreen* touch = Input::get<Touchscreen>();
	touch->addBeginListener(LISTENER_KEY, [=](const TouchEvent& event, bool focus) {
		this->touchBeganCB(event, focus);
		});
	touch->addEndListener(LISTENER_KEY, [=](const TouchEvent& event, bool focus) {
		this->touchEndedCB(event, focus);
		});
	touch->addMotionListener(LISTENER_KEY, [=](const TouchEvent& event, const Vec2& previous, bool focus) {
		this->touchesMovedCB(event, previous, focus);
		});

#endif
	_active = success;
	return success;
}

/**
 * Processes the currently cached inputs.
 *
 * This method is used to to poll the current input state.  This will poll the
 * keyboad and accelerometer.
 *
 * This method also gathers the delta difference in the touches. Depending on
 * the OS, we may see multiple updates of the same touch in a single animation
 * frame, so we need to accumulate all of the data together.
 */
void PlatformInput::update(float dt) {
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void PlatformInput::clear() {
	_resetPressed = false;
	_debugPressed = false;
	_exitPressed = false;
}

/**
 * Populates the initial values of the input TouchInstance
 */
void PlatformInput::clearTouchInstance(TouchInstance& touchInstance) {
	touchInstance.touchids.clear();
	touchInstance.position = Vec2::ZERO;
}

/**
 * Returns the scene location of a touch
 *
 * Touch coordinates are inverted, with y origin in the top-left
 * corner. This method corrects for this and scales the screen
 * coordinates down on to the scene graph size.
 *
 * @return the scene location of a touch
 */
Vec2 PlatformInput::touch2Screen(const Vec2 pos) const {
	float px = pos.x / _tbounds.size.width - _tbounds.origin.x;
	float py = pos.y / _tbounds.size.height - _tbounds.origin.y;
	Vec2 result;
	result.x = px * _sbounds.size.width + _sbounds.origin.x;
	result.y = (1 - py) * _sbounds.size.height + _sbounds.origin.y;
	return result;
}

#pragma mark -
#pragma mark Touch and Mouse Callbacks

/**
 * Callback for the beginning of a touch event
 * **FROM GEORGE: The NAMING of this class is UNCLEAR. Here's the logic:
 *
 * _character is an inner abstract struct, used to denote the positions of two hands.
 * 
 * the logic is as such:
 * Since the _character is an inner struct, at first we should assign the POSITIONS OF CHARACTER's two hands to _character using "FillHands()" function.
 *
 * When the player first touches a finger onto the screen, both 'leftHand' and 'rightHand' OF _character are unassigned.
 *
 * Then, we compare the positions of these hands with player's finger, and assign the nearest hand to player's finger. When the player puts the other finger on, the leftover hand (be it right or left) is assigned. That is to say: **leftHand and rightHand should match with character's two hands, but are not matched with player's two fingers.*
 *
 * ***Similar things happen when the player removes fingers. The corresponding hand gets unassigned.
 *
 * @param event The associated event
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchBeganCB(const TouchEvent& event, bool focus) {
	if (!_character.leftHand.assigned && _character.rightHand.assigned) {
		// Assign this touch to left hand
		_character.leftHand.assigned = true;
		_character.leftHand.prev = event.position;
		_character.leftHand.curr = event.position;
		_character.leftHand.touchID = event.touch;
	}
	else if (!_character.rightHand.assigned && _character.leftHand.assigned) {
		// Assign this touch to right hand
		_character.rightHand.assigned = true;
		_character.rightHand.prev = event.position;
		_character.rightHand.curr = event.position;
		_character.rightHand.touchID = event.touch;
	}
	else if (!_character.leftHand.assigned && !_character.rightHand.assigned) {
		if (_character.leftHand.hand.distance(event.position) < _character.rightHand.hand.distance(event.position)) {
			// Assign this touch to left hand
			_character.leftHand.assigned = true;
			_character.leftHand.prev = event.position;
			_character.leftHand.curr = event.position;
			_character.leftHand.touchID = event.touch;
		}
		else {
			// Assign this touch to right hand
			_character.rightHand.assigned = true;
			_character.rightHand.prev = event.position;
			_character.rightHand.curr = event.position;
			_character.rightHand.touchID = event.touch;
		}
	}
}

/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchEndedCB(const TouchEvent& event, bool focus) {
	if (_character.leftHand.touchID == event.touch) {
		_character.leftHand.assigned = false;
		_character.leftHand.prev = Vec2(-1, -1);
		_character.leftHand.curr = Vec2(-1, -1);
		_character.leftHand.touchID = 0;
	}
	else if (_character.rightHand.touchID == event.touch) {
		_character.rightHand.assigned = false;
		_character.rightHand.prev = Vec2(-1, -1);
		_character.rightHand.curr = Vec2(-1, -1);
		_character.rightHand.touchID = 0;
	}
}

/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
	if (_character.leftHand.touchID == event.touch) {
		_character.leftHand.prev = _character.leftHand.curr;
		_character.leftHand.curr = event.position;
	}
	else if (_character.rightHand.touchID == event.touch) {
		_character.rightHand.prev = _character.rightHand.curr;
		_character.rightHand.curr = event.position;
	}
}

unsigned int PlatformInput::getNumTouches() {
	if (!_character.leftHand.assigned && !_character.rightHand.assigned) {
		return 0;
	}
	else if (_character.leftHand.assigned && _character.rightHand.assigned) {
		return 2;
	}
	else return 1;
}

Vec2 PlatformInput::getTouchPosition(TouchEvent touchEvent) {
	return touchEvent.position;
}

void PlatformInput::fillHand(Vec2 leftHandPos, Vec2 rightHandPos) {
	_character.leftHand.hand = leftHandPos;
	_character.rightHand.hand = rightHandPos;
}

Vec2 PlatformInput::getLeftHandMovement() {
	if (!_character.leftHand.assigned) return { -1,-1 };
	else return _character.leftHand.curr - _character.leftHand.prev;
}

Vec2 PlatformInput::getrightHandMovement() {
	if (!_character.rightHand.assigned) return { -1,-1 };
	else {
		return _character.rightHand.curr - _character.rightHand.prev;
	}
}
