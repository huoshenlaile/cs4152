//
//  EndingScene.cpp
//  DustyPaints
//
//  Created by Haoxuan Zou on 5/17/24.
//

#include "EndingScene.h"

#define SCENE_HEIGHT  720
bool EndingScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT / dimen.height;
    if (assets == nullptr) {
        CULog("Failed init Ending Scene");
        return false;
    }
    else if (!Scene2::init(dimen)) {
        CULog("Failed init Ending Scene");
        return false;
    }

    // Start up the input handler
    _assets = assets;

    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("endingscreen");
    
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD

    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("endingscreen_back"));
    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            std::cout << "ending back menu hit" << std::endl;
            state = MENU;
        }
    });
    
    // Create the server configuration
    addChild(scene);
    setActive(false);
    
    std::cout << "ending scene is initialized!" << std::endl;
    return true;
}

void EndingScene::dispose() {
    std::cout << "Ending dispose" << std::endl;
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}

void EndingScene::setActive(bool value) {
    std::cout << "Ending setActive called" << std::endl;
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _backout->activate();
            state = INSCENE;
        }
        else {
            _backout->deactivate();
            _backout->setDown(false);
        }
    }
}

void EndingScene::update(float timestep) {
    
}
