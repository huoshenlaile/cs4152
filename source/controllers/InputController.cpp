//#include "InputController.h"
//
//using namespace cugl;
//
//#pragma mark Input Control
///**
// * Creates a new input controller.
// *
// * This constructor DOES NOT attach any listeners, as we are not
// * ready to do so until the scene is created. You should call
// * the {@link #init} method to initialize the scene.
// */
//InputController::InputController() :
//    _active(false),
//    _currDown(false),
//    _prevDown(false),
//    _mouseDown(false),
//    _mouseKey(0) {
//}
//
//
///**
// * Initializes the control to support mouse or touch.
// *
// * This method attaches all of the listeners. It tests which
// * platform we are on (mobile or desktop) to pick the right
// * listeners.
// *
// * This method will fail (return false) if the listeners cannot
// * be registered or if there is a second attempt to initialize
// * this controller
// *
// * @return true if the initialization was successful
// */
//bool InputController::init() {
//#ifdef CU_TOUCH_SCREEN
//    Touchscreen* touch = Input::get<Touchscreen>();
//    if (touch) {
//        Uint32 touchID = touch->acquireKey();
//        touch->addBeginListener(touchID,[=](const cugl::TouchEvent& event, bool focus) {
//            this->fingerDownCB(event,touch->touchSet());
//        });
//        touch->addEndListener(touchID,[=](const cugl::TouchEvent& event, bool focus) {
//            this->fingerUpCB(event);
//        });
//        touch->addMotionListener(touchID,[=](const cugl::TouchEvent& event, const Vec2 previous, bool focus) {
//            this->fingerMotionCB(event);
//        });
//        _active = true;
//    }
//#else
//    Mouse* mouse = Input::get<Mouse>();
//    if (mouse) {
//        mouse->setPointerAwareness(Mouse::PointerAwareness::DRAG);
//        _mouseKey = mouse->acquireKey();
//        mouse->addPressListener(_mouseKey,[=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
//            this->buttonDownCB(event,clicks,focus);
//        });
//        mouse->addReleaseListener(_mouseKey,[=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
//            this->buttonUpCB(event,clicks,focus);
//        });
//        mouse->addDragListener(_mouseKey,[=](const cugl::MouseEvent& event, const Vec2 previous, bool focus) {
//            this->motionCB(event,previous,focus);
//        });
//        _active = true;
//    }
//#endif
//    return _active;
//}
//
///**
// * Disposes this input controller, deactivating all listeners.
// *
// * As the listeners are deactived, the user will not be able to
// * monitor input until the controller is reinitialized with the
// * {@link #init} method.
// */
//void InputController::dispose() {
//    if (_active) {
//        Mouse* mouse = Input::get<Mouse>();
//        mouse->removePressListener(_mouseKey);
//        mouse->removeReleaseListener(_mouseKey);
//        mouse->removeDragListener(_mouseKey);
//        mouse->setPointerAwareness(Mouse::PointerAwareness::BUTTON);
//        _active = false;
//    }
//}
//
///**
// * Return true if the user initiated a press this frame.
// *
// * A press means that the user is pressing (button/finger) this
// * animation frame, but was not pressing during the last frame.
// *
// * @return true if the user initiated a press this frame.
// */
//bool InputController::didPress(){
//    for (const auto & [ key, value ] : _currDown) {
//        if (!_prevDown[key] && _currDown[key]){
//            return true;
//        }
//    }
//    return false;
//}
//
///**
// * Return true if the user initiated a release this frame.
// *
// * A release means that the user was pressing (button/finger) last
// * animation frame, but is not pressing during this frame.
// *
// * @return true if the user initiated a release this frame.
// */
//bool InputController::didRelease() {
//    for (const auto & [ key, value ] : _currDown) {
//        if (!_currDown[key] && _prevDown[key]){
//            return true;
//        }
//    }
//    return false;
//}
//
///**
// * Return true if the user is actively pressing this frame.
// *
// * This method only checks that a press is active or ongoing.
// * It does not care when the press was initiated.
// *
// * @return true if the user is actively pressing this frame.
// */
//bool InputController::isDown() {
//    for (const auto & [ key, value ] : _currDown) {
//        if (_currDown[key]){
//            return true;
//        }
//    }
//    return false;
//}
///**
// * Updates the input controller for the latest frame.
// *
// * It might seem weird to have this method given that everything
// * is processed with call back functions.  But we need some way
// * to synchronize the input with the animation frame.  Otherwise,
// * how can we know what was the touch location *last frame*?
// * Maybe there has been no callback function executed since the
// * last frame. This method guarantees that everything is properly
// * synchronized.
// */
//void InputController::update() {
//    _prevDown = _currDown;
//    _prevPos = _currPos;
//
//#ifdef CU_TOUCH_SCREEN
//    _currDown = _touchDown;
//    _currPos = _touchPos;
//
//# else
//    _currDown = _mouseDown;
//    _currPos = _mousePos;
//#endif
//    
//}
//
//#pragma mark -
//#pragma mark Touch Callbacks
///**
// * Call back to execute when a finger is down.
// *
// * This function will record a press only for one finger.
// *
// * @param event     The event with the touch information
// * @param touchIDs     The vector of fingers on the screen
// */
//void InputController::fingerDownCB(const cugl::TouchEvent& event, const std::vector<Sint64> touchIDs) {
//    std::string touchID = std::to_string(event.touch);
//    if (_touchPos.size() < _lengthPos && _touchPos.count(touchID)==0) {
//        _touchPos[touchID] = event.position;
//    }
//}
//    
//    /**
//     * Call back to execute when the finger is released.
//     *
//     * This function will record a release for the finger.
//     *
//     * @param event     The event with the mouse information
//     */
//    void InputController::fingerUpCB(const cugl::TouchEvent& event) {
//        std::string touchID = std::to_string(event.touch);
//        if (_touchPos.count(touchID)>0) {
//            _touchPos.erase(touchID);
//        }
//    }
//    
//    /**
//     * Call back to execute when a finger moves.
//     *
//     *
//     * @param event     The event with the touch information
//     */
//    void InputController::fingerMotionCB(const cugl::TouchEvent& event) {
//        std::string touchID = std::to_string(event.touch);
//        if (_touchPos.count(touchID)>0) {
//            _touchPos[touchID] = event.position;
//        }
//    }
//    
//    
//    
//#pragma mark Mouse Callbacks
//    /**
//     * Call back to execute when a mouse button is first pressed.
//     *
//     * This function will record a press only if the left button is pressed.
//     *
//     * @param event     The event with the mouse information
//     * @param clicks    The number of clicks (for double clicking)
//     * @param focus     Whether this device has focus (UNUSED)
//     */
//    void InputController::buttonDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
//        // Only recognize the left mouse button
//        if (_mousePos.count("0")==0 && event.buttons.hasLeft()) {
//            _mousePos["0"] = event.position;
//        }
//    }
//    
//    /**
//     * Call back to execute when a mouse button is first released.
//     *
//     * This function will record a release for the left mouse button.
//     *
//     * @param event     The event with the mouse information
//     * @param clicks    The number of clicks (for double clicking)
//     * @param focus     Whether this device has focus (UNUSED)
//     */
//    void InputController::buttonUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
//        // Only recognize the left mouse button
//        if (_mousePos.count("0")>=0 && event.buttons.hasLeft()) {
//            _mousePos.erase("0");
//        }
//    }
//    
//    /**
//     * Call back to execute when the mouse moves.
//     *
//     * This input controller sets the pointer awareness only to monitor a mouse
//     * when it is dragged (moved with button down), not when it is moved. This
//     * cuts down on spurious inputs. In addition, this method only pays attention
//     * to drags initiated with the left mouse button.
//     *
//     * @param event     The event with the mouse information
//     * @param previous  The previously reported mouse location
//     * @param focus     Whether this device has focus (UNUSED)
//     */
//    void InputController::motionCB(const cugl::MouseEvent& event, const Vec2 previous, bool focus) {
//        if (_mousePos.count("0")>=0) {
//            _mousePos["0"] = event.position;
//        }
//    }


#include "InputController.h"
using namespace cugl;

/** The key for the event handlers */
#define LISTENER_KEY      1

#pragma mark -
#pragma mark Input Controller

/**
 * Creates a new input controller.
 *
 * This constructor DOES NOT attach any listeners, as we are not
 * ready to do so until the scene is created. You should call
 * the {@link #init} method to initialize the scene.
 */
PlatformInput::PlatformInput() :
_active(false){
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
 * @param focus    Whether the listener currently has focus
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
 * @param focus    Whether the listener currently has focus
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
 * @param focus    Whether the listener currently has focus
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

