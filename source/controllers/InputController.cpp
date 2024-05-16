#include "InputController.h"
using namespace cugl;

/** The key for the event handlers */
#define LISTENER_KEY 1

#pragma mark -
#pragma mark Input Controller

InputController::InputController() {}

void InputController::dispose() {
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Input::deactivate<Keyboard>();
#else
        Touchscreen *touch = Input::get<Touchscreen>();
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
#endif
        _active = false;
    }
}

bool InputController::init(const Rect bounds) {
    bool success = true;
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();
    _character.leftHand.assigned = false;
    _character.rightHand.assigned = false;
    _character.leftHand.HandPos = cugl::Vec2(0, 0);
    _character.rightHand.HandPos = cugl::Vec2(0, 0);
    _currDown = false;
    _prevDown = false;
    _started = false;
    _initialCd = 0;

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

Touchscreen * savedtouch = nullptr;

void InputController::update(float dt) {
    // CULog("World coord in GF at: %f %f \n", touchPos.x, touchPos.y);
//#ifdef CU_TOUCH_SCREEN
    Touchscreen *touch = Input::get<Touchscreen>();
    savedtouch = touch;
    // Press to continue
    if (!_started) {
        if (_initialCd <= 10) {
            _initialCd++;
            return;
        }
        if (touch->touchCount() == 0) {
           // CULog("Press to continue!");
        }

        else {
          //  CULog("Now is active");
            _started = true;
        }
        return;
    }
    _prevDown = _currDown;
    _currDown = touch->touchCount()>0;
    for (auto touchID : touch->touchSet()) {
       // CULog("Press");
        bool exist = false;
        for (auto &info : _character._touchInfo) {
            if (info.id == touchID)
                exist = true;
        }
        if (!exist) {
            TouchInfo touchInfo;
            if (touch->touchDown(touchID)) {
                touchInfo.position = touch->touchPosition(touchID);
                touchInfo.worldPos = cugl::Vec2(0, 0);
                touchInfo.id = touchID;
                touchInfo.type = 0;
                _character._touchInfo.push_back(touchInfo);
            }
            
        }
    }

    // CULog("Here!!!");

    for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end();) {
        if (touch->touchReleased(i->id)) {
            // CULog("Release");
            if (i->type == 1)
                _character.leftHand.assigned = false;
            else if (i->type == 2)
                _character.rightHand.assigned = false;
            i = _character._touchInfo.erase(i);
        } else
            i++;
    }

    for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end(); i++) {
        // CULog("Touchid %d", i->id);
//        Vec2 position = touch2Screen(touch->touchPosition(i->id));
        if (touch->touchDown(i->id))
            i->position = touch->touchPosition(i->id);
        // CULog("size of touchInfo %d", _character._touchInfo.size());
        // CULog("Current Postion %f, %f", position.x, position.y);
        // CULog("Event type %d", i->type);
    }
