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

    std::shared_ptr<cugl::scene2::Button> _pauseButton;

    /** Reference to the physics root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _worldnode;    // the root node
    std::shared_ptr<cugl::scene2::SceneNode> _platformNode; // the platform canvas (from level loader2)
    std::shared_ptr<cugl::scene2::Label> _winnode;          // the win message ( TODO: NOT USED NOW???)
    std::shared_ptr<cugl::scene2::SceneNode> _uinode;       // the UI canvas

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
    // construct UI elements only, game canvas will be loaded from level loader2
    void constructSceneNodes(const Size &dimen);

public:
    std::vector<std::shared_ptr<cugl::physics2::Joint>> Alljoints;
    /** the state of this scene, referenced by DPApp*/
    enum SceneState { INGAME, PAUSE, NETERROR, RESET, QUIT };
    SceneState state;
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

    bool isComplete() const { return _complete; }

    void setComplete(bool value) { _complete = value; }

#pragma mark -

    virtual void preUpdate(float dt);
    virtual void postUpdate(float dt);
    virtual void fixedUpdate(float dt);

    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();
    bool isCharacterInMap();
};

#endif
