//
//  GLInputController.cpp
//  Geometry Lab
//
//  This class buffers in input from the devices and converts it into its
//  semantic meaning. If your game had an option that allows the player to
//  remap the control keys, you would store this information in this class.
//  That way, the main game scene does not have to keep track of the current
//  key mapping.
//
//  Author: Walker M. White
//  Version: 1/29/24
//
#include <cugl/cugl.h>
#include "GLInputController.h"

using namespace cugl;

#pragma mark Input Control
/**
 * Creates a new input controller.
 *
 * This constructor DOES NOT attach any listeners, as we are not
 * ready to do so until the scene is created. You should call
 * the {@link #init} method to initialize the scene.
 */
InputController::InputController() :
_active(false),
_currDown(false),
_prevDown(false),
_mouseDown(false),
_mouseKey(0) {
}


/**
 * Initializes the control to support mouse or touch.
 *
 * This method attaches all of the listeners. It tests which
 * platform we are on (mobile or desktop) to pick the right
 * listeners.
 *
 * This method will fail (return false) if the listeners cannot
 * be registered or if there is a second attempt to initialize
 * this controller
 *
 * @return true if the initialization was successful
 */
bool InputController::init() {
#ifdef CU_TOUCH_SCREEN
    Touchscreen* touch = Input::get<Touchscreen>();
    // ADD CODE HERE FOR EXTRA CREDIT
    if (touch) {
        Uint32 touchID = touch->acquireKey();
        touch->addBeginListener(touchID,[=](const cugl::TouchEvent& event, bool focus) {
            this->fingerDownCB(event,touch->touchSet());
        });
        touch->addEndListener(touchID,[=](const cugl::TouchEvent& event, bool focus) {
            this->fingerUpCB(event);
        });
        touch->addMotionListener(touchID,[=](const cugl::TouchEvent& event, const Vec2 previous, bool focus) {
            this->fingerMotionCB(event);
        });
        _active = true;
    }
#else
    Mouse* mouse = Input::get<Mouse>();
    if (mouse) {
        mouse->setPointerAwareness(Mouse::PointerAwareness::DRAG);
        _mouseKey = mouse->acquireKey();
        mouse->addPressListener(_mouseKey,[=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
            this->buttonDownCB(event,clicks,focus);
        });
        mouse->addReleaseListener(_mouseKey,[=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
            this->buttonUpCB(event,clicks,focus);
        });
        mouse->addDragListener(_mouseKey,[=](const cugl::MouseEvent& event, const Vec2 previous, bool focus) {
            this->motionCB(event,previous,focus);
        });
        _active = true;
    }
#endif
    return _active;
}

/**
 * Disposes this input controller, deactivating all listeners.
 *
 * As the listeners are deactived, the user will not be able to
 * monitor input until the controller is reinitialized with the
 * {@link #init} method.
 */
void InputController::dispose() {
    if (_active) {
        Mouse* mouse = Input::get<Mouse>();
        mouse->removePressListener(_mouseKey);
        mouse->removeReleaseListener(_mouseKey);
        mouse->removeDragListener(_mouseKey);
        mouse->setPointerAwareness(Mouse::PointerAwareness::BUTTON);
        _active = false;
    }
}

/**
 * Return true if the user initiated a press this frame.
 *
 * A press means that the user is pressing (button/finger) this
 * animation frame, but was not pressing during the last frame.
 *
 * @return true if the user initiated a press this frame.
 */
bool InputController::didPress(){
    for (const auto & [ key, value ] : _currDown) {
        if (!_prevDown[key] && _currDown[key]){
            return true;
        }
    }
    return false;
}

/**
 * Return true if the user initiated a release this frame.
 *
 * A release means that the user was pressing (button/finger) last
 * animation frame, but is not pressing during this frame.
 *
 * @return true if the user initiated a release this frame.
 */
bool InputController::didRelease() {
    for (const auto & [ key, value ] : _currDown) {
        if (!_currDown[key] && _prevDown[key]){
            return true;
        }
    }
    return false;
}