//#endif
}
void InputController::process() {
    Touchscreen *touch = savedtouch;
    for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end(); i++) {
        // CULog("Touchid %d", i->id);
        if (touch->touchDown(i->id)) {
            i->position = touch2Screen(touch->touchPosition(i->id));
        }

        // CULog("count: %d", count);
        // CULog("size of touchInfo %d", _character._touchInfo.size());
        // CULog("Current Postion %f, %f", position.x, position.y);
        // CULog("Event type %d", i->type);
        // count++;
    }
    for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end(); i++) {
        if (touch->touchDown(i->id)) {
            Vec2 position = touch2Screen(touch->touchPosition(i->id));
            /*if(position.x < 2 && position.y < 2){
                _pausePressed = true;
                CULog("PausePressed input set to true");
                return;
            }*/
            // Not assigned yet
            //_character.leftHand.assigned = false;
            // if (_character.leftHand.assigned) CULog("leftHand is assigned");
            // else CULog("leftHand is not assigned");
            // if (_character.rightHand.assigned) CULog("rightHand is assigned");
            // else CULog("rightHand is not assigned");
            if (i->type == 0) {
                if (!_character.leftHand.assigned && _character.rightHand.assigned) {
                    // Assign this touch to left hand
                    // CULog("Assign to leftHand");
                    _character.leftHand.assigned = true;
                    _character.leftHand.prev = position;
                    _character.leftHand.curr = position;
                    _character.leftHand.touchID = i->id;
                    i->type = 1;
                } else if (!_character.rightHand.assigned && _character.leftHand.assigned) {
                    // Assign this touch to right hand
                    // CULog("Assign to rightHand");
                    _character.rightHand.assigned = true;
                    _character.rightHand.prev = position;
                    _character.rightHand.curr = position;
                    _character.rightHand.touchID = i->id;
                    i->type = 2;
                } else if (!_character.leftHand.assigned && !_character.rightHand.assigned) {
                    // CULog("LeftHand realworldPos %f, %f", _character.leftHand.HandPos.x, _character.leftHand.HandPos.y);
                    // CULog("RightHand realworldPos %f, %f", _character.rightHand.HandPos.x, _character.rightHand.HandPos.y);
                    // CULog("Realworld touchPos %f, %f", i->worldPos.x, i->worldPos.y);
                    // CULog("Now comparing distance. Distance to Left: %f, \n Distance to Right: %f", _character.leftHand.HandPos.distance(worldtouchPos),
                    // _character.rightHand.HandPos.distance(worldtouchPos));
                    if (_character.leftHand.HandPos.distance(i->worldPos) < _character.rightHand.HandPos.distance(i->worldPos)) {
                        //    CULog("Now, Assigning to Left");
                        // Assign this touch to left hand
                        _character.leftHand.assigned = true;
                        _character.leftHand.prev = position;
                        _character.leftHand.curr = position;
                        _character.leftHand.touchID = i->id;
                        i->type = 1;
                    } else {
                        // CULog("Now, Assigning to Right");
                        //  Assign this touch to right hand
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
                // CULog("Moving the leftHand");
                _character.leftHand.prev = _character.leftHand.curr;
                _character.leftHand.curr = position;
                // CULog("left hand prev pos: %f, %f", _character.leftHand.prev.x, _character.leftHand.prev.y);
                // CULog("left hand curr pos: %f, %f", _character.leftHand.curr.x, _character.leftHand.curr.y);
            }
            // Has been assigned to right hand
            else if (i->type == 2) {
                // CULog("Moving the rightHand");
                _character.rightHand.prev = _character.rightHand.curr;
                _character.rightHand.curr = position;
                // CULog("right hand curr pos: %f, %f", _character.rightHand.curr.x, _character.rightHand.curr.y);
            }
        }
    }

    for (auto i = _character._touchInfo.begin(); i != _character._touchInfo.end();) {
        if (touch->touchReleased(i->id)) {
            // CULog("Release");
            if (i->type == 1)
                _character.leftHand.assigned = false;
            else if (i->type == 2)
                _character.rightHand.assigned = false;
            i = _character._touchInfo.erase(i);
        } else
            i++;
    }
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void InputController::clear() {
    _resetPressed = false;
    _debugPressed = false;
    _exitPressed = false;
}

/**
 * Populates the initial values of the input TouchInstance
 */
void InputController::clearTouchInstance(TouchInstance &touchInstance) {
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
Vec2 InputController::touch2Screen(const Vec2 pos) const {
    float px = pos.x / _tbounds.size.width - _tbounds.origin.x;
    float py = pos.y / _tbounds.size.height - _tbounds.origin.y;
    Vec2 result;
    result.x = px * _sbounds.size.width + _sbounds.origin.x;
    result.y = (1 - py) * _sbounds.size.height + _sbounds.origin.y;
    return result;
}

PublishedMessage InputController::getMessageInPreUpdate() {
    auto msg = PublishedMessage();
    if (didPress()){
        std::cout << "SCREEN PRESSED MSG PUSHED TO QUEUE" << std::endl;
        msg.Head = "Screen Pressed";
    }
    else if (didRelease()){
        msg.Head = "Screen Released";
    }
    return msg;
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
 * Then, we compare the positions of these hands with player's finger, and assign the nearest hand to player's finger. When the player puts the other finger on, the leftover hand (be it right or left)
 * is assigned. That is to say: **leftHand and rightHand should match with character's two hands, but are not matched with player's two fingers.*
 *
 * ***Similar things happen when the player removes fingers. The corresponding hand gets unassigned.
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchBeganCB(const TouchEvent &event, bool focus) {
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
    CULog("Original touch move at: %f %f \n", event.position.x, event.position.y);
}

/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchEndedCB(const TouchEvent &event, bool focus) {
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
            } else if (i->type == 2) {
                _character.rightHand.assigned = false;
                _character.rightHand.prev = Vec2(-1, -1);
                _character.rightHand.curr = Vec2(-1, -1);
                _character.rightHand.touchID = -1;
            }
            _character._touchInfo.erase(i);
            break;
        }
    }
}

/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchesMovedCB(const TouchEvent &event, const Vec2 &previous, bool focus) {
    //_character._event = event;
    if (_touchDown) {
        for (auto i : _character._touchInfo) {
            if (i.id == event.touch) {
                i.position = event.position;
            }
        }
    }
}

unsigned int InputController::getNumTouches() {
    if (!_character.leftHand.assigned && !_character.rightHand.assigned) {
        return 0;
    } else if (_character.leftHand.assigned && _character.rightHand.assigned) {
        return 2;
    } else
        return 1;
}

Vec2 InputController::getTouchPosition(TouchEvent touchEvent) { return touchEvent.position; }

void InputController::fillHand(Vec2 leftHandPos, Vec2 rightHandPos, Vec2 realLHPos, Vec2 realRHPos) {
    _character.leftHand.hand = leftHandPos;
    _character.rightHand.hand = rightHandPos;
    _character.leftHand.HandPos = realLHPos;
    _character.rightHand.HandPos = realRHPos;
}

Vec2 InputController::getLeftHandMovement() {
    _leftHandCur = _character.leftHand.curr;
    _leftHandPrev = _character.leftHand.prev;
    if (!_character.leftHand.assigned)
        return {0, 0};
    Vec2 movement = _character.leftHand.curr - _character.leftHand.prev;
    // log movement
    // CULog("Left Hand Movement Log: %f, %f", movement.x, movement.y);
    return movement;
}

Vec2 InputController::getrightHandMovement() {
    _rightHandCur = _character.rightHand.curr;
    _rightHandPrev = _character.rightHand.prev;
    if (!_character.rightHand.assigned)
        return {0, 0};
    Vec2 movement = _character.rightHand.curr - _character.rightHand.prev;
    // log movement
    // CULog("Right Hand Movement Log: %f, %f", movement.x, movement.y);
    return movement;
}

void InputController::resetInput() { 
    _character.leftHand.assigned = false; 
    _character.rightHand.assigned = false; 
    _character._touchInfo.clear();
}
