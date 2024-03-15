#include "DPInput.h"
using namespace cugl;

/** The key for the event handlers */
#define LISTENER_KEY      1

#pragma mark -
#pragma mark Input Controller

PlatformInput::PlatformInput() {
}

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
	std::cout << _sbounds.getMinX() << " sbound " << _sbounds.getMaxX() << std::endl;
	_tbounds = Application::get()->getDisplayBounds();
	std::cout << _tbounds.getMinX() << " tbound " << _tbounds.getMaxX() << std::endl;
	_character.leftHand.assigned = false;
	_character.rightHand.assigned = false;

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
	if (_character.leftHand.curr == _leftHandCur && _character.leftHand.prev == _leftHandPrev) {
		_character.leftHand.prev = _character.leftHand.curr;
	}
	if (_character.rightHand.curr == _rightHandCur && _character.rightHand.prev == _rightHandPrev) {
		_character.rightHand.prev = _character.rightHand.curr;
	}
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
    // TRANSFORM touchevent pos to screen pos.
    Vec2 position = touch2Screen(event.position);
    /*if(position.x < 2 && position.y < 2){
        _pausePressed = true;
        CULog("PausePressed input set to true");
        return;
    }*/
    
	if (!_character.leftHand.assigned && _character.rightHand.assigned) {
		// Assign this touch to left hand
		_character.leftHand.assigned = true;
		_character.leftHand.prev = position;
		_character.leftHand.curr = position;
		_character.leftHand.touchID = event.touch;
	}
	else if (!_character.rightHand.assigned && _character.leftHand.assigned) {
		// Assign this touch to right hand
		_character.rightHand.assigned = true;
		_character.rightHand.prev = position;
		_character.rightHand.curr = position;
		_character.rightHand.touchID = event.touch;
	}
	else if (!_character.leftHand.assigned && !_character.rightHand.assigned) {
		// CULog("Now comparing distance. Distance to Left: %f, \n Distance to Right: %f", _character.leftHand.hand.distance(position), _character.rightHand.hand.distance(position));
		 // std::cout<< "Now, I think your positions is: " << position.toString() << "and character left hand: " << _character.leftHand.hand.toString() << "and right hand: " << _character.rightHand.hand.toString() << std::endl;
		//if (_character.leftHand.hand.distance(position) < _character.rightHand.hand.distance(position)) {
		if (_character.leftHand.HandPos.distance(event.position) < _character.rightHand.HandPos.distance(event.position)) {
			// CULog("Now, Assigning to Left");
			// Assign this touch to left hand
			_character.leftHand.assigned = true;
			_character.leftHand.prev = position;
			_character.leftHand.curr = position;
			_character.leftHand.touchID = event.touch;
		}
		else {
			// CULog("Now, Assigning to Right");
			// Assign this touch to right hand
			_character.rightHand.assigned = true;
			_character.rightHand.prev = position;
			_character.rightHand.curr = position;
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
    _pausePressed = false;
	if (_character.leftHand.touchID == event.touch) {
		_character.leftHand.assigned = false;
		_character.leftHand.prev = Vec2(-1, -1);
		_character.leftHand.curr = Vec2(-1, -1);
		_character.leftHand.touchID = -1;
	}
	else if (_character.rightHand.touchID == event.touch) {
		_character.rightHand.assigned = false;
		_character.rightHand.prev = Vec2(-1, -1);
		_character.rightHand.curr = Vec2(-1, -1);
		_character.rightHand.touchID = -1;
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
	Vec2 position = touch2Screen(event.position);
	CULog("Touch move at: %f %f \n", event.position.x, event.position.y);
	CULog("Touch move at: %f %f \n", position.x, position.y);
	CULog("Left hand pos: %f %f \n", _character.leftHand.hand.x, _character.leftHand.hand.y);
	CULog("Right hand pos: %f %f \n", _character.rightHand.hand.x, _character.rightHand.hand.y);
	if (_character.leftHand.touchID == event.touch) {
		_character.leftHand.prev = _character.leftHand.curr;
		_character.leftHand.curr = position;
		//        _leftHandCur = _character.leftHand.curr;
		//        _leftHandPrev = _character.leftHand.prev;
	}
	else if (_character.rightHand.touchID == event.touch) {
		_character.rightHand.prev = _character.rightHand.curr;
		_character.rightHand.curr = position;
		//        _rightHandCur = _character.rightHand.curr;
		//        _rightHandPrev = _character.rightHand.prev;
	}

	Vec2 offset = _character.rightHand.curr - _character.rightHand.prev;
	CULog("Touch offset is: %f %f \n", offset.x, offset.y);
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

void PlatformInput::fillHand(Vec2 leftHandPos, Vec2 rightHandPos, Vec2 realLHPos, Vec2 realRHPos) {
	_character.leftHand.hand = leftHandPos;
	_character.rightHand.hand = rightHandPos;
	_character.leftHand.HandPos = realLHPos;
	_character.rightHand.HandPos = realRHPos;
}

Vec2 PlatformInput::getLeftHandMovement() {
	_leftHandCur = _character.leftHand.curr;
	_leftHandPrev = _character.leftHand.prev;
	if (!_character.leftHand.assigned) return { 0,0 };
	Vec2 movement = _character.leftHand.curr - _character.leftHand.prev;
	// log movement
	//CULog("Left Hand Movement Log: %f, %f", movement.x, movement.y);
	return movement;
}

Vec2 PlatformInput::getrightHandMovement() {
	_rightHandCur = _character.rightHand.curr;
	_rightHandPrev = _character.rightHand.prev;
	if (!_character.rightHand.assigned) return { 0,0 };
	Vec2 movement = _character.rightHand.curr - _character.rightHand.prev;
	// log movement
	CULog("Right Hand Movement Log: %f, %f", movement.x, movement.y);
    return movement;
}

