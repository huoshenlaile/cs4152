#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include <cugl/cugl.h>
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include "../controllers/InputController.h"
#include "../controllers/InteractionController.h"
#include "../controllers/CharacterController.h"
#include "../controllers/AudioController.h"
#include "../models/PlatformModel.h"
#include "../models/ButtonModel.h"
#include "../models/WallModel.h"
#include "../helpers/LevelLoader.h"
#include "../helpers/LevelConstants.h"
#include "../controllers/CameraController.h"
#include "../controllers/PauseEvent.h"
#include "../controllers/InputController.h"

// INCLUDE THIS AT LAST to avoid repeat naming (will trigger bewildering bugs)
#include "../helpers/GameSceneConstants.h"
#include "../models/PaintGrowerModel.hpp"

class GameScene : public cugl::Scene2 {
protected:
	std::shared_ptr<cugl::AssetManager> _assets;

	// CONTROLLERS
	/** Controller for abstracting out input across multiple platforms */
	//    InputController _input;
	/** Controller for handling all kinds of interactions*/
	std::shared_ptr<InteractionController> _interactionController;
	/** Network Controller*/
	std::shared_ptr<NetEventController> _network;
	/** Character Controller*/
	std::shared_ptr<CharacterController> _characterControllerA;
	//	std::shared_ptr<CharacterController> _characterControllerB;
		/** Audio Controller*/
	std::shared_ptr<AudioController> _audioController;
	std::shared_ptr<InputController> _inputController;

	std::shared_ptr<cugl::physics2::net::NetWorld> _platformWorld;
    
    std::shared_ptr<cugl::scene2::Button> _pauseButton;
    
    std::vector<std::shared_ptr<PaintModel>> _paintModels;

	// MODELS
	// TODO: Do we need a vector of these?
	PlatformModel _platform;
	WallModel _wall;
	std::shared_ptr<ButtonModel> _button;

	// VIEW
	/** Reference to the goalDoor (for collision detection) */
	std::shared_ptr<cugl::physics2::BoxObstacle> _goalDoor;
	/** Reference to the physics root of the scene graph */
	std::shared_ptr<cugl::scene2::SceneNode> _worldnode;
	/** Reference to the debug root of the scene graph */
	std::shared_ptr<cugl::scene2::SceneNode> _debugnode;
	/** Reference to the win message label */
	std::shared_ptr<cugl::scene2::Label> _winnode;
	/** Reference to the ui layer */
	std::shared_ptr<cugl::scene2::SceneNode> _uinode;
	/** Reference to the reset message label */
	std::shared_ptr<cugl::scene2::Label> _loadnode;
    /** the level complete scene */
    std::shared_ptr<cugl::scene2::SceneNode> _levelComplete;
    std::shared_ptr<cugl::scene2::Button> _levelCompleteReset;
    std::shared_ptr<cugl::scene2::Button> _levelCompleteMenuButton;

	CameraController _camera;

	std::shared_ptr<LevelLoader> _level;
	/** The scale between the physics world and the screen (MUST BE UNIFORM) */
	float _scale;

	/** Host is by default the left cannon */
	bool _isHost;

	/** Whether we have completed this "game" */
	bool _complete;
	/** Whether or not debug mode is active */
	bool _debug;

	bool _gamePaused;
	/** Relates input id to arm */
	std::unordered_map<std::string, int> _input_to_arm;
	/** Relates arm to input id*/
	std::unordered_map<int, std::string> _arm_to_input;
#pragma mark Internal Object Management

	/**
	 * Returns the active screen size of this scene.
	 *
	 * This method is for graceful handling of different aspect
	 * ratios
	 */
	cugl::Size computeActiveSize() const;

public:
    std::vector<std::shared_ptr<cugl::physics2::Joint>> Alljoints;
	/** the state of this scene, referenced by DPApp*/
    enum SceneState {
        INGAME,
        PAUSE,
        NETERROR,
        RESET,
        QUIT
    };
	SceneState state;
#pragma mark -
#pragma mark Constructors
	/**
	 * Creates a new game world with the default values.
	 *
	 * This constructor does not allocate any objects or start the controller.
	 * This allows us to use a controller without a heap pointer.
	 */
	GameScene();

	/**
	 * Disposes of all (non-static) resources allocated to this mode.
	 *
	 * This method is different from dispose() in that it ALSO shuts off any
	 * static resources, like the input controller.
	 */
	~GameScene() { dispose(); }

	/**
	 * Disposes of all (non-static) resources allocated to this mode.
	 */
	void dispose() override;

	/**
	 * Initializes the controller contents, and starts the game
	 *
	 * The constructor does not allocate any objects or memory.  This allows
	 * us to have a non-pointer reference to this controller, reducing our
	 * memory allocation.  Instead, allocation happens in this method.
	 *
	 * The game world is scaled so that the screen coordinates do not agree
	 * with the Box2d coordinates.  This initializer uses the default scale.
	 *
	 * @param assets    The (loaded) assets for this game mode
	 *
	 * @return true if the controller is initialized properly, false otherwise.
	 */
	bool init(const std::shared_ptr<cugl::AssetManager>& assets,
		const std::shared_ptr<NetEventController> network, bool isHost);

