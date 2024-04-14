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
#include "../models/ButtonModel.h"
#include "../models/WallModel.h"
#include "../helpers/LevelLoader2.h"
#include "../helpers/LevelConstants.h"
#include "../controllers/CameraController.h"
#include "../controllers/PauseEvent.h"
#include "../controllers/InputController.h"

// INCLUDE THIS AT LAST to avoid repeat naming (will trigger bewildering bugs)
#include "../helpers/GameSceneConstants.h"
#include "../models/PaintGrowerModel.h"

class GameScene : public cugl::Scene2 {
protected:
	std::shared_ptr<cugl::AssetManager> _assets;


	std::shared_ptr<InteractionController> _interactionController;
	std::shared_ptr<CharacterController> _characterController;
	std::shared_ptr<AudioController> _audioController;
	std::shared_ptr<InputController> _inputController;

	std::shared_ptr<cugl::physics2::ObstacleWorld> _platformWorld;
    
    std::shared_ptr<cugl::scene2::Button> _pauseButton;



	/** Reference to the physics root of the scene graph */
	std::shared_ptr<cugl::scene2::SceneNode> _worldnode;
	std::shared_ptr<cugl::scene2::SceneNode> _platformNode;
	std::shared_ptr<cugl::scene2::Label> _winnode;
	std::shared_ptr<cugl::scene2::SceneNode> _uinode;

    /** the level complete scene */
    std::shared_ptr<cugl::scene2::SceneNode> _levelComplete;
    std::shared_ptr<cugl::scene2::Button> _levelCompleteReset;
    std::shared_ptr<cugl::scene2::Button> _levelCompleteMenuButton;
    cugl::Vec2 _gravity;

	CameraController _camera;

	std::shared_ptr<LevelLoader2> _level;
	/** The scale between the physics world and the screen (MUST BE UNIFORM) */
	float _scale;


	/** Whether we have completed this "game" */
	bool _complete;

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
	bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    void constructSceneNodes(const Size &dimen);
    

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

#pragma mark Pause Esther


#pragma mark -
#pragma mark Gameplay Handling

	virtual void preUpdate(float timestep);
	virtual void postUpdate(float timestep);
	virtual void fixedUpdate(float dt);


    /**
	 * Resets the status of the game so that we can play again.
	 */
	void reset();

	void cameraReset();


};

#endif
