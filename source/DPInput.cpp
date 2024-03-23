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
	_character.leftHand.HandPos = cugl::Vec2(0, 0);
	_character.rightHand.HandPos = cugl::Vec2(0, 0);
	_currDown = false;
	_prevDown = false;

#ifndef CU_TOUCH_SCREEN
	success = Input::activate<Keyboard>();
#else
	/*Touchscreen* touch = Input::get<Touchscreen>();
	touch->addBeginListener(LISTENER_KEY, [=](const TouchEvent& event, bool focus) {
		this->touchBeganCB(event, focus);
		});
	touch->addEndListener(LISTENER_KEY, [=](const TouchEvent& event, bool focus) {
		this->touchEndedCB(event, focus);
		});
	touch->addMotionListener(LISTENER_KEY, [=](const TouchEvent& event, const Vec2& previous, bool focus) {
		this->touchesMovedCB(event, previous, focus);
		});*/

#endif
	_active = success;
	return success;
}

void PlatformInput::update(float dt) {
	//CULog("World coord in GF at: %f %f \n", touchPos.x, touchPos.y);
	

	/*if (!_currDown) CULog("curr is not DOWN!");
	else CULog("curr is DOWN!");
	if (!_prevDown) CULog("prev is not DOWN!");
	else CULog("prev is DOWN!");*/

	//if (_character.leftHand.curr == _leftHandCur && _character.leftHand.prev == _leftHandPrev) {
	//	_character.leftHand.prev = _character.leftHand.curr;
	//}
	//if (_character.rightHand.curr == _rightHandCur && _character.rightHand.prev == _rightHandPrev) {
	//	_character.rightHand.prev = _character.rightHand.curr;
	//}
//	CULog("Loop begins");
    _prevDown = _currDown;
    _currDown = _touchDown;
	Touchscreen* touch = Input::get<Touchscreen>();
	for (auto touchID : touch->touchSet()) {
//		CULog("Press");
		bool exist = false;
		for (auto info : _character._touchInfo) {
			if (info.id == touchID) exist = true;
		}
		if (!exist) {
			TouchInfo touchInfo;
			touchInfo.position = touch->touchPosition(touchID);
			touchInfo.worldPos = cugl::Vec2(0, 0);
			touchInfo.id = touchID;
			touchInfo.type = 0;
			_character._touchInfo.push_back(touchInfo);
		}
	}

	int count = 0;
//	CULog("Here!!!");

	for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end();) {
		if (touch->touchReleased(i->id)) {
//			CULog("Release");
			if (i->type == 1) _character.leftHand.assigned = false;
			else if (i->type == 2) _character.rightHand.assigned = false;
			i = _character._touchInfo.erase(i);
		}
		else i++;
	}

	for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end(); i++) {
//		CULog("Touchid %d", i->id);
		Vec2 position = touch2Screen(touch->touchPosition(i->id));
		i->position = touch->touchPosition(i->id);
//		CULog("count: %d", count);
//		CULog("size of touchInfo %d", _character._touchInfo.size());
//		CULog("Current Postion %f, %f", position.x, position.y);
//		CULog("Event type %d", i->type);
		count++;
	}
}
void PlatformInput::process() {
//	CULog("Loop begins");
//	Touchscreen* touch = Input::get<Touchscreen>();
//	for (auto touchID : touch->touchSet()) {
//		CULog("Press");
//		bool exist = false;
//		for (auto info : _character._touchInfo) {
//			if (info.id == touchID) exist = true;
//		}
//		if (!exist) {
//			TouchInfo touchInfo;
//			touchInfo.position = touch->touchPosition(touchID);
//			touchInfo.worldPos = cugl::Vec2(0, 0);
//			touchInfo.id = touchID;
//			touchInfo.type = 0;
//			_character._touchInfo.push_back(touchInfo);
//		}
//	}
//
//	int count = 0;
//	CULog("Here!!!");
//
//	for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end(); i++) {
//		if (touch->touchReleased(i->id)) {
//			CULog("Release");
//			if (i->type == 1) _character.leftHand.assigned = false;
//			else if (i->type == 2) _character.rightHand.assigned = false;
//			_character._touchInfo.erase(i);
//			break;
//		}
//	}
    
	Touchscreen* touch = Input::get<Touchscreen>();
	for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end(); i++) {
//		CULog("Touchid %d", i->id);
		Vec2 position = touch2Screen(touch->touchPosition(i->id));
		i->position = position;
		//CULog("count: %d", count);
//		CULog("size of touchInfo %d", _character._touchInfo.size());
//		CULog("Current Postion %f, %f", position.x, position.y);
//		CULog("Event type %d", i->type);
		//count++;
	}
	for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end(); i++) {
		Vec2 position = touch2Screen(touch->touchPosition(i->id));
		/*if(position.x < 2 && position.y < 2){
			_pausePressed = true;
			CULog("PausePressed input set to true");
			return;
		}*/
		// Not assigned yet
		//_character.leftHand.assigned = false;
//		if (_character.leftHand.assigned) CULog("leftHand is assigned");
//		else CULog("leftHand is not assigned");
//		if (_character.rightHand.assigned) CULog("rightHand is assigned");
//		else CULog("rightHand is not assigned");
		if (i->type == 0) {
			if (!_character.leftHand.assigned && _character.rightHand.assigned) {
				// Assign this touch to left hand
//				CULog("Assign to leftHand");
				_character.leftHand.assigned = true;
				_character.leftHand.prev = position;
				_character.leftHand.curr = position;
				_character.leftHand.touchID = i->id;
				i->type = 1;
			}
			else if (!_character.rightHand.assigned && _character.leftHand.assigned) {
				// Assign this touch to right hand
//				CULog("Assign to rightHand");
				_character.rightHand.assigned = true;
				_character.rightHand.prev = position;
				_character.rightHand.curr = position;
				_character.rightHand.touchID = i->id;
				i->type = 2;
			}
			else if (!_character.leftHand.assigned && !_character.rightHand.assigned) {
//				CULog("LeftHand realworldPos %f, %f", _character.leftHand.HandPos.x, _character.leftHand.HandPos.y);
//				CULog("RightHand realworldPos %f, %f", _character.rightHand.HandPos.x, _character.rightHand.HandPos.y);
//				CULog("Realworld touchPos %f, %f", i->worldPos.x, i->worldPos.y);
//				CULog("Now comparing distance. Distance to Left: %f, \n Distance to Right: %f", _character.leftHand.HandPos.distance(worldtouchPos), _character.rightHand.HandPos.distance(worldtouchPos));
				if (_character.leftHand.HandPos.distance(i->worldPos) < _character.rightHand.HandPos.distance(i->worldPos)) {
//					CULog("Now, Assigning to Left");
					// Assign this touch to left hand
					_character.leftHand.assigned = true;
					_character.leftHand.prev = position;
					_character.leftHand.curr = position;
					_character.leftHand.touchID = i->id;
					i->type = 1;
				}
				else {
//					CULog("Now, Assigning to Right");
					// Assign this touch to right hand
					_character.rightHand.assigned = true;
					_character.rightHand.prev = position;
					_character.rightHand.curr = position;
					_character.rightHand.touchID = i->id;
					i->type = 2;
				}
			}
		}
		// Has been assigned to left hand
		else if (i->type == 1) {
//			CULog("Moving the leftHand");
			_character.leftHand.prev = _character.leftHand.curr;
			_character.leftHand.curr = position;
//			CULog("left hand prev pos: %f, %f", _character.leftHand.prev.x, _character.leftHand.prev.y);
//			CULog("left hand curr pos: %f, %f", _character.leftHand.curr.x, _character.leftHand.curr.y);
		}
		//Has been assigned to right hand
		else if (i->type == 2) {
//			CULog("Moving the rightHand");
			_character.rightHand.prev = _character.rightHand.curr;
			_character.rightHand.curr = position;
//			CULog("right hand curr pos: %f, %f", _character.rightHand.curr.x, _character.rightHand.curr.y);
		}
	}

	for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end();) {
		if (touch->touchReleased(i->id)) {
//			CULog("Release");
			if (i->type == 1) _character.leftHand.assigned = false;
			else if (i->type == 2) _character.rightHand.assigned = false;
			i = _character._touchInfo.erase(i);
		}
		else i++;
	}

	//// Press
	//if (didPress()) {
	//	CULog("Press!");
	//	if (!_character.leftHand.assigned && _character.rightHand.assigned) {
	//		// Assign this touch to left hand
	//		CULog("Assign to leftHand");
	//		_character.leftHand.assigned = true;
	//		_character.leftHand.prev = position;
	//		_character.leftHand.curr = position;
	//		_character.leftHand.touchID = event.touch;
	//	}
	//	else if (!_character.rightHand.assigned && _character.leftHand.assigned) {
	//		// Assign this touch to right hand
	//		CULog("Assign to rightHand");
	//		_character.rightHand.assigned = true;
	//		_character.rightHand.prev = position;
	//		_character.rightHand.curr = position;
	//		_character.rightHand.touchID = event.touch;
	//	}
	//	else if (!_character.leftHand.assigned && !_character.rightHand.assigned) {
	//		CULog("Now comparing distance. Distance to Left: %f, \n Distance to Right: %f", _character.leftHand.HandPos.distance(_character._event.position), _character.rightHand.HandPos.distance(_character._event.position));
	//		CULog("World coord in IF at: %f %f \n", _character._event.position.x, _character._event.position.y);
	//		CULog("World coord in EF at: %f %f \n", worldtouchPos.x, worldtouchPos.y);
	//		// std::cout<< "Now, I think your positions is: " << position.toString() << "and character left hand: " << _character.leftHand.hand.toString() << "and right hand: " << _character.rightHand.hand.toString() << std::endl;
	//		//if (_character.leftHand.hand.distance(position) < _character.rightHand.hand.distance(position)) {
	//		if (_character.leftHand.HandPos.distance(worldtouchPos) < _character.rightHand.HandPos.distance(worldtouchPos)) {
	//			// CULog("Now, Assigning to Left");
	//			// Assign this touch to left hand
	//			_character.leftHand.assigned = true;
	//			_character.leftHand.prev = position;
	//			_character.leftHand.curr = position;
	//			_character.leftHand.touchID = event.touch;
	//		}
	//		else {
	//			// CULog("Now, Assigning to Right");
	//			// Assign this touch to right hand
	//			_character.rightHand.assigned = true;
	//			_character.rightHand.prev = position;
	//			_character.rightHand.curr = position;
	//			_character.rightHand.touchID = event.touch;
	//		}
	//	}
	//}

	//if (isDown()) {
	//	if (_currDown) CULog("curr is DOWN!");
	//	if (_prevDown) CULog("prev is DOWN!");
	//	Vec2 position = touch2Screen(event.position);
	//	CULog("Left Hand Pos at: %f %f \n", _character.leftHand.HandPos.x, _character.leftHand.HandPos.y);
	//	CULog("Right Hand Pos at: %f %f \n", _character.rightHand.HandPos.x, _character.rightHand.HandPos.y);
	//	CULog("Touch move at: %f %f \n", event.position.x, event.position.y);
	//	CULog("Touch move at: %f %f \n", position.x, position.y);
	//	CULog("Left hand pos: %f %f \n", _character.leftHand.hand.x, _character.leftHand.hand.y);
	//	CULog("Right hand pos: %f %f \n", _character.rightHand.hand.x, _character.rightHand.hand.y);
	//	if (_character.leftHand.touchID == event.touch) {
	//		CULog("Moving the leftHand");
	//		_character.leftHand.prev = _character.leftHand.curr;
	//		_character.leftHand.curr = position;
	//		//        _leftHandCur = _character.leftHand.curr;
	//		//        _leftHandPrev = _character.leftHand.prev;
	//	}
	//	else if (_character.rightHand.touchID == event.touch) {
	//		CULog("Moving the rightHand");
	//		_character.rightHand.prev = _character.rightHand.curr;
	//		_character.rightHand.curr = position;
	//		//        _rightHandCur = _character.rightHand.curr;
	//		//        _rightHandPrev = _character.rightHand.prev;
	//	}

	//	Vec2 offset = _character.rightHand.curr - _character.rightHand.prev;
	//	//CULog("Touch offset is: %f %f \n", offset.x, offset.y);
	//}

	/*if (didRelease()) {
		CULog("Release!");
		if (_character.leftHand.touchID == event.touch) {
			_character.leftHand.assigned = false;
			_character.leftHand.prev = Vec2(-1, -1);
			_character.leftHand.curr = Vec2(-1, -1);
			_character.leftHand.touchID = -1;
		}
		if (_character.rightHand.touchID == event.touch) {
			_character.rightHand.assigned = false;
			_character.rightHand.prev = Vec2(-1, -1);
			_character.rightHand.curr = Vec2(-1, -1);
			_character.rightHand.touchID = -1;
		}
	}*/
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
	//_character._event = event;
	if (!_touchDown) {
		_touchDown = true;
		TouchInfo touchInfo;
		touchInfo.position = event.position;
		touchInfo.type = 0;
		touchInfo.id = event.touch;
		_character._touchInfo.push_back(touchInfo);
	}
	touchPos = event.position;
