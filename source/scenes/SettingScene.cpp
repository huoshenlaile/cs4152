#include "SettingScene.h"

#define SCENE_HEIGHT  720
bool SettingScene::init(const std::shared_ptr<cugl::AssetManager>& assets){
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        CULog("Failed init Setting Scene");
        return false;
    } else if (!Scene2::init(dimen)) {
        CULog("Failed init Setting Scene");
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("settings");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("settings_back"));
    
    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            state = BACK;
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
            state = INSCENE;
        } else {
            _backout->deactivate();
            _backout->setDown(false);
        }
    }


}

void SettingScene::update(float timestep) {
    std::cout << "Setting is updating!" << std::endl;
}
