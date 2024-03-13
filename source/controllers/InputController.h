//#ifndef __INPUT_CONTROLLER_H__
//#define __INPUT_CONTROLLER_H__
//
//#include <cugl/cugl.h>
//#include <stdio.h>
//
//class InputController {
//protected:
//    /** Whether the input device was successfully initialized */
//    bool _active;
//    /** The current touch/mouse position */
//    std::unordered_map<std::string, cugl::Vec2> _currPos;
//    /** The previous touch/mouse position */
//    std::unordered_map<std::string, cugl::Vec2> _prevPos;
//    /** Max num of positions*/
//    int _lengthPos = 2;
//    /** Whether there is an active button/touch press */
//    std::unordered_map<std::string, bool> _currDown;
//    /** Whether there was an active button/touch press last frame*/
//    std::unordered_map<std::string, bool> _prevDown;
//
//protected:
//    /** The key for the mouse listeners */
//    Uint32 _mouseKey;
//    /** The mouse position (for mice-based interfaces) */
//    std::unordered_map<std::string, cugl::Vec2> _mousePos;
//    /** Whether the (left) mouse button is down */
//    std::unordered_map<std::string, bool> _mouseDown;
//    
//    // ADD NEW ATTRIBUTES HERE FOR EXTRA CREDIT
//    
//    /** The finger IDs and positions  */
//    std::unordered_map<std::string, cugl::Vec2> _touchPos;
//    /** Whether a finger is down */
//    std::unordered_map<std::string, bool> _touchDown;
//    
//    
//#pragma mark Input Control
//public:
//    /**
//     * Creates a new input controller.
//     *
//     * This constructor DOES NOT attach any listeners, as we are not
//     * ready to do so until the scene is created. You should call
//     * the {@link #init} method to initialize the scene.
//     */
//    InputController();
//
//    /**
//     * Deletes this input controller, releasing all resources.
//     */
//    ~InputController() { dispose(); }
//    
//    /**
//     * Initializes the control to support mouse or touch.
//     *
//     * This method attaches all of the listeners. It tests which
//     * platform we are on (mobile or desktop) to pick the right
//     * listeners.
//     *
//     * This method will fail (return false) if the listeners cannot
//     * be registered or if there is a second attempt to initialize
//     * this controller
//     *
//     * @return true if the initialization was successful
//     */
//    bool init();
//        
//    /**
//     * Disposes this input controller, deactivating all listeners.
//     *
//     * As the listeners are deactived, the user will not be able to
//     * monitor input until the controller is reinitialized with the
//     * {@link #init} method.
//     */
//    void dispose();
//    
//    /**
//     * Updates the input controller for the latest frame.
//     *
//     * It might seem weird to have this method given that everything
//     * is processed with call back functions.  But we need some way
//     * to synchronize the input with the animation frame.  Otherwise,
//     * how can we know what was the touch location *last frame*?
//     * Maybe there has been no callback function executed since the
//     * last frame. This method guarantees that everything is properly
//     * synchronized.
//     */
//    void update();
//
//#pragma mark Attributes
//    /**
//     * Returns true if this control is active.
//     *
//     * An active control is one where all of the listeners are attached
//     * and it is actively monitoring input. An input controller is only
//     * active if {@link #init} is called, and if {@link #dispose} is not.
//     *
//     * @return true if this control is active.
//     */
//    bool isActive() const { return _active; }
//    
//    
//    /**
//     * Returns the current mouse/touch position
//     *
//     * @return the current mouse/touch position
//     */
//    const std::unordered_map<std::string, cugl::Vec2>& getPosition() const {
//        return _currPos;
//    }
//
//    /**
//     * Returns the previous mouse/touch position
//     *
//     * @return the previous mouse/touch position
//     */
//    const std::unordered_map<std::string, cugl::Vec2>& getPrevious() const {
//        return _prevPos;
//    }
//    
//    /**
//     * Return true if the user initiated a press this frame.
//     *
//     * A press means that the user is pressing (button/finger) this
//     * animation frame, but was not pressing during the last frame.
//     *
//     * @return true if the user initiated a press this frame.
//     */
//    bool didPress();
//   
//    /**
//     * Return true if the user initiated a release this frame.
//     *
//     * A release means that the user was pressing (button/finger) last
//     * animation frame, but is not pressing during this frame.
//     *
//     * @return true if the user initiated a release this frame.
//     */
//    bool didRelease();
//
//    /**
//     * Return true if the user is actively pressing this frame.
//     *
//     * This method only checks that a press is active or ongoing.
//     * It does not care when the press was initiated.
//     *
//     * @return true if the user is actively pressing this frame.
//     */
//    bool isDown();
//    
//#pragma mark Touch Callbacks
//private:
//    /**
//     * Call back to execute when a finger is down.
//     *
//     * This function will record a press only for one finger.
//     *
//     * @param event     The event with the touch information
//     * @param touchIDs     The vector of fingers on the screen
//     */
//    void fingerDownCB(const cugl::TouchEvent& event, const std::vector<Sint64> touchIDs);
//    
//    /**
//     * Call back to execute when the finger is released.
//     *
//     * This function will record a release for the finger.
//     *
//     * @param event     The event with the mouse information
//     */
//    void fingerUpCB(const cugl::TouchEvent& event);
//    
//    /**
//     * Call back to execute when a finger moves.
//     *
//     *
//     * @param event     The event with the touch information
//     */
//    void fingerMotionCB(const cugl::TouchEvent& event);
//
//        
//#pragma mark Mouse Callbacks
//private:
//    /**
//     * Call back to execute when a mouse button is first pressed.
//     *
//     * This function will record a press only if the left button is pressed.
//     *
//     * @param event     The event with the mouse information
//     * @param clicks    The number of clicks (for double clicking)
//     * @param focus     Whether this device has focus (UNUSED)
//     */
//    void buttonDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);
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
//    void buttonUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);
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
//    void motionCB(const cugl::MouseEvent& event, const cugl::Vec2 previous, bool focus);
//};
//
//#endif


