//
//  PauseScene.cpp
//  DustyPaints
//
//  Created by Emily on 3/15/24.
//

#include "PauseScene.h"
using namespace cugl;

bool PauseScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<physics2::net::NetEventController>& network){
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    _network = network;
    // Start up the input handler
    _assets = assets;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("resume");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("resume_back"));
    _reset = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("resume_reset"));

    
    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            state = BACK;
            _network->pushOutEvent(PauseEvent::allocPauseEvent(Vec2(DEFAULT_WIDTH/2,DEFAULT_HEIGHT/2), false));
        }
    });
    
    _reset->addListener([this](const std::string& name, bool down) {
        if (down) {
            //state = BACK;
        }
    });

     
    // Create the server configuration
    addChild(scene);
    setActive(false);
    return true;

}

void PauseScene::dispose() {
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
void PauseScene::fixedUpdate() {
    if(_network->isInAvailable()){
        auto e = _network->popInEvent();
        if(auto pauseEvent = std::dynamic_pointer_cast<PauseEvent>(e)){
            CULog("Pause Event RECIEVED");
            state = BACK;
        }
    }
}

void PauseScene::update(float timestep) {
}
