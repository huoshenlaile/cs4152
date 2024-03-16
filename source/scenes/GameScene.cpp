#include "GameScene.h"
#include <box2d/b2_collision.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_world.h>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

using namespace cugl;
using namespace cugl::physics2::net;

#define STATIC_COLOR Color4::WHITE
#define PRIMARY_FONT "retro"
/** The message to display on a level reset */
#define RESET_MESSAGE "Resetting"

#pragma mark Initializers and Disposer
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameScene::GameScene()
    : cugl::Scene2(), _complete(false), _debug(false), _isHost(false) {}

bool GameScene::init(const std::shared_ptr<cugl::AssetManager> &assets,
                     const std::shared_ptr<NetEventController> network,
                     bool isHost) {
  return GameScene::init(assets, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT),
                         Vec2(0, DEFAULT_GRAVITY), network, isHost);
}

bool GameScene::init(const std::shared_ptr<cugl::AssetManager> &assets,
                     const cugl::Rect rect,
                     const std::shared_ptr<NetEventController> network,
                     bool isHost) {
  return init(assets, rect, Vec2(0, DEFAULT_GRAVITY), network, isHost);
}

bool GameScene::init(const std::shared_ptr<cugl::AssetManager> &assets,
                     const cugl::Rect rect, const cugl::Vec2 gravity,
                     const std::shared_ptr<NetEventController> network,
                     bool isHost) {
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

  _scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width
                                      : dimen.height / rect.size.height;
  Vec2 offset{(dimen.width - SCENE_WIDTH) / 2.0f,
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

  _loadnode = scene2::Label::allocWithText(RESET_MESSAGE,
                                           _assets->get<Font>(PRIMARY_FONT));
  _loadnode->setAnchor(Vec2::ANCHOR_CENTER);
  _loadnode->setPosition(computeActiveSize() / 2);
  _loadnode->setForeground(STATIC_COLOR);
  _loadnode->setVisible(false);

  _winnode = scene2::Label::allocWithText("VICTORY!",
                                          _assets->get<Font>(PRIMARY_FONT));
  _winnode->setAnchor(Vec2::ANCHOR_CENTER);
  _winnode->setPosition(computeActiveSize() / 2);
  _winnode->setForeground(STATIC_COLOR);
  _winnode->setVisible(false);

  _uinode = scene2::SceneNode::alloc();
  _uinode->setContentSize(dimen);
  _uinode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
  _uinode->addChild(_winnode);
  _uinode->addChild(_loadnode);

  _pause = std::dynamic_pointer_cast<scene2::Button>(
      _assets->get<scene2::SceneNode>("pause"));
  _pause->addListener([this](const std::string &name, bool down) {
    if (down) {
      CULog("Pause button hit");
      _network->pushOutEvent(PauseEvent::allocPauseEvent(
          Vec2(DEFAULT_WIDTH / 2, DEFAULT_HEIGHT / 2), true));
    }
  });
  _uinode->addChild(_pause);

  _pause = std::dynamic_pointer_cast<scene2::Button>(
      _assets->get<scene2::SceneNode>("pause"));

  _pause->addListener([this](const std::string &name, bool down) {
    if (down) {
      CULog("Pause button hit");
      _network->pushOutEvent(PauseEvent::allocPauseEvent(
          Vec2(DEFAULT_WIDTH / 2, DEFAULT_HEIGHT / 2), true));
    }
    // CULog("Pause button hit");
  });

  addChild(_worldnode);
  addChild(_uinode);
  addChild(_pause);

  _worldnode->setContentSize(Size(SCENE_WIDTH, SCENE_HEIGHT));
#pragma mark LevelLoader
  // make the getter for loading the map
  _level = assets->get<LevelLoader>(LEVEL_ONE_KEY);
  if (_level == nullptr) {
    CULog("Fail loading levels");
    return false;
  }
  // TODO: see the changes here - setting the world here somehow passes in our
  // gravity factor.
  //        _platformWorld =
  //        physics2::net::NetWorld::alloc(_level->getBounds(),Vec2(0,DEFAULT_GRAVITY));
  //	    _level->setWorld(_platformWorld);
  _level->setAssets(_assets);
  _level->setRootNode(_worldnode);
  _platformWorld = _level->getPhysicsWorld();

#pragma mark Character 1
  _characterControllerA = CharacterController::alloc({16, 10}, _scale);
  CULog("7538fe43 _scale = %f", _scale);
  _characterControllerA->buildParts(_assets);
  _characterControllerA->createJoints();

  auto charNode = scene2::SceneNode::alloc();
  _worldnode->addChild(charNode);
  //_characterController->setSceneNode(charNode);
  _characterControllerA->linkPartsToWorld(_platformWorld, charNode, _scale);

#pragma mark Character 2
  //    _characterControllerB = CharacterController::alloc({22,8}, 200);
  //    _characterControllerB->buildParts(_assets);
  //    _characterControllerB->createJoints();
  //
  //    auto charNodeB = scene2::SceneNode::alloc();
  //    _worldnode->addChild(charNodeB);
  //    _characterControllerB->linkPartsToWorld(_platformWorld, charNodeB,
  //    _scale);

#pragma mark NetEvents
  _network->attachEventType<GrabEvent>();
  _network->attachEventType<PauseEvent>();

#pragma mark InputControllers
  // TODO: setup the inputController (PlatformInput, from Harry)
  _inputController = std::make_shared<PlatformInput>();
  _inputController->init(rect);
  _inputController->fillHand(_characterControllerA->getLeftHandPosition(),
                             _characterControllerA->getRightHandPosition(),
                             _characterControllerA->getLHPos(),
                             _characterControllerA->getRHPos());

#pragma mark InteractionController
  // TODO: fix this init after finishing characterControllers
  _interactionController.init({}, nullptr, nullptr, {}, {});
  // TODO: remove, this is for testing purposes
  InteractionController::PublisherMessage pub = {"button1", "pressed",
                                                 "pressed", nullptr};
  _interactionController.publishMessage(std::move(pub));

  //    _camera.init(charNode,_worldnode,1.0f,
  //    std::dynamic_pointer_cast<OrthographicCamera>(getCamera()),
  //    _uinode, 2.0f);
  _active = false;
  _gamePaused = false;
  _complete = false;
  setDebug(false);

#pragma mark AudioController
  //    _audioController = std::make_shared<AudioController>();
  //    _audioController->init(_assets);
  //    _audioController->play("box2DtheWORSTphysicsEngineIEverUsed",
  //    "PhantomLiberty");
  _camera.setTarget(_characterControllerA->getBodySceneNode());
  _camera.init(_characterControllerA->getBodySceneNode(), _worldnode, 10.0f,
               std::dynamic_pointer_cast<OrthographicCamera>(getCamera()),
               _uinode, 2.0f);

  return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
  if (_active) {
    // TODO: implemetation

    removeAllChildren();
    //        _input.dispose();
    _inputController->dispose();
    _inputController = nullptr;
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

void GameScene::setActive(bool value) {
  if (isActive() != value) {
    Scene2::setActive(value);
    if (value) {
      _pause->activate();
      _gamePaused = false;
    } else {
      _pause->deactivate();
      _pause->setDown(false);
    }
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
  _camera.setTarget(_characterControllerA->getBodySceneNode());
  Application::get()->resetFixedRemainder();
}

/**
 * This method takes a grabEvent and processes it.
 */
void GameScene::processGrabEvent(const std::shared_ptr<GrabEvent> &event) {
  // TODO: Waiting for Other Module
}

/**
 * This method takes a pauseEvent and processes it.
 */
void GameScene::processPauseEvent(const std::shared_ptr<PauseEvent> &event) {
  std::cout << "GAME PAUSE PROCESS EVENT" << std::endl;
  if (event->isPause()) {
    _gamePaused = true;
  } else {
    _gamePaused = false;
  }
}

#pragma mark Physics Handling

void GameScene::preUpdate(float dt) {
  if (_level == nullptr) {
    return;
  }
  // _input.update();
  _inputController->update(dt);
  _characterControllerA->moveLeftHand(INPUT_SCALER *
                                      _inputController->getLeftHandMovement());
  _characterControllerA->moveRightHand(
      INPUT_SCALER * _inputController->getrightHandMovement());
  _inputController->fillHand(_characterControllerA->getLeftHandPosition(),
                             _characterControllerA->getRightHandPosition(),
                             _characterControllerA->getLHPos(),
                             _characterControllerA->getRHPos());

  _interactionController.preUpdate(dt);
  // TODO: error handle for loading different levels when we have multiple
  // levels
  //    _camera.update(dt);
  //_camera.addZoom(_input.getZoom() * 0.01);
  _camera.update(dt);

  // TODO: if (indicator == true), allocate a crate event for the center of the
  // screen(use DEFAULT_WIDTH/2 and DEFAULT_HEIGHT/2) and send it using the
  // pushOutEvent() method in the network controller.
  //    if (???????){
  //        CULog("Grab Event COMING");
  //        _network->pushOutEvent(GrabEvent::allocGrabEvent(Vec2(DEFAULT_WIDTH/2,DEFAULT_HEIGHT/2)));
  //    }
}

void GameScene::postUpdate(float dt) {
  // CULog("_platformWorld gravity: %f, %f", _platformWorld->getGravity().x,
  // _platformWorld->getGravity().y);
}

void GameScene::fixedUpdate(float dt) {
  // TODO: check for available incoming events from the network controller and
  // call processGrabEvent if it is a GrabEvent.
  if (_network->isInAvailable()) {
    auto e = _network->popInEvent();
    if (auto grabEvent = std::dynamic_pointer_cast<GrabEvent>(e)) {
      // CULog("BIG Grab Event GOT");
      processGrabEvent(grabEvent);
    } else if (auto pauseEvent = std::dynamic_pointer_cast<PauseEvent>(e)) {
      CULog("BIG Pause Event GOT");
      processPauseEvent(pauseEvent);
    }
  }

  // TODO: check for available incoming events from the network controller and
  // call processGrabEvent if it is a GrabEvent. std::cout << "position" <<
  // _characterControllerA->getBodySceneNode()->getPositionX() <<
  // _characterControllerA->getBodySceneNode()->getPositionY() << std::endl;
  _platformWorld->update(dt);
  _camera.update(dt);
}

void GameScene::update(float dt) {
  // deprecated
}

void GameScene::setActive(bool value) {
  Scene2::setActive(value);
  if (value) {
    CULog("Pause activated");
    _pause->activate();
    _gamePaused = false;
  } else {
    _pause->deactivate();
    _pause->setDown(false);
  }
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
  } else {
    dimen *= SCENE_HEIGHT / dimen.height;
  }
  return dimen;
}