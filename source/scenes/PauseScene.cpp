//
//  PauseScene.cpp
//  DustyPaints
//
//  Created by Emily on 3/15/24.
//

#include "PauseScene.h"

#define SCENE_HEIGHT  720
bool PauseScene::init(const std::shared_ptr<cugl::AssetManager>& assets){
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("resume");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("resume_back"));
    
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

void PauseScene::dispose() {
    std::cout << "Pause dispose" << std::endl;
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}


void PauseScene::setActive(bool value) {
    if (isActive() != value) {
        CULog("Making active %d", value);
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

void PauseScene::update(float timestep) {
}

