#include "GameScene.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include <ctime>
#include <string>
#include <iostream>
#include <sstream>

using namespace cugl;
using namespace cugl::physics2::net;

#define STATIC_COLOR    Color4::WHITE
#define PRIMARY_FONT        "retro"

#pragma mark Initializers and Disposer
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameScene::GameScene() : cugl::Scene2(),
_complete(false),
_debug(false),
_isHost(false)
{
}


bool GameScene::init(const std::shared_ptr<cugl::AssetManager> &assets, const std::shared_ptr<NetEventController> network, bool isHost) {
    return GameScene::init(assets, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT), Vec2(0, DEFAULT_GRAVITY), network, isHost);
}


bool GameScene::init(const std::shared_ptr<cugl::AssetManager> &assets, const cugl::Rect rect, const std::shared_ptr<NetEventController> network, bool isHost) {
    return init(assets, rect, Vec2(0, DEFAULT_GRAVITY), network, isHost);
}


bool GameScene::init(const std::shared_ptr<cugl::AssetManager> &assets, const cugl::Rect rect, const cugl::Vec2 gravity, const std::shared_ptr<NetEventController> network, bool isHost) {
    Size dimen = computeActiveSize();
        
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    state = INGAME;
    _isHost = isHost;
    _network = network;
    _assets = assets;
    _platformWorld = physics2::net::NetWorld::alloc(Rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT),Vec2(0,DEFAULT_GRAVITY));
    _input.init(rect);
    _input.update(0);
    
    _scale = dimen.width == SCENE_WIDTH ? dimen.width/rect.size.width : dimen.height/rect.size.height;
    Vec2 offset {(dimen.width - SCENE_WIDTH) / 2.0f, 
                    (dimen.height - SCENE_HEIGHT) / 2.0f};


    
    // TODO: add children to the scene, initialize Controllers
    // Create the scene graph
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);
    
    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);
    
    _winnode = scene2::Label::allocWithText("VICTORY!", _assets->get<Font>(PRIMARY_FONT));
    _winnode->setAnchor(Vec2::ANCHOR_CENTER);
    _winnode->setPosition(computeActiveSize() / 2);
    _winnode->setForeground(STATIC_COLOR);
    _winnode->setVisible(false);
    
    _uinode = scene2::SceneNode::alloc();
    _uinode->setContentSize(dimen);
    _uinode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _uinode->addChild(_winnode);
    
    addChild(_worldnode);
    addChild(_uinode);
    _worldnode->setContentSize(Size(SCENE_WIDTH,SCENE_HEIGHT));
    
    
    //make the getter for loading the map
    _level = assets->get<LevelLoader>(LEVEL_ONE_KEY);
    if (_level == nullptr) {
        CULog("Fail!");
        return false;
    }
    _level->setWorld(_platformWorld);
    _level->setAssets(_assets);
    _level->setRootNode(_worldnode);

    
#pragma mark Character 1
<<<<<<< HEAD
    _characterControllerA = CharacterController::alloc({10,8},  200);
    _characterControllerA->buildParts(_assets);
    _characterControllerA->createJoints();
    
    auto charNode = scene2::SceneNode::alloc();
    _worldnode->addChild(charNode);
    //_characterController->setSceneNode(charNode);
    _characterControllerA->linkPartsToWorld(_platformWorld, charNode, _scale);

#pragma mark Character 2
<<<<<<< HEAD
    _characterControllerB = CharacterController::alloc({20,8}, 200);
=======
    _characterControllerB = CharacterController::alloc({20,8},200);
>>>>>>> parent of 6d9fa5f (Merge remote-tracking branch 'refs/remotes/origin/main')
    _characterControllerB->buildParts(_assets);
    _characterControllerB->createJoints();
    
    auto charNodeB = scene2::SceneNode::alloc();
    _worldnode->addChild(charNodeB);
    _characterControllerB->linkPartsToWorld(_platformWorld, charNodeB, _scale);
=======
    _characterController = CharacterController::alloc({15,8},200);
    _characterController->buildParts(_assets);
    _characterController->createJoints();
    
    auto charNode = scene2::SceneNode::alloc();
    _worldnode->addChild(charNode);
//    _characterController->setSceneNode(charNode);
//    _characterController->setDrawScale(16.0f);
    //_characterController->activate(_platformWorld);
    _characterController->linkPartsToWorld(_platformWorld, charNode, _scale);
>>>>>>> 957b87a (debugging)
    
    
#pragma mark Esther
    _network->attachEventType<GrabEvent>();
    _network->attachEventType<PauseEvent>();
