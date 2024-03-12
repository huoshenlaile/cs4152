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
    _input.init();
    _input.update();
    _scale = dimen.width == SCENE_WIDTH ? dimen.width/rect.size.width : dimen.height/rect.size.height;
    Vec2 offset {(dimen.width - SCENE_WIDTH) / 2.0f, 
                    (dimen.height - SCENE_HEIGHT) / 2.0f};
    
    //make the getter for loading the map
    _level = assets->get<LevelLoader>(LEVEL_ONE_KEY);
    if (_level == nullptr) {
        CULog("Fail!");
        return false;
    }
    
    _assets = assets;
    std::shared_ptr<cugl::physics2::net::NetWorld> platformWorld = _level->getWorld();

    
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
    _level->setAssets(_assets);
    _level->setRootNode(_worldnode);
    
    
    _camera.init(_worldnode,_worldnode,1.0f, std::dynamic_pointer_cast<OrthographicCamera>(getCamera()), _uinode, 2.0f);
    _active = true;
    _complete = false;
    setDebug(false);
    
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
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    setComplete(false);
    Application::get()->resetFixedRemainder();
}


#pragma mark Physics Handling

void GameScene::preUpdate(float dt) {
    if(_level == nullptr){
        return;
    }
    _input.update();
    //TODO: error handle for loading different levels when we have multiple levels
    _camera.update(dt);
    _level->getWorld()->update(dt);
}


void GameScene::postUpdate(float dt) {
}


void GameScene::fixedUpdate() {
    
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
