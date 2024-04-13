#include "LevelLoadScene.h"


#define SCENE_SIZE  1024

bool LevelLoadScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    if (dimen.width > dimen.height) {
        dimen *= SCENE_SIZE/dimen.width;
    } else {
        dimen *= SCENE_SIZE/dimen.height;
    }
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }

    if (assets == nullptr) {
        return false;
    } 

    _state = LOADING;

    _assets = assets;

    // currently no progress bar or button simple black screen

    Application::get()->setClearColor(Color4(192,192,192,255));


    return true;
}

bool LevelLoadScene::loadFileAsync(const std::string &file, const std::string &key) {
    // TODO: 
    return true;
}

void LevelLoadScene::dispose() {
    _assets = nullptr;
    _state = LOADING;
}
