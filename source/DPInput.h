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
	cugl::Vec2 _rightHandCur;
	cugl::Vec2 _rightHandPrev;
	cugl::Vec2 _leftHandCur = { -1.0f, -1.0f };
	cugl::Vec2 _leftHandPrev = { -1.0f, -1.0f };

protected:
	// INPUT RESULTS
	/** Whether the reset action was chosen. */
	bool _resetPressed;
	/** Whether the debug toggle was chosen. */
	bool _debugPressed;
	/** Whether the exit action was chosen. */
	bool _exitPressed;
    bool _pausePressed;
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
			cugl::Vec2 HandPos;
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
    
    bool didPause() const { return _pausePressed; }

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
	 * @param focus	Whether the listener currently has focus
	 */
	void touchEndedCB(const cugl::TouchEvent& event, bool focus);

	/**
	 * Callback for a mouse release event.
	 *
	 * @param event The associated event
	 * @param previous The previous position of the touch
	 * @param focus	Whether the listener currently has focus
	 */
	void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);

	void update();

	unsigned int getNumTouches();

	cugl::Vec2 getTouchPosition(cugl::TouchEvent touchEvent);

	/**
	 This function matches the real game character's two hands positions with the two hands positions of the inner _character model.
	 Hence, it should be called right after the entire class is initialized.
	 */
	void fillHand(cugl::Vec2 leftHandPos, cugl::Vec2 rightHandPos, cugl::Vec2 realLHPos, cugl::Vec2 realRHPos);

	cugl::Vec2 getLeftHandMovement();

	cugl::Vec2 getrightHandMovement();
};

#endif /* __PF_INPUT_H__ */
