#include <stdio.h>
#include <iostream>
#include "LevelSelectScene.h"

void LevelSelectScene::dispose() {
	std::cout << "LevelSelect dispose" << std::endl;
}

void LevelSelectScene::setActive(bool value) {
	std::cout << "LevelSelect set active" << std::endl;
}