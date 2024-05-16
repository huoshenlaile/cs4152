#include "SettingScene.h"

#define SCENE_HEIGHT  720
bool SettingScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_HEIGHT / dimen.height;
	if (assets == nullptr) {
		CULog("Failed init Setting Scene");
		return false;
	}
	else if (!Scene2::init(dimen)) {
		CULog("Failed init Setting Scene");
		return false;
	}

	// Start up the input handler
	_assets = assets;

	// Acquire the scene built by the asset loader and resize it the scene
	std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("setting");
    
	scene->setContentSize(dimen);
	scene->doLayout(); // Repositions the HUD
    
    _honors_mode_button = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("setting_toggle"));
    
    _honors_mode_button->addListener([this](const std::string& name, bool down) {
        if (down) {
            honors_mode = !honors_mode;
        }
    });
    
	_backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("sAttings_back"));
	_backout->addListener([this](const std::string& name, bool down) {
		if (down) {
            if (honors_mode){
                state = HONORSMODE;
            } else {
                state = NONHONORSMODE;
            }
		}
    });

	// Create the server configuration
	addChild(scene);
	setActive(false);
	return true;
}

void SettingScene::dispose() {
	std::cout << "Setting dispose" << std::endl;
	if (_active) {
		removeAllChildren();
		_active = false;
	}
}

void SettingScene::setActive(bool value) {
	std::cout << "Setting set active" << std::endl;
	if (isActive() != value) {
		Scene2::setActive(value);
		if (value) {
			_backout->activate();
            _honors_mode_button->activate();
			state = INSCENE;
		}
		else {
			_backout->deactivate();
			_backout->setDown(false);
            _honors_mode_button->deactivate();
            _honors_mode_button->setDown(false);
		}
	}
}

void SettingScene::update(float timestep) {
	std::cout << "Setting is updating!" << honors_mode << std::endl;
    if (honors_mode){
        auto _honorsModeButtonImageNode = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("setting_toggle")->getChildByName("up")->getChildByName("toggle_up"));
        _honorsModeButtonImageNode->setTexture(_assets->get<Texture>("toggle_up"));
        Size contentDimen = _assets->get<scene2::SceneNode>("setting_toggle")->getChildByName("up")->getChildByName("toggle_up") -> getContentSize();
        _honorsModeButtonImageNode->setContentSize(contentDimen.width, contentDimen.height);
    } else {
        auto _honorsModeButtonImageNode = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>("setting_toggle")->getChildByName("up")->getChildByName("toggle_up"));
        _honorsModeButtonImageNode->setTexture(_assets->get<Texture>("toggle_down"));
        Size contentDimen = _assets->get<scene2::SceneNode>("setting_toggle")->getChildByName("up")->getChildByName("toggle_up") -> getContentSize();
        _honorsModeButtonImageNode->setContentSize(contentDimen.width, contentDimen.height);
    }
}
