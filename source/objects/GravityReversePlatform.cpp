//
//  GravityReversePlatform.cpp
//  DustyPaints
//
//  Created by Haoxuan Zou on 4/14/24.
//

#include "GravityReversePlatform.h"

bool GravityReversePlatform::init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect  bounds){
    Interactable::init(json, scale, bounds);
    OnBeginContactEnabled = true;
    activated = true;
    _name = "Gravity";
    std::cout << "gravity wall init" << std::endl;
    return true;
}

PublishedMessage GravityReversePlatform::onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){

//    std::cout << "gravity pf update" << std::endl;
    if (isCharacter && character != nullptr) {
//        std::cout << "reversing gravity" << std::endl;
        character -> getBodyObstacle() -> setGravityScale(-1);
        character -> getLHObstacle() -> setGravityScale(-1);
        character -> getRHObstacle() -> setGravityScale(-1);
    }
    return PublishedMessage();
}
