//
//  PauseScene.cpp
//  DustyPaints
//
//  Created by Emily on 3/15/24.
//

#include "PauseScene.h"
using namespace cugl;

bool PauseScene::init(
    const std::shared_ptr<cugl::AssetManager> &assets,
    const std::shared_ptr<physics2::net::NetEventController> &network) {
  Size dimen = Application::get()->getDisplaySize();
  dimen *= SCENE_HEIGHT / dimen.height;
  if (assets == nullptr) {
    return false;
  } else if (!Scene2::init(dimen)) {
    return false;
  }

  _network = network;
  // Start up the input handler
  _assets = assets;

  // Acquire the scene built by the asset loader and resize it the scene
  std::shared_ptr<scene2::SceneNode> scene =
      _assets->get<scene2::SceneNode>("pause");
  scene->setContentSize(dimen);
  scene->doLayout(); // Repositions the HUD
  _backout = std::dynamic_pointer_cast<scene2::Button>(
      _assets->get<scene2::SceneNode>("pause_pausemenu_options_resume"));
  _reset = std::dynamic_pointer_cast<scene2::Button>(
      _assets->get<scene2::SceneNode>("pause_pausemenu_options_restart"));
//    _setting = std::dynamic_pointer_cast<scene2::Button>(
//        _assets->get<scene2::SceneNode>("pause_pausemenu_options_settingicon"));
  _menu = std::dynamic_pointer_cast<scene2::Button>(
        _assets->get<scene2::SceneNode>("pause_pausemenu_options_menu"));

  _backout->addListener([this](const std::string &name, bool down) {
    if (down) {
      state = BACK;
    }
  });

  _reset->addListener([this](const std::string &name, bool down) {
    if (down) {
       state = RESET;
    }
  });
    
//    _setting->addListener([this](const std::string &name, bool down) {
//        if (down) {
//           state = SETTING;
//        }
//      });

  _menu->addListener([this](const std::string &name, bool down) {
      if (down) {
         state = MENU;
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
//    CULog("Making active %d", value);
    Scene2::setActive(value);
    if (value) {
      _backout->activate();
      _reset->activate();
//        _setting->activate();
      _menu->activate();
      state = INSCENE;
    } else {
      _backout->deactivate();
      _backout->setDown(false);
      _reset->deactivate();
      _reset->setDown(false);
//        _setting->deactivate();
//        _setting->setDown(false);
      _menu->deactivate();
      _menu->setDown(false);
    }
  }
}

void PauseScene::fixedUpdate() {
//  if (_network->isInAvailable()) {
//    auto e = _network->popInEvent();
//    if (auto pauseEvent = std::dynamic_pointer_cast<PauseEvent>(e)) {
//      CULog("Pause Event RECIEVED");
//        if(!pauseEvent->isPause()){
//            state = BACK;
//        }
//    }
//  }
}

void PauseScene::update(float timestep) {}