#pragma mark Esther
    
    
    //TODO: fix this init after finishing characterControllers
    _interactionController.init({}, nullptr, nullptr, {}, {});
    
    //TODO: remove, this is for testing purposes
    InteractionController::PublisherMessage pub = { "button1", "pressed", "pressed", nullptr};
    _interactionController.publishMessage(std::move(pub));
    
    _camera.init(_worldnode,_worldnode,1.0f, std::dynamic_pointer_cast<OrthographicCamera>(getCamera()), _uinode, 2.0f);
    _active = true;
    _complete = false;
    setDebug(false);
    
    
//    _audioController = std::make_shared<AudioController>();
//    _audioController->init(_assets);
//    _audioController->play("box2DtheWORSTphysicsEngineIEverUsed", "PhantomLiberty");
    
    
    return true;
}


/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        
        // TODO: implemetation
        
        removeAllChildren();
        _input.dispose();
        _worldnode = nullptr;
        _debugnode = nullptr;
        _winnode = nullptr;
        _complete = false;
        _debug = false;
        _characterControllerA->dispose();
        _characterControllerB->dispose();
        Scene2::dispose();
    }
}

#pragma mark -
#pragma mark Level Layout

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void GameScene::reset() {
    CULog("GAME RESET");
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    setComplete(false);
    Application::get()->resetFixedRemainder();
}


#pragma mark Grab Esther
/**
 * This method takes a grabEvent and processes it.
 */
void GameScene::processGrabEvent(const std::shared_ptr<GrabEvent>& event){
    //TODO: Waiting for Other Module
}
#pragma mark Grab Esther


#pragma mark Pause Esther
/**
 * This method takes a grabEvent and processes it.
 */
void GameScene::processPauseEvent(const std::shared_ptr<PauseEvent>& event){
    //TODO: Waiting for Other Module
}
#pragma mark Pause Esther


#pragma mark Physics Handling

void GameScene::preUpdate(float dt) {
    if(_level == nullptr){
        return;
    }
    _input.update(dt);
    _interactionController.preUpdate(dt);
    //TODO: error handle for loading different levels when we have multiple levels
    _camera.update(dt);

    
#pragma mark Grab Esther
    //TODO: if (indicator == true), allocate a crate event for the center of the screen(use DEFAULT_WIDTH/2 and DEFAULT_HEIGHT/2) and send it using the pushOutEvent() method in the network controller.
//    if (???????){
//        CULog("Grab Event COMING");
//        _network->pushOutEvent(GrabEvent::allocGrabEvent(Vec2(DEFAULT_WIDTH/2,DEFAULT_HEIGHT/2)));
//    }
    
#pragma mark Grab Esther
    
    
    
#pragma mark Pause Esther
    //TODO: if (indicator == true), allocate a pause event for the center of the screen(use DEFAULT_WIDTH/2 and DEFAULT_HEIGHT/2) and send it using the pushOutEvent() method in the network controller.
//    if (???????){
//        CULog("Pause Event COMING");
//        _network->pushOutEvent(PauseEvent::allocPauseEvent(Vec2(DEFAULT_WIDTH/2,DEFAULT_HEIGHT/2)));
//    }
    
#pragma mark Pause Esther
    
    
    
}


void GameScene::postUpdate(float dt) {
}


void GameScene::fixedUpdate() {
#pragma mark Grab Esther
    //TODO: check for available incoming events from the network controller and call processGrabEvent if it is a GrabEvent.
    if(_network->isInAvailable()){
        auto e = _network->popInEvent();
        if(auto grabEvent = std::dynamic_pointer_cast<GrabEvent>(e)){
            CULog("BIG Grab Event GOT");
            processGrabEvent(grabEvent);
        }
    }
#pragma mark Grab Esther
    
    
#pragma mark Pause Esther
    //TODO: check for available incoming events from the network controller and call processGrabEvent if it is a GrabEvent.
    if(_network->isInAvailable()){
        auto e = _network->popInEvent();
        if(auto pauseEvent = std::dynamic_pointer_cast<PauseEvent>(e)){
            CULog("BIG Pause Event GOT");
            processPauseEvent(pauseEvent);
        }
    }
#pragma mark Pause Esther
    
    
    _platformWorld->update(0.1);
    CULog("fixed update called");
}


void GameScene::update(float dt) {
    //deprecated
}


#pragma mark Helper Functions

/**
 * Returns the active screen size of this scene.
 *
 * This method is for graceful handling of different aspect
 * ratios
 */
Size GameScene::computeActiveSize() const {
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width / dimen.height;
    float ratio2 = ((float)SCENE_WIDTH) / ((float)SCENE_HEIGHT);
    if (ratio1 < ratio2) {
        dimen *= SCENE_WIDTH / dimen.width;
    }
    else {
        dimen *= SCENE_HEIGHT / dimen.height;
    }
    return dimen;
}
