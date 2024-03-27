#include "GameScene.h"

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

bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets,
	const std::shared_ptr<NetEventController> network,
	bool isHost) {
	return GameScene::init(assets, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT),
		Vec2(0, CHARACTER_GRAVITY), network, isHost);
}

bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets,
	const cugl::Rect rect,
	const std::shared_ptr<NetEventController> network,
	bool isHost) {
	return init(assets, rect, Vec2(0, CHARACTER_GRAVITY), network, isHost);
}

bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets,
	const cugl::Rect rect, const cugl::Vec2 gravity,
	const std::shared_ptr<NetEventController> network,
	bool isHost) {
	Size dimen = computeActiveSize();

	if (assets == nullptr) {
		return false;
	}
	else if (!Scene2::init(dimen)) {
		return false;
	}

	state = INGAME;
	_isHost = isHost;
	_network = network;
	_assets = assets;

	_scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width
		: dimen.height / rect.size.height;
	Vec2 offset{ (dimen.width - SCENE_WIDTH) / 2.0f,
				(dimen.height - SCENE_HEIGHT) / 2.0f };

	//CULog("dimen: %f, %f", dimen.width, dimen.height);
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
		_assets->get<scene2::SceneNode>("pausebutton"));
	_pause->addListener([this](const std::string& name, bool down) {
		if (down) {
			_gamePaused = true;
			//			CULog("Pause button hit");
			//			_network->pushOutEvent(PauseEvent::allocPauseEvent(
			//				Vec2(DEFAULT_WIDTH / 2, DEFAULT_HEIGHT / 2), true));
		}
		});
	_uinode->addChild(_pause);
	addChild(_worldnode);
	addChild(_uinode);

	_worldnode->setContentSize(Size(SCENE_WIDTH, SCENE_HEIGHT));

#pragma mark LevelLoader
	// make the getter for loading the map
	_level = assets->get<LevelLoader>(ALPHA_RELEASE_KEY);
	if (_level == nullptr) {
		CULog("Fail loading levels");
		return false;
	}
	_level->setAssets(_assets);
	_level->setRootNode(_worldnode);
	_platformWorld = _level->getPhysicsWorld();

	_platformWorld->setGravity(gravity);

	// TODO: MOVE THIS TO LEVEL LOADER
	Rect rec3(0, 0, 700, 700);
	Rect rec2(0, 0, 600, 600);
	Rect rec1(0, 0, 500, 500);

	std::vector<Vec2> locations1{ {180, 50}, {180, 50}, {180, 50} };
	auto pm = PaintModel::alloc({ rec1, rec2, rec3 }, locations1, _assets, _worldnode, _scale);
	_paintModels.push_back(pm);

	std::vector<Vec2> locations2{ {200, 70}, {200, 70}, {200, 70} };
	auto pm2 = PaintModel::alloc({ rec1, rec2, rec3 }, locations2, _assets, _worldnode, _scale);
	_paintModels.push_back(pm2);

#pragma mark Character 1
	_characterControllerA = CharacterController::alloc(_level->getCharacterPos(), _scale);
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
	_inputController = std::make_shared<InputController>();
	_inputController->init(rect);
	_inputController->fillHand(_characterControllerA->getLeftHandPosition(),
		_characterControllerA->getRightHandPosition(),
		_characterControllerA->getLHPos(),
		_characterControllerA->getRHPos());

#pragma mark InteractionController
	_assets->load<JsonValue>(ALPHA_RELEASE_KEY_JSON, ALPHA_RELEASE_FILE);
	_interactionController.init({}, _characterControllerA, nullptr, {}, {}, _level, _assets->get<JsonValue>(ALPHA_RELEASE_KEY_JSON));

	//    _camera.init(charNode,_worldnode,1.0f,
	//    std::dynamic_pointer_cast<OrthographicCamera>(getCamera()),
	//    _uinode, 2.0f);
	_active = false;
	_gamePaused = false;
	_complete = false;
	setDebug(false);
	activateWorldCollisions(_platformWorld);

