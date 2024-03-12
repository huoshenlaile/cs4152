#include "GameScene.h"

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
    
    // TODO: add children to the scene, initialize Controllers
    // Create the scene graph
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);
    
    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);
    
    addChild(_worldnode);
    addChild(_debugnode);
    
    populate();
    _active = true;
    _complete = false;
    setDebug(false);
    
    auto source = _assets->get<Sound>("PhantomLiberty");
    std::string key = "1234123414523145";
    if (!AudioEngine::get()->isActive(key)) {
        AudioEngine::get() -> play(key, source, false, source->getVolume());
    }
    
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
        _world = nullptr;
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
    populate();
    Application::get()->resetFixedRemainder();
}


void GameScene::populate() {
    _world = physics2::net::NetWorld::alloc(Rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT),Vec2(0,DEFAULT_GRAVITY));
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact* contact) {
        _interactionController.beginContact(contact);
    };
    _world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
        _interactionController.beforeSolve(contact,oldManifold);
    };
}


void GameScene::linkSceneToObs(const std::shared_ptr<physics2::Obstacle>& obj,
    const std::shared_ptr<scene2::SceneNode>& node) {
    node->setPosition(obj->getPosition() * _scale);
    _worldnode->addChild(node);

    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs) {
            float leftover = Application::get()->getFixedRemainder() / 1000000.f;
            Vec2 pos = obs->getPosition() + leftover * obs->getLinearVelocity();
            float angle = obs->getAngle() + leftover * obs->getAngularVelocity();
            weak->setPosition(pos * _scale);
            weak->setAngle(angle);
            });
    }
}


void GameScene::addInitObstacle(const std::shared_ptr<physics2::Obstacle>& obj,
    const std::shared_ptr<scene2::SceneNode>& node) {
    _world->initObstacle(obj);
    if (_isHost) {
        _world->getOwnedObstacles().insert({ obj,0 });
    }
    linkSceneToObs(obj, node);
}

#pragma mark Physics Handling

void GameScene::preUpdate(float dt) {
    _input.update();
}


void GameScene::postUpdate(float dt) {
}


void GameScene::fixedUpdate() {
    _world->update(FIXED_TIMESTEP_S);
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
