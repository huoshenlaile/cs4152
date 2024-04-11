#include "LevelLoadScene.h"


bool LevelLoadScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    
    if (assets == nullptr) {
        return false;
    } 

    _assets = assets;
    return true;
}




void LevelLoadScene::dispose() {
    _assets = nullptr;
}