#pragma mark AudioController
	//    _audioController = std::make_shared<AudioController>();
	//    _audioController->init(_assets);
	//    _audioController->play("box2DtheWORSTphysicsEngineIEverUsed",
	//    "PhantomLiberty");

	/*_camera.setTarget(_characterControllerA->getBodySceneNode());*/
	_camera.init(_characterControllerA->getBodySceneNode(), _worldnode, 10.0f,
		std::dynamic_pointer_cast<OrthographicCamera>(getCamera()),
		_uinode, 5.0f);
	//CULog("Character Pos: %f, %f", _characterControllerA->getBodySceneNode()->getPositionX(), _characterControllerA->getBodySceneNode()->getPositionY());
	_camera.setZoom(DEFAULT_ZOOM);
    
    
    
    std::shared_ptr<scene2::SceneNode> levelComplete = _assets->get<scene2::SceneNode>("levelcomplete");
    Size tryDimen = Application::get()->getDisplaySize();
    levelComplete -> doLayout(); // Repositions the HUD
    levelComplete -> setContentSize(dimen);
    levelComplete -> setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    addChild(levelComplete);

	return true;
}

void GameScene::addPaintObstacles() {
}

/**
 * Activates world collision callbacks on the given physics world and sets the onBeginContact and beforeSolve callbacks
 *
 * @param world the physics world to activate world collision callbacks on
 */
void GameScene::activateWorldCollisions(const std::shared_ptr<physics2::ObstacleWorld>& world) {
	world->activateCollisionCallbacks(true);
	world->onBeginContact = [this](b2Contact* contact) {
		_interactionController.beginContact(contact);
		};
	world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
		_interactionController.beforeSolve(contact, oldManifold);
		};
	world->onEndContact = [this](b2Contact* contact) {
		_interactionController.endContact(contact);
		};
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
	if (_active) {
		// TODO: implemetation
		removeAllChildren();
		_worldnode = nullptr;
		_debugnode = nullptr;
		_winnode = nullptr;
		_complete = false;
		_debug = false;
		_characterControllerA->dispose();
		//_characterControllerB->dispose();
		Scene2::dispose();
	}
}

void GameScene::setActive(bool value) {
	if (isActive() != value) {
		Scene2::setActive(value);
		if (value) {
			_pause->activate();
			_gamePaused = false;
		}
		else {
			_pause->deactivate();
			_pause->setDown(false);
		}
	}
	_inputController = std::make_shared<InputController>();
	_inputController->init(Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT));
	_inputController->fillHand(_characterControllerA->getLeftHandPosition(),
		_characterControllerA->getRightHandPosition(),
		_characterControllerA->getLHPos(),
		_characterControllerA->getRHPos());
}

#pragma mark -
#pragma mark Level Layout

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void GameScene::reset() {
	// reload the level

//	CULog("GAME RESET");
    for(auto & pm : _paintModels){
        pm->clear();
    }
	_assets->unload<LevelLoader>(ALPHA_RELEASE_KEY);
	_assets->load<LevelLoader>(ALPHA_RELEASE_KEY, ALPHA_RELEASE_FILE);

	_level = nullptr;
	_level = _assets->get<LevelLoader>(ALPHA_RELEASE_KEY);
	_level->setAssets(_assets);
	_level->setRootNode(_worldnode);

	_platformWorld = nullptr;
	_platformWorld = _level->getPhysicsWorld();
    
    //TODO: refactor out adding to scene so we don't need to reinstatiate everything - do this after LevelLoader refactor
    std::vector<std::shared_ptr<PaintModel>> newPaints;
    for (auto & pm : _paintModels){
        auto re_pm = PaintModel::alloc(pm->getPaintFrames(), pm->getLocations(), _assets, _worldnode, _scale);
        newPaints.push_back(re_pm);
    }
    _paintModels = newPaints;
	//reload the character
//    _characterControllerA = nullptr;
	_characterControllerA = CharacterController::alloc(_level->getCharacterPos(), _scale);
	CULog("7538fe43 _scale = %f", _scale);
	_characterControllerA->buildParts(_assets);
	_characterControllerA->createJoints();
	auto charNode = scene2::SceneNode::alloc();
	_worldnode->addChild(charNode);
	_characterControllerA->linkPartsToWorld(_platformWorld, charNode, _scale);
	setComplete(false);

	//reload the input
	_inputController = std::make_shared<InputController>();
	_inputController->init(Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT));
	_inputController->fillHand(_characterControllerA->getLeftHandPosition(),
		_characterControllerA->getRightHandPosition(),
		_characterControllerA->getLHPos(),
		_characterControllerA->getRHPos());

	//reload interaction controller
	_assets->unload<JsonValue>(ALPHA_RELEASE_KEY_JSON);
	_assets->load<JsonValue>(ALPHA_RELEASE_KEY_JSON, ALPHA_RELEASE_FILE);
	_interactionController.init({}, _characterControllerA, nullptr, {}, {}, _level, _assets->get<JsonValue>(ALPHA_RELEASE_KEY_JSON));
	//reload the camera
	cameraReset();
	//_camera.setTarget(_characterControllerA->getBodySceneNode());

	Application::get()->resetFixedRemainder();
	activateWorldCollisions(_platformWorld);
}

