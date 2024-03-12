#include "SettingScene.h"

void SettingScene::dispose() {
    std::cout << "Setting dispose" << std::endl;
}


void SettingScene::setActive(bool value) {
    std::cout << "Setting set active" << std::endl;
}

void SettingScene::update(float timestep) {
    std::cout << "Setting is updating!" << std::endl;
}