	/**
	 * Initializes the controller contents, and starts the game
	 *
	 * The constructor does not allocate any objects or memory.  This allows
	 * us to have a non-pointer reference to this controller, reducing our
	 * memory allocation.  Instead, allocation happens in this method.
	 *
	 * The game world is scaled so that the screen coordinates do not agree
	 * with the Box2d coordinates.  The bounds are in terms of the Box2d
	 * world, not the screen.
	 *
	 * @param assets    The (loaded) assets for this game mode
	 * @param rect      The game bounds in Box2d coordinates
	 *
	 * @return  true if the controller is initialized properly, false otherwise.
	 */
	bool init(const std::shared_ptr<cugl::AssetManager>& assets,
		const cugl::Rect rect,
		const std::shared_ptr<NetEventController> network, bool isHost);
    
    void constructLevel();
    
    void constructSceneNodes(const Size &dimen);
    
    /**
	 * Initializes the controller contents, and starts the game
	 *
	 * The constructor does not allocate any objects or memory.  This allows
	 * us to have a non-pointer reference to this controller, reducing our
	 * memory allocation.  Instead, allocation happens in this method.
	 *
	 * The game world is scaled so that the screen coordinates do not agree
	 * with the Box2d coordinates.  The bounds are in terms of the Box2d
	 * world, not the screen.
	 *
	 * @param assets    The (loaded) assets for this game mode
	 * @param rect      The game bounds in Box2d coordinates
	 * @param gravity   The gravitational force on this Box2d world
	 *
	 * @return  true if the controller is initialized properly, false otherwise.
	 */
	bool init(const std::shared_ptr<cugl::AssetManager>& assets,
		const cugl::Rect rect, const cugl::Vec2 gravity,
		const std::shared_ptr<NetEventController> network, bool isHost);

#pragma mark -
#pragma mark State Access
	/**
	 * Returns true if the gameplay controller is currently active
	 *
	 * @return true if the gameplay controller is currently active
	 */
	bool isActive() const { return _active; }
	virtual void setActive(bool value) override;

	bool isPaused() const { return _gamePaused; }
	/**
	 * Returns true if debug mode is active.
	 *
	 * If true, all objects will display their physics bodies.
	 *
	 * @return true if debug mode is active.
	 */
	bool isDebug() const { return _debug; }

	/**
	 * Sets whether debug mode is active.
	 *
	 * If true, all objects will display their physics bodies.
	 *
	 * @param value whether debug mode is active.
	 */
	void setDebug(bool value) {
		_debug = value;
		_debugnode->setVisible(value);
	}

	/**
	 * Returns true if the level is completed.
	 *
	 * If true, the level will advance after a countdown
	 *
	 * @return true if the level is completed.
	 */
	bool isComplete() const { return _complete; }

	/**
	 * Sets whether the level is completed.
	 *
	 * If true, the level will advance after a countdown
	 *
	 * @param value whether the level is completed.
	 */
    void setComplete(bool value);
#pragma mark Grab Esther
	/**
	 * This method takes a grabEvent and processes it.
	 */
	void processGrabEvent(const std::shared_ptr<GrabEvent>& event);
#pragma mark Grab Esther

#pragma mark Pause Esther
	/**
	 * This method takes a grabEvent and processes it.
	 */
	void processPauseEvent(const std::shared_ptr<PauseEvent>& event);
#pragma mark Pause Esther

	void processPaintCallbacks(float millis);

	/**
	 * Handles any modifications necessary before collision resolution
	 *
	 * This method is called just before Box2D resolves a collision.  We use
	 * this method to implement sound on contact, using the algorithms outlined
	 * in Ian Parberry's "Introduction to Game Physics with Box2D".
	 *
	 * @param  contact  The two bodies that collided
	 * @param  contact  The collision manifold before contact
	 */
	void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);

	/**
	 * Processes the start of a collision
	 *
	 * This method is called when we first get a collision between two objects.
	 * We use this method to test if it is the "right" kind of collision.  In
	 * particular, we use it to test if we make it to the win door.
	 *
	 * @param  contact  The two bodies that collided
	 */
	void beginContact(b2Contact* contact);

	/**
	 * Activates world collision callbacks on the given physics world and sets the onBeginContact and beforeSolve callbacks
	 *
	 * @param world the physics world to activate world collision callbacks on
	 */
	void activateWorldCollisions(const std::shared_ptr<physics2::ObstacleWorld>& world);
#pragma mark -
#pragma mark Gameplay Handling

	virtual void preUpdate(float timestep);
	virtual void postUpdate(float timestep);
	virtual void fixedUpdate(float dt);

	/**
	 * I DON'T THINK WE ARE USING THIS METHOD.
	 *
	 * The method called to update the game mode.
	 *
	 * This method contains any gameplay code that is not an OpenGL call.
	 *
	 * @param timestep  The amount of time (in seconds) since the last frame
	 */
	void update(float timestep) override;

    
    /**
	 * Resets the status of the game so that we can play again.
	 */
	void reset();

	void addPaintObstacles();

	void cameraReset();
    
    void constructCharacterA();
    void constructInputController(const cugl::Rect &rect);
};

#endif
