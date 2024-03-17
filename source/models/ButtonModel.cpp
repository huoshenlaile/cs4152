//
//  ButtonModel.cpp
//  DustyPaints
//
//  Created by Haoxuan Zou on 3/11/24.
//

#include <stdio.h>
#include "ButtonModel.h"

ButtonModel::ButtonModel(const Vec2 pos, const Size size, const Vec2& publications, const std::string texture_name, const float gamescale) {
    
    // Create obstacle
    Vec2 buttonPos = pos;
    Size buttonSize(size.width / gamescale,
                    size.height / gamescale);
    _button = physics2::BoxObstacle::alloc(buttonPos, buttonSize);
    
    // Set the physics attributes
    _button->setBodyType(b2_staticBody);
    _button->setDensity(0.0f);
    _button->setFriction(0.0f);
    _button->setRestitution(0.0f);
    _button->setSensor(true);
    
}
