#include <stdio.h>
#include "LevelSelectScene.h"

void LevelSelectScene::dispose() {
    std::cout << "LevelSelect dispose" << std::endl;
}


void LevelSelectScene::setActive(bool value) {
    std::cout << "LevelSelect set active" << std::endl;
}


void LevelSelectScene::update(float timestep) {
    std::cout << "LevelSelect is updating" << std::endl;
}
