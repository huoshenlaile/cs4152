#ifndef __CAMERA__CONTROLLER_H
#define __CAMERA__CONTROLLER_H

#include <chrono>
#include <cugl/cugl.h>
#include <stdio.h>
#include <thread>
#include "../helpers/GameSceneConstants.h"
#include "../objects/message.h"
using namespace cugl;

/**
 * A controller for the perspective in the game. It will follow a
 * target at a specific zoom level given the root node of the scene
 * and the target node in the scene lazily given a lerp value.
 */
class CameraController {
protected:
    /** The pointer to current scene graph object that the camera is targeting */
    std::shared_ptr<scene2::SceneNode> _target;
    /** The pointer to root node of the scene */
    std::shared_ptr<scene2::SceneNode> _root;
    /** The UI layer that will move with the camera */
    std::shared_ptr<scene2::SceneNode> _ui;
    /** The camera for this scene */
    std::shared_ptr<cugl::OrthographicCamera> _camera;
    /** The lerp speed of the camera */
    //std::shared_ptr<InteractionController2> _interactionController;
    
    float _lerp;
    /** The time to cancel the pan if someone is panning */
    float _timeToCancelPan;
    /** Timer to handle pan cancelling */
    float _timer;
    /** The maximum allowed zoom */
    float _maxZoom;

    Vec2 _UIPosition;
    int _initialStay, _finalStay;
    std::vector<int> _inBetweenPanelSize;
    bool _paused = false;
    Vec2 _initialPos;
    bool _levelComplete, _completed, _initialUpdate, _displayed, _moveToLeft, _moveToTop, _replay, _skipCameraSpan, _skipPosMove;
    float _defaultZoom, _levelCompleteZoom;
    Vec2 _panSpeed;
    bool _horizontal;
    int _finalPos;
    int _state;
    int _counter;
    int _initPosOnce;
    
    std::map<std::string, std::function<PublishedMessage(ActionParams)>> uiActions;
public:
    
    int cur_panel = 0;

    /**
     * Creates a new camera controller with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    CameraController(){};

    /**
     * Destroys the CameraController, releasing all resources.
     */
    virtual ~CameraController(void){};

    static std::shared_ptr<CameraController> alloc(const std::shared_ptr<cugl::scene2::SceneNode> target, const std::shared_ptr<cugl::scene2::SceneNode> root, float lerp,
                                                   std::shared_ptr<cugl::OrthographicCamera> camera, std::shared_ptr<scene2::SceneNode> ui, float maxZoom, bool horizontal, bool skipCameraSpan) {
        std::shared_ptr<CameraController> result = std::make_shared<CameraController>();
        return (result->init(target, root, lerp, camera, ui, maxZoom, horizontal, skipCameraSpan) ? result : nullptr);
    }

    /**
     * Initializes the controller, and starts the camera tracking.
     *
     * @param target     The scene graph object that the camera will target
     * @param root       The root node in our scene graph so the camera can move the view
     * @param lerp       The lerp speed of the camera
     * @param screenSize The width and height of the screen
     *
     * @return true if the controller is initialized properly, false otherwise
     */
    bool init(const std::shared_ptr<cugl::scene2::SceneNode> target, const std::shared_ptr<cugl::scene2::SceneNode> root, float lerp, std::shared_ptr<cugl::OrthographicCamera> camera,
              std::shared_ptr<scene2::SceneNode> ui, float maxZoom, bool horizontal, bool skipCameraSpan);

    /**
     * The method called to update the camera
     *
     * This method will move the view towards the target based off of lerp
     *
     * @param dt The delta time
     */
    void update(float dt);

    /**
     * This method sets the zoom of the camera by a multiplier.
     *
     * @param zoom The zoom multiplier
     */
    void setZoom(float zoom);

    /**
     * This method adds the zoom of the camera by a value.
     *
     * @param zoom The zoom value
     */
    void addZoom(float zoom);

    /**
     * This method _state = 1;s the target of the camera.
     *
     * @param target The target to set the camera to
     */
    void setTarget(const std::shared_ptr<cugl::scene2::SceneNode> target);

    /**
     * Gets the camera
     *
     * @return The camera
     */
    std::shared_ptr<cugl::OrthographicCamera> getCamera() { return _camera; };

    void process(int zoomIn, float speed);

    Vec2 getUIPosition() { return _UIPosition; }
    void setInitialStay(int stay) { _initialStay = stay; }
    void setFinalStay(int stay) { _finalStay = stay; }
    void setDisplayed(bool display) {_displayed = display;}
    bool getDisplayed() { return _displayed; }
    void setDefaultZoom(float zoom) { _defaultZoom = zoom; }
    float getDefaultZoom() { return _defaultZoom; }
    void setLevelCompleteZoom(float zoom) { _levelCompleteZoom = zoom; }
    float getLevelCompleteZoom() { return _levelCompleteZoom; }
    void setMode(bool mode) { _horizontal = mode; }
    bool getMode() { return _horizontal; }
    bool getInitialUpdate() { return _initialUpdate; }
    void setInitialUpdate(bool update) { _initialUpdate = update; }
    bool getReplay() { return _replay; }
    void setReplay(bool replay) { _replay = replay; }
    
    int getState() const {return _state; }
    void setState(int val) {_state  = val;}
    void levelComplete();
    bool getCameraComplete() { return _completed; }
    bool getLevelComplete() { return _levelComplete; }

    void setCamera(std::string selectedLevelKey, Vec2 activeSize);

    bool cameraStay(int time);

    void setCameraState(int state) { _state = state; }
    
    void setCameraSkip(bool skip){_skipCameraSpan = skip;}
    bool getCameraSkip(){return _skipCameraSpan;}
    
    std::vector<int> getInBetweenPanelSize(){
        return _inBetweenPanelSize;
    }
    bool getPaused(){
        return _paused;
    }
};

#endif
