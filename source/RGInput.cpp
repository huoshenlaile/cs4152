//
//  RGInputController.h
//  RagdollDemo
//
//  This input controller is primarily designed for keyboard control.  On mobile
//  you will notice that we use gestures to emulate keyboard commands. They even
//  use the same variables (though we need other variables for internal keyboard
//  emulation).  This simplifies our design quite a bit.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White and Anthony Perello
//  Version: 1/26/17
//
#include "RGInput.h"

using namespace cugl;

#pragma mark Input Constants

/** The key to use for reseting the game */
#define RESET_KEY KeyCode::R
/** The key for toggling the debug display */
#define DEBUG_KEY KeyCode::D
/** The key for exitting the game */
#define EXIT_KEY  KeyCode::ESCAPE

/** How fast a double click must be in milliseconds */
#define EVENT_DOUBLE_CLICK  400
/** How far we must swipe left or right for a gesture */
#define EVENT_SWIPE_LENGTH  100
/** How fast we must swipe left or right for a gesture */
#define EVENT_SWIPE_TIME   1000
/** How far we must turn the tablet for the accelerometer to register */
#define EVENT_ACCEL_THRESH  M_PI/10.0f
/** The key for the event handlers */
#define LISTENER_KEY        1

#pragma mark -
#pragma mark Input Controller
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
RagdollInput::RagdollInput() :
_active(false),
_resetPressed(false),
_debugPressed(false),
_exitPressed(false),
_keyUp(false),
_keyDown(false),
_keyReset(false),
_keyDebug(false),
_keyExit(false),
_select(false),
_touchID(-1) {
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void RagdollInput::dispose() {
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Input::deactivate<Keyboard>();
        Mouse* mouse = Input::get<Mouse>();
        mouse->removePressListener(LISTENER_KEY);
        mouse->removeReleaseListener(LISTENER_KEY);
        mouse->removeDragListener(LISTENER_KEY);
#else
        Input::deactivate<Accelerometer>();
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
#endif
        _active = false;
    }
}

/**
 * Initializes the input control for the given drawing scale.
 *
 * This method works like a proper constructor, initializing the input
 * controller and allocating memory.  However, it still does not activate
 * the listeners.  You must call start() do that.
 *
 * @return true if the controller was initialized successfully
 */
bool RagdollInput::init() {
    _timestamp.mark();
    bool success = true;
    
#ifndef CU_TOUCH_SCREEN
    success = Input::activate<Keyboard>();
    success = success && Input::activate<Mouse>();
    Mouse* mouse = Input::get<Mouse>();
    // Set pointer awareness to always so listening for drags registers
    // See addDragListener for an explanation
    mouse->setPointerAwareness(cugl::Mouse::PointerAwareness::ALWAYS);
    mouse->addPressListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
      this->mousePressBeganCB(event, clicks, focus);
    });
    mouse->addReleaseListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
      this->mouseReleasedCB(event, clicks, focus);
    });
    mouse->addDragListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, const cugl::Vec2& previous, bool focus) {
      this->mouseDraggedCB(event, previous, focus);
    });
#else
    success = Input::activate<Accelerometer>();
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
    touch->addMotionListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus) {
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
void RagdollInput::update(float dt) {
#ifndef CU_TOUCH_SCREEN
    // DESKTOP CONTROLS
    Keyboard* keys = Input::get<Keyboard>();

    // Map "keyboard" events to the current frame boundary
    _keyReset  = keys->keyPressed(RESET_KEY);
    _keyDebug  = keys->keyPressed(DEBUG_KEY);
    _keyExit   = keys->keyPressed(EXIT_KEY);
#endif

    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed  = _keyExit;

// If it does not support keyboard, we must reset "virtual" keyboard
#ifdef CU_TOUCH_SCREEN
    _keyExit = false;
    _keyReset = false;
    _keyDebug = false;
#endif
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void RagdollInput::clear() {
    _resetPressed = false;
    _debugPressed = false;
    _exitPressed  = false;
    _select = false;
    _touchID = -1;
    
    _dtouch = Vec2::ZERO;
    _timestamp.mark();
}

#pragma mark -
#pragma mark Touch and Mouse Callbacks
/**
 * Callback for the beginning of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void RagdollInput::touchBeganCB(const cugl::TouchEvent& event, bool focus) {
    // Time how long it has been since last start touch, and check for enabling debug mode on mobile.
    _keyDebug = event.timestamp.ellapsedMillis(_timestamp) <= EVENT_DOUBLE_CLICK;
    _timestamp = event.timestamp;
    // if there is currently no touch for a selection
    if (_touchID == -1) {
        _touchID = event.touch;
        touchBegan(event.timestamp, event.position);
    }
}

/**
 * Callback for a mouse press event.
 *
 * @param t     The touch information
 * @param event The associated event
 */
void RagdollInput::mousePressBeganCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    touchBegan(event.timestamp, event.position);
}

/**
 * Handles touchBegan and mousePress events using shared logic.
 *
 * Depending on the platform, the appropriate callback (i.e. touch or mouse) will call into this method to handle the Event.
 *
 * @param timestamp	 the timestamp of the event
 * @param pos		 the position of the touch
 */
void RagdollInput::touchBegan(const cugl::Timestamp timestamp, const cugl::Vec2& pos) {
    // All touches correspond to key up
    _keyUp = true;
    _timestamp = timestamp;

    // Update the touch location for later gestures
	if (!_select) {
		_dtouch = pos;
	}
	_select = true;
}

 
/**
 * Callback for the end of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void RagdollInput::touchEndedCB(const cugl::TouchEvent& event, bool focus) {
    if (event.touch == _touchID) {
        touchEnded(event.timestamp, event.position);
        _touchID = -1;
    }
}

/**
 * Callback for a mouse release event.
 *
 * @param t     The touch information
 * @param event The associated event
 */
void RagdollInput::mouseReleasedCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    touchEnded(event.timestamp, event.position);
}

/**
 * Handles touchEnded and mouseReleased events using shared logic.
 *
 * Depending on the platform, the appropriate callback (i.e. touch or mouse) will call into this method to handle the Event.
 *
 * @param timestamp	 the timestamp of the event
 * @param pos		 the position of the touch
 */
void RagdollInput::touchEnded(const cugl::Timestamp timestamp, const cugl::Vec2& pos) {
    _keyUp = false;
    _select = false;
}

/**
 * Callback for a touch moved event.
 *
 * @param t     The touch information
 * @param event The associated event
 */
void RagdollInput::touchesMovedCB(const cugl::TouchEvent& event, const Vec2& previous, bool focus) {
    if (event.touch == _touchID) {
        touchMoved(event.position);
    }
}

/**
 * Callback for a mouse drag event.
 *
 * @param t     The touch information
 * @param event The associated event
 */
void RagdollInput::mouseDraggedCB(const cugl::MouseEvent& event, const Vec2& previous, bool focus) {
    touchMoved(event.position);
}

/**
 * Handles touchMoved and mouseDragged events using shared logic.
 *
 * Depending on the platform, the appropriate callback (i.e. touch or mouse) will call into this method to handle the Event.
 *
 * @param timestamp	 the timestamp of the event
 * @param pos		 the position of the touch
 */
void RagdollInput::touchMoved(const cugl::Vec2& pos) {
    _dtouch.set(pos);
}

