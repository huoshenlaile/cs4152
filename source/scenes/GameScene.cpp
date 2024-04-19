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
GameScene::GameScene() : cugl::Scene2(), _complete(false) {}

bool GameScene::init(const std::shared_ptr<cugl::AssetManager> &assets, std::string levelName) {
    Size dimen = computeActiveSize();
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }

    Rect rect = Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    state = INGAME;
    _assets = assets;
    _levelName = levelName;
    _active = false;
    _gamePaused = false;
    _complete = false;
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width : dimen.height / rect.size.height;

#pragma mark fetch from level loader
    _level = _assets->get<LevelLoader2>(levelName);
    _platformWorld = _level->getWorld();

    _worldnode = _level->getWorldNode();
    addChild(_worldnode);
    _character = _level->getCharacter();

#pragma mark Construct UI elements
    constructSceneNodes(dimen);

#pragma mark Construct Interaction Controller
    _interactionController = InteractionController2::alloc(_level);
    _interactionController->activateController();

#pragma mark Construct Input Controller
    _inputController = std::make_shared<InputController>();
    _inputController->init(rect);
    _inputController->fillHand(_character->getLeftHandPosition(), _character->getRightHandPosition(), _character->getLHPos(), _character->getRHPos());
    std::cout << "initialized input controller" << std::endl;

#pragma mark Construct Camera Controller
    _camera.setCamera(levelName);
    _camera.init(_character->getTrackSceneNode(), _worldnode, 10.0f, std::dynamic_pointer_cast<OrthographicCamera>(getCamera()), _uinode, 5.0f, _camera.getMode(), skipCameraSpan);
    _camera.setZoom(_camera.getDefaultZoom());
    
//    _level -> changeBackground(-1);

    return true;
}

void GameScene::dispose() {
    _assets = nullptr;
    _interactionController = nullptr;
    _character = nullptr;
    _audioController = nullptr;
    _inputController = nullptr;
    _platformWorld = nullptr;
    _pauseButtonNode = nullptr;
    _pauseButton = nullptr;
    _worldnode->removeAllChildren();
    _worldnode = nullptr;
    _levelComplete = nullptr;
    _paintMeter = nullptr;
    _level = nullptr;
    this->removeAllChildren();
}

void GameScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _worldnode->setVisible(true);
            _pauseButton->activate();
            _gamePaused = false;
            _inputController->fillHand(_character->getLeftHandPosition(), _character->getRightHandPosition(), _character->getLHPos(), _character->getRHPos());
        } else {
            _pauseButton->deactivate();
            _pauseButton->setDown(false);
        }
    }
}

void GameScene::reset() {
    _assets->unload<LevelLoader2>(_levelName);
    GameScene::dispose();
}

#pragma mark preUpdate
void GameScene::preUpdate(float dt) {
    if (_level == nullptr)
        return;
    // process input
    if (_camera.getDisplayed() || !_inputController->getStarted())
        _inputController->update(dt);
    auto character = _inputController->getCharacter();
    for (auto i = character->_touchInfo.begin(); i != character->_touchInfo.end(); i++) {
        i->worldPos = (Vec2)Scene2::screenToWorldCoords(i->position);
    }
    _inputController->process();
    Size dimen = computeActiveSize();
    float screen_height_multiplier = SCENE_WIDTH / dimen.height;
    //std::cout << "screen_height_multiplier: " << screen_height_multiplier << "\n";
    _character->moveLeftHand(INPUT_SCALER * _inputController->getLeftHandMovement(), _interactionController->leftHandReverse);
    _character->moveRightHand(INPUT_SCALER * _inputController->getrightHandMovement(), _interactionController->rightHandReverse);
    _inputController->fillHand(_character->getLeftHandPosition(), _character->getRightHandPosition(), _character->getLHPos(), _character->getRHPos());

    // update camera
    if (_inputController->getStarted() || !_camera.getInitialUpdate()) {
        _camera.update(dt);
        _camera.setInitialUpdate(true);
    }

    // update interaction controller
    _interactionController->updateHandsHeldInfo(_inputController->isLHAssigned(), _inputController->isRHAssigned());
    _interactionController->preUpdate(dt);

    if (!isCharacterInMap()) {
        // CULog("Character out!");
        state = RESET;
        _camera.setCameraState(0);
    }
    _interactionController->connectGrabJoint();
    _interactionController->ungrabIfNecessary();
    _interactionController->grabCDIfNecessary(dt);
}

void GameScene::fixedUpdate(float dt) {
    if (_level == nullptr)
        return;
    _platformWorld->update(dt);
    _character->update(dt);
}