#pragma once
#ifndef __PF_INPUT_H__
#define __PF_INPUT_H__
#include <cugl/cugl.h>
#include <unordered_set>

class PlatformInput {
private:
    /** Whether or not this input is active */
    bool _active;
    // KEYBOARD EMULATION
    /** Whether the jump key is down */
    bool  _keyJump;
    /** Whether the fire key is down */
    bool  _keyFire;
    /** Whether the reset key is down */
    bool  _keyReset;
    /** Whether the debug key is down */
    bool  _keyDebug;
    /** Whether the exit key is down */
    bool  _keyExit;
    /** Whether the left arrow key is down */
    bool  _keyLeft;
    /** Whether the right arrow key is down */
    bool  _keyRight;

protected:
    // INPUT RESULTS
    /** Whether the reset action was chosen. */
    bool _resetPressed;
    /** Whether the debug toggle was chosen. */
    bool _debugPressed;
    /** Whether the exit action was chosen. */
    bool _exitPressed;
    /** How much did we move horizontally? */
    float _horizontal;

protected:
    /** Information representing a single "touch" (possibly multi-finger) */
    struct TouchInstance {
        /** The anchor touch position (on start) */
        cugl::Vec2 position;
        /** The current touch time */
        cugl::Timestamp timestamp;
        /** The touch id(s) for future reference */
        std::unordered_set<Uint64> touchids;
    };

    /** The bounds of the entire game screen (in touch coordinates) */
    cugl::Rect _tbounds;
    /** The bounds of the entire game screen (in scene coordinates) */
    cugl::Rect _sbounds;

    struct Character {
        struct Hand {
            cugl::Vec2 hand, prev, curr;
            cugl::TouchID touchID;
            bool assigned;
        };
        Hand leftHand;
        Hand rightHand;
    };

    Character _character;

    /**
     * Populates the initial values of the TouchInstances
     */
    void clearTouchInstance(TouchInstance& touchInstance);

    /**
     * Returns the scene location of a touch
     *
     * Touch coordinates are inverted, with y origin in the top-left
     * corner. This method corrects for this and scales the screen
     * coordinates down on to the scene graph size.
     *
     * @return the scene location of a touch
     */
    cugl::Vec2 touch2Screen(const cugl::Vec2 pos) const;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    PlatformInput(); // Don't initialize.  Allow stack based

    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~PlatformInput() { dispose(); }

    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();

    /**
     * Initializes the input control for the given bounds
     *
     * The bounds are the bounds of the scene graph.  This is necessary because
     * the bounds of the scene graph do not match the bounds of the display.
     * This allows the input device to do the proper conversion for us.
     *
     * @param bounds    the scene graph bounds
     *
     * @return true if the controller was initialized successfully
     */
    bool init(const cugl::Rect bounds);

#pragma mark -
#pragma mark Input Detection
    /**
     * Returns true if the input handler is currently active
     *
     * @return true if the input handler is currently active
     */
    bool isActive() const { return _active; }

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
    void update(float dt);

    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();

#pragma mark -
#pragma mark Input Results
    /**
     * Returns the amount of sideways movement.
     *
     * -1 = left, 1 = right, 0 = still
     *
     * @return the amount of sideways movement.
     */
    float getHorizontal() const { return _horizontal; }

    /**
     * Returns true if the reset button was pressed.
     *
     * @return true if the reset button was pressed.
     */
    bool didReset() const { return _resetPressed; }

    /**
     * Returns true if the player wants to go toggle the debug mode.
     *
     * @return true if the player wants to go toggle the debug mode.
     */
    bool didDebug() const { return _debugPressed; }

    /**
     * Returns true if the exit button was pressed.
     *
     * @return true if the exit button was pressed.
     */
    bool didExit() const { return _exitPressed; }

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
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);

    /**
     * Callback for the end of a touch event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);

    /**
     * Callback for a mouse release event.
     *
     * @param event The associated event
     * @param previous The previous position of the touch
     * @param focus    Whether the listener currently has focus
     */
    void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);

    void update();

    unsigned int getNumTouches();

    cugl::Vec2 getTouchPosition(cugl::TouchEvent touchEvent);

    /**
     This function matches the real game character's two hands positions with the two hands positions of the inner _character model.
     Hence, it should be called right after the entire class is initialized.
     */
    void fillHand(cugl::Vec2 leftHandPos, cugl::Vec2 rightHandPos);

    cugl::Vec2 getLeftHandMovement();

    cugl::Vec2 getrightHandMovement();
};

#endif /* __PF_INPUT_H__ */