//	CULog("Original touch move at: %f %f \n", event.position.x, event.position.y);
	//// TRANSFORM touchevent pos to screen pos.
	//Vec2 position = touch2Screen(event.position);

	///*if(position.x < 2 && position.y < 2){
	//	_pausePressed = true;
	//	CULog("PausePressed input set to true");
	//	return;
	//}*/

	//if (!_character.leftHand.assigned && _character.rightHand.assigned) {
	//	// Assign this touch to left hand
	//	_character.leftHand.assigned = true;
	//	_character.leftHand.prev = position;
	//	_character.leftHand.curr = position;
	//	_character.leftHand.touchID = event.touch;
	//}
	//else if (!_character.rightHand.assigned && _character.leftHand.assigned) {
	//	// Assign this touch to right hand
	//	_character.rightHand.assigned = true;
	//	_character.rightHand.prev = position;
	//	_character.rightHand.curr = position;
	//	_character.rightHand.touchID = event.touch;
	//}
	//else if (!_character.leftHand.assigned && !_character.rightHand.assigned) {
	//	// CULog("Now comparing distance. Distance to Left: %f, \n Distance to Right: %f", _character.leftHand.hand.distance(position), _character.rightHand.hand.distance(position));
	//	 // std::cout<< "Now, I think your positions is: " << position.toString() << "and character left hand: " << _character.leftHand.hand.toString() << "and right hand: " << _character.rightHand.hand.toString() << std::endl;
	//	//if (_character.leftHand.hand.distance(position) < _character.rightHand.hand.distance(position)) {
	//	if (_character.leftHand.HandPos.distance(event.position) < _character.rightHand.HandPos.distance(event.position)) {
	//		// CULog("Now, Assigning to Left");
	//		// Assign this touch to left hand
	//		_character.leftHand.assigned = true;
	//		_character.leftHand.prev = position;
	//		_character.leftHand.curr = position;
	//		_character.leftHand.touchID = event.touch;
	//	}
	//	else {
	//		// CULog("Now, Assigning to Right");
	//		// Assign this touch to right hand
	//		_character.rightHand.assigned = true;
	//		_character.rightHand.prev = position;
	//		_character.rightHand.curr = position;
	//		_character.rightHand.touchID = event.touch;
	//	}
	//}
}

