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
#ifndef __RG_INPUT_H__
#define __RG_INPUT_H__
#include <cugl/cugl.h>

/**
 * This class represents player input in the Ragdollk demo.
 *
 * This input handler uses the CUGL input API.  It uses the polling API for
 * keyboard, but the callback API for touch.  This demonstrates a mix of ways
 * to handle input, and the reason for hiding it behind an abstraction like
 * this class.
 *
 * Unlike CUGL input devices, this class is not a singleton.  It must be
 * allocated before use.  However, you will notice that we do not do any
 * input initialization in the constructor.  This allows us to allocate this
 * controller as a field without using pointers. We simply add the class to the
 * header file of its owner, and delay initialization (via the method init())
 * until later. This is one of the main reasons we like to avoid initialization
 * in the constructor.
 */
class RagdollInput {
private:
    /** Whether or not this input is active */
    bool _active;
    // KEYBOARD EMULATION
    /** Whether the up arrow key is down */
    bool  _keyUp;
    /** Whether the down arrow key is down */
    bool  _keyDown;
    /** Whether the reset key is down */
    bool  _keyReset;
    /** Whether the debug key is down */
    bool  _keyDebug;
    /** Whether the exit key is down */
    bool  _keyExit;

    // TOUCH SUPPORT
    /** The initial touch location for the current gesture, IN SCREEN COORDINATES */
    cugl::Vec2 _dtouch;
    /** The timestamp for the beginning of the current gesture */
    cugl::Timestamp _timestamp;
  

	/**
	 * Handles touchBegan and mousePress events using shared logic.
	 *
	 * Depending on the platform, the appropriate callback (i.e. touch or mouse) 
     * will call into this method to handle the Event.
	 *
	 * @param timestamp	 the timestamp of the event
	 * @param pos		 the position of the touch
	 */
    void touchBegan(const cugl::Timestamp timestamp, const cugl::Vec2& pos);


	/**
	 * Handles touchEnded and mouseReleased events using shared logic.
	 *
	 * Depending on the platform, the appropriate callback (i.e. touch or mouse) 
     * will call into this method to handle the Event.
	 *
	 * @param timestamp	 the timestamp of the event
	 * @param pos		 the position of the touch
	 */
    void touchEnded(const cugl::Timestamp timestamp, const cugl::Vec2& pos);

	/**
	 * Handles touchMoved and mouseDragged events using shared logic.
	 *
	 * Depending on the platform, the appropriate callback (i.e. touch or mouse) 
     * will call into this method to handle the Event.
	 *
	 * @param timestamp	 the timestamp of the event
	 * @param pos		 the position of the touch
	 */
    void touchMoved(const cugl::Vec2& pos);
  

protected:
    // INPUT RESULTS
    /** Whether the reset action was chosen. */
    bool _resetPressed;
    /** Whether the debug toggle was chosen. */
    bool _debugPressed;
    /** Whether the exit action was chosen. */
    bool _exitPressed;
	/** Are we registering an object selection? */
	bool _select;
    /** The id of the current selection touch */
    long long _touchID;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    RagdollInput(); // Don't initialize.  Allow stack based
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~RagdollInput() { dispose(); }
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    bool init();
    
#pragma mark -
#pragma mark Input Detection
    /**
     * Returns true if the input handler is currently active
     *
     * @return true if the input handler is currently active
     */
    bool isActive( ) const { return _active; }

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
    void  update(float dt);

    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();
    
#pragma mark -
#pragma mark Input Results
	/**
	 * Returns the amount of vertical movement.
	 *
	 * -1 = down, 1 = up, 0 = still
	 *
	 * @return the amount of vertical movement.
	 */
	bool didSelect() const { return _select; }

	/**
	 * Returns the location (in world space) of the selection.
     *
     * The origin of the coordinate space is the top left corner of the
     * screen.  This will need to be transformed to world coordinates
     * (via the scene graph) to be useful
     *
	 * @return the location (in world space) of the selection.
	 */
	const cugl::Vec2& getSelection() const { return _dtouch; }

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
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);

	/**
	 * Callback for a mouse press event.
	 *
	 * @param t     The touch information
	 * @param event The associated event
	 */
    void mousePressBeganCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);
  
    /**
     * Callback for the end of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);
  
	/**
	 * Callback for a mouse release event.
	 *
	 * @param t     The touch information
	 * @param event The associated event
	 */
    void mouseReleasedCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);

	/**
	 * Callback for a mouse release event.
	 *
	 * @param t     The touch information
	 * @param event The associated event
	 */
	void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);
  
	/**
	 * Callback for a mouse drag event.
	 *
	 * @param t     The touch information
	 * @param event The associated event
	 */
	void mouseDraggedCB(const cugl::MouseEvent& event, const cugl::Vec2& previous, bool focus);


};

#endif /* __RG_INPUT_H__ */
