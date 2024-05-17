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
    _skipCameraSpan = false;
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width : dimen.height / rect.size.height;

#pragma mark fetch from level loader
    _level = _assets->get<LevelLoader2>(levelName);
    _platformWorld = _level->getWorld();

    _worldnode = _level->getWorldNode();
    addChild(_worldnode);
    _character = _level->getCharacter();

#pragma mark Construct UI elements
    constructSceneNodes(dimen);

#pragma mark Construct Input Controller
    _inputController = std::make_shared<InputController>();
    _inputController->init(rect);
    _inputController->fillHand(_character->getLeftHandPosition(), _character->getRightHandPosition(), _character->getLHPos(), _character->getRHPos());

#pragma mark Construct Camera Controller
    _camera = std::make_shared<CameraController>();
    _camera->setCamera(levelName, computeActiveSize());
    _camera->init(_character->getTrackSceneNode(), _worldnode, 10.0f, std::dynamic_pointer_cast<OrthographicCamera>(getCamera()), _uinode, 5.0f, _camera->getMode(), _skipCameraSpan);
    _camera->setZoom(_camera->getLevelCompleteZoom());

#pragma mark Construct Interaction Controller
    _interactionController = InteractionController2::alloc(_level, _inputController, _camera, _skipCameraSpan);
    _interactionController->activateController();

#pragma mark Audio Controller
    _audioController = std::make_shared<AudioController>();
    _audioController->init(_assets);
    _audioController->play("touch1", "touch1");
    _interactionController->setAudioController(_audioController);

    PublishedMessage pm;
    pm.Head = "UI Start";
    _interactionController->pushToMessageQueue(pm);

    CULog("Actual input scaler: %f", acutal_input_scaler);

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
    _mapButton = nullptr;
    _worldnode->removeAllChildren();
    _worldnode = nullptr;
    _levelCompleteGood = nullptr;
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
            _mapButton->activate();
            _gamePaused = false;
            _inputController->fillHand(_character->getLeftHandPosition(), _character->getRightHandPosition(), _character->getLHPos(), _character->getRHPos());
        } else {
            _pauseButton->deactivate();
            _pauseButton->setDown(false);
            _mapButton->deactivate();
            _mapButton->setDown(false);
        }
    }
}

void GameScene::reset() {
    _assets->unload<LevelLoader2>(_levelName);
    _camera->setCameraState(3);
    _camera->setCameraSkip(true);
    GameScene::dispose();
}

