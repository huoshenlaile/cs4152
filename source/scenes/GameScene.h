#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include "../controllers/AudioController.h"
#include "../controllers/InputController.h"
#include "../controllers/InteractionController2.h"
#include <box2d/b2_collision.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_world.h>
#include <ctime>
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include <string>

#include "../controllers/CameraController.h"
#include "../controllers/InputController.h"
#include "../helpers/LevelConstants.h"
#include "../helpers/LevelLoader2.h"

// INCLUDE THIS AT LAST to avoid repeat naming (will trigger bewildering bugs)
#include "../helpers/GameSceneConstants.h"

class GameScene : public cugl::Scene2 {
protected:
    std::shared_ptr<cugl::AssetManager> _assets;
    std::string _levelName;
    std::shared_ptr<InteractionController2> _interactionController;
    std::shared_ptr<CharacterController> _character;
    std::shared_ptr<AudioController> _audioController;
    std::shared_ptr<InputController> _inputController;

    std::shared_ptr<cugl::physics2::ObstacleWorld> _platformWorld;

    /** Note that this node contains the pause button AND the map button. */
    std::shared_ptr<cugl::scene2::SceneNode> _pauseButtonNode;
    std::shared_ptr<cugl::scene2::Button> _pauseButton;
    std::shared_ptr<cugl::scene2::Button> _mapButton;

	/** Reference to the physics root of the scene graph */
	std::shared_ptr<cugl::scene2::SceneNode> _worldnode; // the root node
	std::shared_ptr<cugl::scene2::SceneNode> _uinode; // the UI canvas

    /** the level complete scene */
    std::shared_ptr<cugl::scene2::SceneNode> _levelCompleteGood;
    std::shared_ptr<cugl::scene2::Button> _levelCompleteGoodReset;
    std::shared_ptr<cugl::scene2::Button> _levelCompleteGoodMenu;
    std::shared_ptr<cugl::scene2::Button> _levelCompleteGoodNext;
    
    std::shared_ptr<cugl::scene2::SceneNode> _levelCompleteBad;
    std::shared_ptr<cugl::scene2::Button> _levelCompleteBadReset;
    std::shared_ptr<cugl::scene2::Button> _levelCompleteBadMenu;
    std::shared_ptr<cugl::scene2::Button> _levelCompleteBadNext;
    
    std::shared_ptr<cugl::scene2::SpriteNode> _paintMeter;

    cugl::Vec2 _gravity;

    std::shared_ptr<CameraController> _camera;

    std::shared_ptr<LevelLoader2> _level;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;

    /** Whether we have completed this "game" */
    bool _complete;

    bool _gamePaused;
    bool _skipCameraSpan;
        
        
;
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
    // construct UI elements only, game canvas will be loaded from level loader2
    void constructSceneNodes(const Size &dimen);

public:
    int defaultGoodOrBad = -1;
    std::vector<std::shared_ptr<cugl::physics2::Joint>> Alljoints;
    /** the state of this scene, referenced by DPApp*/
    enum SceneState { INGAME, PAUSE, NETERROR, RESET, QUIT, NEXTLEVEL, UPDATING};
    SceneState state;
    
    enum GameEndingState { UNFINISHED, GOOD, BAD };
    GameEndingState gameEndingState;
#pragma mark -
#pragma mark Constructors

    GameScene();
    ~GameScene() { dispose(); }

    void dispose() override;

    bool init(const std::shared_ptr<cugl::AssetManager> &assets, std::string levelName);

#pragma mark -
#pragma mark State Access

    bool isActive() const { return _active; }
    virtual void setActive(bool value) override;

    bool isPaused() const { return _gamePaused; }

    int getEndingType() const {return defaultGoodOrBad;}
    
    bool isComplete() const { return _complete; }

    void setComplete(bool value) { _complete = value; }

    void setCameraSkip (bool val) {
        _skipCameraSpan = val;
    }
    
    void setCameraState (int state) {_camera->setCameraState(state);}
    
#pragma mark -

    virtual void preUpdate(float dt);
    void finishLevel();
    
    virtual void postUpdate(float dt);
    virtual void fixedUpdate(float dt);

    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();
    bool isCharacterInMap();
    void setCamera(std::string selectedLevelKey);
};

#endif