void GameScene::postUpdate(float dt) {
    if (_level == nullptr)
        return;
    _interactionController->postUpdate(dt);

    if (_interactionController->isLevelComplete()) {
        this -> defaultGoodOrBad = _interactionController -> defaultGoodOrBad;
         finishLevel();
    }
    if (_interactionController->paintPercent() > 0.01f && _interactionController->paintPercent() < 1.0f) {
        _paintMeter->setVisible(true);
        _paintMeter->setFrame((int)(_interactionController->paintPercent() * 8));
        _paintMeter->setPosition(_uinode->worldToNodeCoords(_character->getBodyPos())+Vec2(0, 50));
    } else {
        _paintMeter->setVisible(false);
    }
}

#pragma mark Helper Functions
void GameScene::constructSceneNodes(const Size &dimen) {
    Vec2 offset{(dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f};
    // _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);

    _uinode = scene2::SceneNode::alloc();
    _uinode->setContentSize(dimen);
    _uinode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);

    _pauseButtonNode = _assets->get<scene2::SceneNode>("pausebutton");
    _pauseButtonNode->removeFromParent();
    _pauseButtonNode->doLayout();
    _pauseButtonNode->setContentSize(dimen);
    _pauseButtonNode->setVisible(true);
    // pause button
//    _pauseButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pausebutton"));
//    _pauseButton->removeFromParent();
//    _pauseButton -> setPosition(Vec2::ANCHOR_TOP_RIGHT.x - 50, Vec2::ANCHOR_TOP_RIGHT.y - 50);
    _pauseButton = std::dynamic_pointer_cast<scene2::Button>(_pauseButtonNode->getChildByName("pause"));
    _pauseButton -> doLayout();
    _pauseButton->addListener([this](const std::string &name, bool down) {
        if (down) {
            _gamePaused = true;
        }
    });
    _uinode->addChild(_pauseButtonNode);

    // paint meter
    _paintMeter = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>("paintmeter"), 3, 3);
    _paintMeter->removeFromParent();
    _paintMeter->doLayout();
    _paintMeter->setVisible(false);
    _paintMeter->setScale(.5);
    _paintMeter->setFrame(0);
    std::cout << _paintMeter->getWidth() << ", " << _paintMeter->getHeight() << "\n";

    _paintMeter->setScissor(Scissor::alloc(_paintMeter->getSize() * 2));

    // level complete scene

    _levelComplete = _assets->get<scene2::SceneNode>("levelcomplete");
    _levelComplete->removeFromParent();
    _levelComplete->doLayout();
    _levelComplete->setContentSize(dimen);
    _levelComplete->setVisible(false);

    for (auto i : _levelComplete->getChildren()) {
        std::cout << "child name: " << i->getName() << std::endl;
    }
    // TODO: Trying on level complete (MAY CRASH)
    // level complete scene buttons
    auto completemenu = _levelComplete->getChildByName("completemenu");
    _levelCompleteReset = std::dynamic_pointer_cast<scene2::Button>(completemenu->getChildByName("options")->getChildByName("restart"));
    _levelCompleteReset->deactivate();
    _levelCompleteReset->addListener([this](const std::string &name, bool down) {
        if (down) {
            std::cout << "level complete reset!" << std::endl;
            this->state = RESET;
            _camera.setCameraState(0);
        }
    });

    // TODO: add this button to the level complete scene
    _levelCompleteMenuButton = std::dynamic_pointer_cast<scene2::Button>(_levelComplete->getChildByName("completemenu")->getChildByName("options")->getChildByName("menu"));
    _levelCompleteMenuButton->deactivate();
    _levelCompleteMenuButton->addListener([this](const std::string &name, bool down) {
        if (down) {
            std::cout << "level complete main menu!" << std::endl;
            this->state = QUIT;
        }
    });

    _uinode->addChild(_levelComplete);
    _uinode->addChild(_paintMeter);
    // deleted level complete related UI
    addChild(_uinode);
}

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

bool GameScene::isCharacterInMap() {
    Vec2 pos = _character->getTrackSceneNode()->getWorldPosition();
    // CULog("current body pos: %f, %f", pos.x, pos.y);
    return pos.x >= 0 && pos.x <= _worldnode->getSize().width && pos.y >= 0 && pos.y <= _worldnode->getSize().height;
}

#pragma mark Level Complete
/**
 * Steps to complete a level:
 * 1. change background (will be done in InteractionController, end of runMessageQueue())
 * 2. zoom out camera (TODO)
 * 3. pan to the last strip frame (TODO)
 * 4. wait for a sec (TODO)
 * 5. display complete scene (done)
 */
void GameScene::finishLevel() {
    _camera.levelComplete();
    if (_camera.getCameraComplete()) {
        _levelComplete->setVisible(true);
        _levelCompleteReset->activate();
        _levelCompleteMenuButton->activate();
        _complete = true;
    }
}