/**
 * This method takes a grabEvent and processes it.
 */
void GameScene::processGrabEvent(const std::shared_ptr<GrabEvent>& event) {
	// TODO: Waiting for Other Module
}

/**
 * This method takes a pauseEvent and processes it.
 */
void GameScene::processPauseEvent(const std::shared_ptr<PauseEvent>& event) {
	std::cout << "GAME PAUSE PROCESS EVENT" << std::endl;
	if (event->isPause()) {
		_gamePaused = true;
	}
	else {
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
	auto character = _inputController->getCharacter();
	for (auto i = character->_touchInfo.begin(); i != character->_touchInfo.end(); i++) {
		i->worldPos = (Vec2)Scene2::screenToWorldCoords(i->position);
		//CULog("Touch coord at: %f %f \n", i->position.x, i->position.y);
		//CULog("World coord at: %f %f \n", i->worldPos.x, i->worldPos.y);
	}
	//_inputController->worldtouchPos = (Vec2)Scene2::screenToWorldCoords(_inputController->touchPos);

	_inputController->fillHand(_characterControllerA->getLeftHandPosition(),
		_characterControllerA->getRightHandPosition(),
		_characterControllerA->getLHPos(),
		_characterControllerA->getRHPos());
	_inputController->process();

	/*if (_inputController->didPress()) {
		CULog("Here!!!");
		CULog("World coord at: %f %f \n", _inputController->touchPos.x, _inputController->touchPos.y);
		_inputController->worldtouchPos = (Vec2)Scene2::screenToWorldCoords(_inputController->touchPos);
	}*/
	//_inputController->process();
	//Vec2 touchPos;
	/*if (_inputController->didPress() || _inputController->isDown()) {
		Vec3 worldCoords = Scene2::screenToWorldCoords(_inputController->getEvent().position);
		CULog("Event coord at: %f %f \n", _inputController->getEvent().position.x, _inputController->getEvent().position.y);
		touchPos = (Vec2)worldCoords;
		_inputController->setTouchEventPos(touchPos);
		_inputController->touchPos = Vec2(touchPos.x, touchPos.y);
		CULog("World coord at: %f %f \n", touchPos.x, touchPos.y);
		CULog("World coord at: %f %f \n", _inputController->getEvent().position.x, _inputController->getEvent().position.y);
		CULog("World coord at: %f %f \n", _inputController->touchPos.x, _inputController->touchPos.y);
	}*/
	_characterControllerA->moveLeftHand(INPUT_SCALER *
		_inputController->getLeftHandMovement());
	_characterControllerA->moveRightHand(
		INPUT_SCALER * _inputController->getrightHandMovement());
	_inputController->fillHand(_characterControllerA->getLeftHandPosition(),
		_characterControllerA->getRightHandPosition(),
		_characterControllerA->getLHPos(),
		_characterControllerA->getRHPos());

	//_interactionController.preUpdate(dt);
	while (!_interactionController.messageQueue.empty()) {
		InteractionController::PublisherMessage publication = _interactionController.messageQueue.front();
		std::cout << "PUB: " << publication.pub_id << " " << publication.trigger << " " << publication.message << "\n";
		for (const InteractionController::SubscriberMessage& s : _interactionController.subscriptions[publication.pub_id][publication.message]) {
			std::cout << s.pub_id << " " << s.listening_for << "\n";
			if (s.actions.count("win") > 0) {
				CULog("Winner!");
				setComplete(true);
			}
			if (s.actions.count("fire") > 0) {
				auto bottle = std::stoi(s.actions.at("fire"));
				std::cout << "Firing bottle <" << bottle << ">\n\n";
				//TODO: Remove after triggering, probably. I think we should keep it as a vector though, not map
				_paintModels[bottle]->trigger();
				// s.actions.at("fire") is the name of the paint bottle obstacle
			}
		}
		_interactionController.messageQueue.pop();
	}
	// TODO: error handle for loading different levels when we have multiple
	// levels
	_camera.update(dt);
	//CULog("Character Pos: %f, %f", _characterControllerA->getBodySceneNode()->getPositionX(), _characterControllerA->getBodySceneNode()->getPositionY());
	//_camera.process(ZOOMIN, 0.01);
	//_camera.process(ZOOMOUT, 0.01);

	// TODO: if (indicator == true), allocate a crate event for the center of the
	// screen(use DEFAULT_WIDTH/2 and DEFAULT_HEIGHT/2) and send it using the
	// pushOutEvent() method in the network controller.
	//    if (???????){
	//        CULog("Grab Event COMING");
	//        _network->pushOutEvent(GrabEvent::allocGrabEvent(Vec2(DEFAULT_WIDTH/2,DEFAULT_HEIGHT/2)));
	//    }

	processPaintCallbacks(dt);
	_camera.setTarget(_characterControllerA->getBodySceneNode());
	//_interactionController.detectPolyContact(_paintModel.currentNode(), _scale);
}

void GameScene::processPaintCallbacks(float millis) {
	for (auto& pm : _paintModels) {
		pm->update(_worldnode, millis);
		if (pm->active) {
			_interactionController.detectPolyContact(pm->currentNode(), _scale);
		}
	}

	//    _levelComplete -> setPosition(_camera.getUIPosition());
}

void GameScene::postUpdate(float dt) {
	// _platformWorld->setGravity(Vec2(0, -30.0f));
	// CULog("_platformWorld gravity: %f, %f", _platformWorld->getGravity().x, _platformWorld->getGravity().y);
	// _platformWorld->setGravity(Vec2(0, -98));
}

void GameScene::fixedUpdate(float dt) {
	// TODO: check for available incoming events from the network controller and
	// call processGrabEvent if it is a GrabEvent.

//    processPauseEvent();
//	if (_network->isInAvailable()) {
//		auto e = _network->popInEvent();
//		if (auto grabEvent = std::dynamic_pointer_cast<GrabEvent>(e)) {
//			// CULog("BIG Grab Event GOT");
//			processGrabEvent(grabEvent);
//		}
//		else if (auto pauseEvent = std::dynamic_pointer_cast<PauseEvent>(e)) {
//			CULog("BIG Pause Event GOT");
//			processPauseEvent(pauseEvent);
//		}
//	}

	// TODO: check for available incoming events from the network controller and
	// call processGrabEvent if it is a GrabEvent. std::cout << "position" <<
	// _characterControllerA->getBodySceneNode()->getPositionX() <<
	// _characterControllerA->getBodySceneNode()->getPositionY() << std::endl;
	_platformWorld->update(dt);
	_characterControllerA->update(dt);
	//_camera.setTarget(_characterControllerA->getBodySceneNode());
}

void GameScene::update(float dt) {
	// deprecated
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


void GameScene::cameraReset() {
	_camera.init(_characterControllerA->getBodySceneNode(), _worldnode, 10.0f,
		std::dynamic_pointer_cast<OrthographicCamera>(getCamera()),
		_uinode, 5.0f);
	_camera.setMove(false);
	_camera.setZoom(DEFAULT_ZOOM);
	_camera.setInitialStay(INITIAL_STAY + 1);
	_camera.setFinalStay(FINAL_STAY + 1);
}
