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
	: cugl::Scene2(), _complete(false) {}

bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::string levelName) {
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
    _inputController->fillHand(_character->getLeftHandPosition(),
                               _character->getRightHandPosition(),
                               _character->getLHPos(),
                               _character->getRHPos());

#pragma mark Construct Camera Controller
    _camera.init(_character->getBodySceneNode(), _worldnode, 10.0f, std::dynamic_pointer_cast<OrthographicCamera>(getCamera()), _uinode, 5.0f);
    _camera.setZoom(DEFAULT_ZOOM);

    return true;
}

void GameScene::dispose(){
    _assets = nullptr;
    _interactionController = nullptr;
    _character = nullptr;
    _audioController = nullptr;
    _inputController = nullptr;
    _platformWorld = nullptr;
    _pauseButton = nullptr;
    _worldnode->removeAllChildren();
    _worldnode = nullptr;
    _levelComplete->removeAllChildren();
    _levelComplete = nullptr;
    _level = nullptr;
    this->removeAllChildren();
}

void GameScene::setActive(bool value) {
    if (isActive() != value){
        Scene2::setActive(value);
        if (value) {
            _worldnode->setVisible(true);
            _pauseButton->activate();
            _gamePaused = false;

            _inputController->fillHand(_character->getLeftHandPosition(),
                                       _character->getRightHandPosition(),
                                       _character->getLHPos(),
                                       _character->getRHPos());
        }else{
            _pauseButton->deactivate();
            _pauseButton->setDown(false);
        }
    }
}

void GameScene::reset(){
    _assets->unload<LevelLoader2>(_levelName);
    GameScene::dispose();
}

void GameScene::preUpdate(float dt){
    if (_level == nullptr) return;

    // process input
    _inputController->update(dt);
    auto character = _inputController->getCharacter();
    for (auto i = character->_touchInfo.begin(); i != character->_touchInfo.end(); i++) {
        i->worldPos = (Vec2)Scene2::screenToWorldCoords(i->position);
    }
    _inputController->process();

    _character->moveLeftHand(INPUT_SCALER * _inputController->getLeftHandMovement());
    _character->moveRightHand(INPUT_SCALER * _inputController->getrightHandMovement());
    _inputController->fillHand(_character->getLeftHandPosition(),
                                _character->getRightHandPosition(),
                                _character->getLHPos(),
                                _character->getRHPos());

    // update camera
    _camera.update(dt);

    // update interaction controller
    _interactionController->preUpdate(dt);
}

void GameScene::fixedUpdate(float dt){
    if (_level == nullptr) return;
    _platformWorld->update(dt);
}

void GameScene::postUpdate(float dt){
    if (_level == nullptr) return;
    _interactionController->postUpdate(dt);
    if (_interactionController->isLevelComplete()) {
        _complete = true;
        _levelComplete->setVisible(true);
    }
}


// ================================= private helper functions =================================
void GameScene::constructSceneNodes(const Size &dimen){
    Vec2 offset{ (dimen.width - SCENE_WIDTH) / 2.0f,(dimen.height - SCENE_HEIGHT) / 2.0f };
    // _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);

    _uinode = scene2::SceneNode::alloc();
    _uinode->setContentSize(dimen);
    _uinode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);

    // pause button
    _pauseButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("pausebutton"));
    _pauseButton -> removeFromParent();
    _pauseButton->addListener([this](const std::string& name, bool down) {
        if (down) {
            _gamePaused = true;
        }
    });
    _uinode->addChild(_pauseButton);

    // level complete scene
    _levelComplete = _assets->get<scene2::SceneNode>("levelcomplete");
    _levelComplete -> removeFromParent();
    _levelComplete -> doLayout();
    _levelComplete -> setContentSize(dimen);
    _levelComplete -> setVisible(false);
    _uinode -> addChild(_levelComplete);

    //deleted level complete related UI
    addChild(_uinode);
}


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