#pragma mark preUpdate
void GameScene::preUpdate(float dt) {
    //    CULog("position %f,%f!!!", _character->getTrackSceneNode()->getWorldPosition().x,_character->getTrackSceneNode()->getWorldPosition().y);
    if (_level == nullptr)
        return;
    // process input
    if(_skipCameraSpan && _camera->getState() < 2) {
        _camera->setState(3);
        _camera->setDisplayed(true);
        _inputController->setStarted(true);
        _skipCameraSpan = false;
    }

    // clear inputController whenever the map button is pressed
    
    
    if ((_camera->getDisplayed() || !_inputController->getStarted()) && !_camera->getLevelComplete()){
        // input enabled
         _inputController->fillHand(_character->getLeftHandPosition(), _character->getRightHandPosition(), _character->getLHPos(), _character->getRHPos());
        _inputController->update(dt);
        auto character = _inputController->getCharacter();
        for (auto i = character->_touchInfo.begin(); i != character->_touchInfo.end(); i++) {
            i->worldPos = (Vec2)Scene2::screenToWorldCoords(i->position);
        }
        _inputController->process();
    }else{
        // input disabled
        _inputController->resetInput();
    }

    
    _character->moveLeftHand(acutal_input_scaler * _inputController->getLeftHandMovement(), _interactionController->leftHandReverse);
    _character->moveRightHand(acutal_input_scaler * _inputController->getrightHandMovement(), _interactionController->rightHandReverse);



    // update camera
    if (_inputController->getStarted() || !_camera->getInitialUpdate()) {
        _camera->update(dt);
        _camera->setInitialUpdate(true);
    }
    // make one time publish with a boolean flag
    if (_camera->getState() == 1) {
        PublishedMessage pm;
        pm.Head = "Erase Tap Continue Button";
        _interactionController->pushToMessageQueue(pm);
    }

    // update interaction controller
    _interactionController->updateHandsHeldInfo(_inputController->isLHAssigned(), _inputController->isRHAssigned());
    _interactionController->preUpdate(dt);

    if (!isCharacterInMap()) {
        // CULog("Character out!");
        state = RESET;
        _camera->setCameraState(3);
    }
    // NOTE: This is moved to InteractionController.
    //    _interactionController->connectGrabJoint();
    //    _interactionController->ungrabIfNecessary();
    //    _interactionController->grabCDIfNecessary(dt);
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
        this->defaultGoodOrBad = _interactionController->defaultGoodOrBad;
        finishLevel();
    }
    if (_interactionController->paintPercent() > 0.01f && _interactionController->paintPercent() < 1.0f) {
        _paintMeter->setVisible(true);
        _paintMeter->setFrame((int)(_interactionController->paintPercent() * 8));
        _paintMeter->setPosition(_uinode->worldToNodeCoords(_character->getBodyPos()) + Vec2(0, 50));
    } else {
        _paintMeter->setVisible(false);
    }
    
    if (_level->getExit()->last_color_collected != _last_color_collected){
        _last_color_collected = _level->getExit()->last_color_collected;
        if (_colorNodesImgs.count(_last_color_collected)>0){
            auto paintnode = _colorNodesImgs[_last_color_collected];
            std::cout << _last_color_collected+"_check";
            
            auto texture = _assets->get<Texture>(_last_color_collected+"_check");
            paintnode->setTexture(texture);
        }
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

    auto exit = _level->getExit();
    float color_offset = 30.0f;
    for (std::string color : exit->getColorReqs()){
        _colorNodesImgs[color] = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(color+"_blank"));
        _colorNodesImgs[color]->removeFromParent();
        _colorNodesImgs[color]->setAnchor(Vec2::ANCHOR_TOP_LEFT);
        _colorNodesImgs[color]->setPosition(color_offset,_uinode->getHeight()-30.0f);
        color_offset += 60.0f;
        _colorNodesImgs[color]->setVisible(true);
        _uinode->addChild(_colorNodesImgs[color]);
    }
    _pauseButtonNode = _assets->get<scene2::SceneNode>("pausebutton");
    _pauseButtonNode->removeFromParent();
    _pauseButtonNode->doLayout();
    _pauseButtonNode->setContentSize(dimen);
    _pauseButtonNode->setVisible(true);
    // pause button
    _pauseButton = std::dynamic_pointer_cast<scene2::Button>(_pauseButtonNode->getChildByName("pause"));
    _pauseButton->doLayout();
    _pauseButton->addListener([this](const std::string &name, bool down) {
        if (down) {
            _gamePaused = true;
        }
    });
    _mapButton = std::dynamic_pointer_cast<scene2::Button>(_pauseButtonNode->getChildByName("map"));
    _mapButton->doLayout();
    _mapButton->addListener([this](const std::string &name, bool down) {
        if (down) {
            //            CULog("Map Button Pressed!");
            // TODO: add map function
            _camera->setReplay(true);
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

    _paintMeter->setScissor(Scissor::alloc(_paintMeter->getSize() * 2));

    // level complete scenes
    // good scene
    _levelCompleteGood = _assets->get<scene2::SceneNode>("levelcomplete");
    _levelCompleteGood->removeFromParent();
    _levelCompleteGood->doLayout();
    _levelCompleteGood->setContentSize(dimen);
    _levelCompleteGood->setVisible(false);
    // level complete scene buttons
    auto completemenu = _levelCompleteGood->getChildByName("completemenu");
    _levelCompleteGoodReset = std::dynamic_pointer_cast<scene2::Button>(completemenu->getChildByName("options")->getChildByName("restart"));
    _levelCompleteGoodReset->deactivate();
    _levelCompleteGoodReset->setDown(false);
    _levelCompleteGoodReset->addListener([this](const std::string &name, bool down) {
        if (down) {
            std::cout << "level complete reset (GOOD) triggered!" << std::endl;
            this->state = RESET;
            _camera->setCameraState(0);
        }
    });
    _levelCompleteGoodMenu = std::dynamic_pointer_cast<scene2::Button>(completemenu->getChildByName("options")->getChildByName("menu"));
    _levelCompleteGoodMenu->deactivate();
    _levelCompleteGoodMenu->setDown(false);
    _levelCompleteGoodMenu->addListener([this](const std::string &name, bool down) {
        if (down) {
            std::cout << "level complete main menu (GOOD) triggered!" << std::endl;
            this->state = QUIT;
        }
    });
    _levelCompleteGoodNext = std::dynamic_pointer_cast<scene2::Button>(completemenu->getChildByName("options")->getChildByName("next"));
    _levelCompleteGoodNext->deactivate();
    _levelCompleteGoodNext->setDown(false);
    _levelCompleteGoodNext->addListener([this](const std::string &name, bool down) {
        if (down) {
            std::cout << "level complete next (GOOD) triggered!" << std::endl;
            this->state = NEXTLEVEL;
        }
    });
    _uinode->addChild(_levelCompleteGood);
    // bad scene
    _levelCompleteBad = _assets->get<scene2::SceneNode>("levelcomplete_bad");
    _levelCompleteBad->removeFromParent();
    _levelCompleteBad->doLayout();
    _levelCompleteBad->setContentSize(dimen);
    _levelCompleteBad->setVisible(false);
    // level complete scene buttons
    completemenu = _levelCompleteBad->getChildByName("badcompletemenu");
    _levelCompleteBadReset = std::dynamic_pointer_cast<scene2::Button>(completemenu->getChildByName("options")->getChildByName("restart"));
    _levelCompleteBadReset->deactivate();
    _levelCompleteBadReset->setDown(false);
    _levelCompleteBadReset->addListener([this](const std::string &name, bool down) {
        if (down) {
            std::cout << "level complete reset (BAD) triggered!" << std::endl;
            this->state = RESET;
            _camera->setCameraState(0);
        }
    });
    _levelCompleteBadMenu = std::dynamic_pointer_cast<scene2::Button>(completemenu->getChildByName("options")->getChildByName("menu"));
    _levelCompleteBadMenu->deactivate();
    _levelCompleteBadMenu->setDown(false);
    _levelCompleteBadMenu->addListener([this](const std::string &name, bool down) {
        if (down) {
            std::cout << "level complete main menu (BAD) triggered!" << std::endl;
            this->state = QUIT;
        }
    });
    _levelCompleteBadNext = std::dynamic_pointer_cast<scene2::Button>(completemenu->getChildByName("options")->getChildByName("next"));
    _levelCompleteBadNext->deactivate();
    _levelCompleteBadNext->setDown(false);
    _levelCompleteBadNext->addListener([this](const std::string &name, bool down) {
        if (down) {
            std::cout << "level complete next (BAD) triggered!" << std::endl;
            this->state = NEXTLEVEL;
        }
    });
    _uinode->addChild(_levelCompleteBad);
    
    _levelCompleteHonorsBad = _assets->get<scene2::SceneNode>("levelcomplete_bad_honors");
    _levelCompleteHonorsBad->removeFromParent();
    _levelCompleteHonorsBad->doLayout();
    _levelCompleteHonorsBad->setContentSize(dimen);
    _levelCompleteHonorsBad->setVisible(false);
    completemenu = _levelCompleteHonorsBad->getChildByName("badcompletemenu");
    _levelCompleteHonorsBadReset = std::dynamic_pointer_cast<scene2::Button>(completemenu->getChildByName("options")->getChildByName("restart"));
    _levelCompleteHonorsBadReset->deactivate();
    _levelCompleteHonorsBadReset->setDown(false);
    _levelCompleteHonorsBadReset->addListener([this](const std::string &name, bool down) {
        if (down) {
            std::cout << "level complete reset (HONORS BAD) triggered!" << std::endl;
            this->state = RESET;
            _camera->setCameraState(0);
        }
    });
    _levelCompleteHonorsBadMenu = std::dynamic_pointer_cast<scene2::Button>(completemenu->getChildByName("options")->getChildByName("menu"));
    _levelCompleteHonorsBadMenu->deactivate();
    _levelCompleteHonorsBadMenu->setDown(false);
    _levelCompleteHonorsBadMenu->addListener([this](const std::string &name, bool down) {
        if (down) {
            std::cout << "level complete main menu (HONORS BAD) triggered!" << std::endl;
            this->state = QUIT;
        }
    });
    _uinode->addChild(_levelCompleteHonorsBad);

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
    return pos.x >= 0 - 400 && pos.x <= _worldnode->getSize().width + 500 && pos.y >= -500;
}

#pragma mark Level Complete
/**
 * Steps to complete a level:
 * 1. change background
 * 2. zoom out camera
 * 3. pan to the last strip frame
 * 4. wait for a sec
 * 5. display complete scene
 */
void GameScene::finishLevel() {
    _camera->levelComplete();
    // TODO: Emily publish end UI
    PublishedMessage pm;
    if (this->defaultGoodOrBad == 0) {
        pm.Head = "UI End Good";
        _interactionController->pushToMessageQueue(pm);
    } else {
        pm.Head = "UI End Bad";
        _interactionController->pushToMessageQueue(pm);
    }

    if (_camera->getCameraComplete()) {
        if (this->defaultGoodOrBad == 0) {
            _levelCompleteGood->setVisible(true);
            _levelCompleteGoodReset->activate();
            _levelCompleteGoodMenu->activate();
            _levelCompleteGoodNext->activate();
            std ::cout << "activating the good menu button" << std::endl;
        } else if (this->defaultGoodOrBad == 1 and !_honors) {
            _levelCompleteBad->setVisible(true);
            _levelCompleteBadReset->activate();
            _levelCompleteBadMenu->activate();
            _levelCompleteBadNext->activate();
        } else if (this->defaultGoodOrBad == 1 and _honors){
            _levelCompleteHonorsBad->setVisible(true);
            _levelCompleteHonorsBadReset->activate();
            _levelCompleteHonorsBadMenu->activate();
        } else {
            CULogError("ERROR: finishing level in an invalid state (idk if this is a good or bad ending)");
        }
        _complete = true;
        this->state = UPDATING;
    }
}