/**
 * Return true if the user is actively pressing this frame.
 *
 * This method only checks that a press is active or ongoing.
 * It does not care when the press was initiated.
 *
 * @return true if the user is actively pressing this frame.
 */
bool InputController::isDown() {
    for (const auto & [ key, value ] : _currDown) {
        if (_currDown[key]){
            return true;
        }
    }
    return false;
}
/**
 * Updates the input controller for the latest frame.
 *
 * It might seem weird to have this method given that everything
 * is processed with call back functions.  But we need some way
 * to synchronize the input with the animation frame.  Otherwise,
 * how can we know what was the touch location *last frame*?
 * Maybe there has been no callback function executed since the
 * last frame. This method guarantees that everything is properly
 * synchronized.
 */
void InputController::update() {
    _prevDown = _currDown;
    _prevPos = _currPos;

#ifdef CU_TOUCH_SCREEN
    _currDown = _touchDown;
    _currPos = _touchPos;

# else
    _currDown = _mouseDown;
    _currPos = _mousePos;

#endif
    
}

#pragma mark -
#pragma mark Touch Callbacks
/**
 * Call back to execute when a finger is down.
 *
 * This function will record a press only for one finger.
 *
 * @param event     The event with the touch information
 * @param touchIDs     The vector of fingers on the screen
 */
void InputController::fingerDownCB(const cugl::TouchEvent& event, const std::vector<Sint64> touchIDs) {
    std::string touchID = std::to_string(event.touch);
    if (_touchPos.size() < _lengthPos && _touchPos.count(touchID)==0) {
        _touchPos[touchID] = event.position;
    }
}
    
    /**
     * Call back to execute when the finger is released.
     *
     * This function will record a release for the finger.
     *
     * @param event     The event with the mouse information
     */
    void InputController::fingerUpCB(const cugl::TouchEvent& event) {
        std::string touchID = std::to_string(event.touch);
        if (_touchPos.count(touchID)>0) {
            _touchPos.erase(touchID);
        }
    }
    
    /**
     * Call back to execute when a finger moves.
     *
     *
     * @param event     The event with the touch information
     */
    void InputController::fingerMotionCB(const cugl::TouchEvent& event) {
        std::string touchID = std::to_string(event.touch);
        if (_touchPos.count(touchID)>0) {
            _touchPos[touchID] = event.position;
        }
    }
    
    
    
#pragma mark Mouse Callbacks
    /**
     * Call back to execute when a mouse button is first pressed.
     *
     * This function will record a press only if the left button is pressed.
     *
     * @param event     The event with the mouse information
     * @param clicks    The number of clicks (for double clicking)
     * @param focus     Whether this device has focus (UNUSED)
     */
    void InputController::buttonDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
        // Only recognize the left mouse button
        if (_mousePos.count("0")==0 && event.buttons.hasLeft()) {
            _mousePos["0"] = event.position;
        }
    }
    
    /**
     * Call back to execute when a mouse button is first released.
     *
     * This function will record a release for the left mouse button.
     *
     * @param event     The event with the mouse information
     * @param clicks    The number of clicks (for double clicking)
     * @param focus     Whether this device has focus (UNUSED)
     */
    void InputController::buttonUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
        // Only recognize the left mouse button
        if (_mousePos.count("0")>=0 && event.buttons.hasLeft()) {
            _mousePos.erase("0");
        }
    }
    
    /**
     * Call back to execute when the mouse moves.
     *
     * This input controller sets the pointer awareness only to monitor a mouse
     * when it is dragged (moved with button down), not when it is moved. This
     * cuts down on spurious inputs. In addition, this method only pays attention
     * to drags initiated with the left mouse button.
     *
     * @param event     The event with the mouse information
     * @param previous  The previously reported mouse location
     * @param focus     Whether this device has focus (UNUSED)
     */
    void InputController::motionCB(const cugl::MouseEvent& event, const Vec2 previous, bool focus) {
        if (_mousePos.count("0")>=0) {
            _mousePos["0"] = event.position;
        }
    }
    