/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchEndedCB(const TouchEvent& event, bool focus) {
	_pausePressed = false;

	if (_touchDown) {
		_touchDown = false;
	}
	//_character._event = event;
	touchPos = event.position;
	for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end(); i++) {
		if (i->id == event.touch) {
			if (i->type == 1) {
				_character.leftHand.assigned = false;
				_character.leftHand.prev = Vec2(-1, -1);
				_character.leftHand.curr = Vec2(-1, -1);
				_character.leftHand.touchID = -1;
			}
			else if (i->type == 2) {
				_character.rightHand.assigned = false;
				_character.rightHand.prev = Vec2(-1, -1);
				_character.rightHand.curr = Vec2(-1, -1);
				_character.rightHand.touchID = -1;
			}
			_character._touchInfo.erase(i);
			break;
		}
	}

	/*if (_character.leftHand.touchID == event.touch) {
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
	}*/
}

/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
	//_character._event = event;
	if (_touchDown) {
		for (auto i : _character._touchInfo) {
			if (i.id == event.touch) {
				i.position = event.position;
			}
		}
	}
	//_character._event = event;
	//touchPos = event.position;
	//Vec2 position = touch2Screen(event.position);
	//CULog("Left Hand Pos at: %f %f \n", _character.leftHand.HandPos.x, _character.leftHand.HandPos.y);
	//CULog("Right Hand Pos at: %f %f \n", _character.rightHand.HandPos.x, _character.rightHand.HandPos.y);
	//CULog("Touch move at: %f %f \n", _character._event.position.x, _character._event.position.y);
	//CULog("Touch move at: %f %f \n", position.x, position.y);
	////CULog("Touch move at: %f %f \n", worldPos.x, worldPos.y);
	//CULog("Left hand pos: %f %f \n", _character.leftHand.hand.x, _character.leftHand.hand.y);
	//CULog("Right hand pos: %f %f \n", _character.rightHand.hand.x, _character.rightHand.hand.y);
	//if (_character.leftHand.touchID == event.touch) {
	//	_character.leftHand.prev = _character.leftHand.curr;
	//	_character.leftHand.curr = position;
	//	//        _leftHandCur = _character.leftHand.curr;
	//	//        _leftHandPrev = _character.leftHand.prev;
	//}
	//else if (_character.rightHand.touchID == event.touch) {
	//	_character.rightHand.prev = _character.rightHand.curr;
	//	_character.rightHand.curr = position;
	//	//        _rightHandCur = _character.rightHand.curr;
	//	//        _rightHandPrev = _character.rightHand.prev;
	//}

	//Vec2 offset = _character.rightHand.curr - _character.rightHand.prev;
	////CULog("Touch offset is: %f %f \n", offset.x, offset.y);
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
	//CULog("Right Hand Movement Log: %f, %f", movement.x, movement.y);
	return movement;
}
