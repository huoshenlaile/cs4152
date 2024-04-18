#include "BouncyWall.h"
bool BouncyWall::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    activated = true;
    _selfObstacle->setSensor(false);
    OnBeginContactEnabled = true;
    return true;
}

PublishedMessage BouncyWall::onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){
    //if the character body collides with the bouncy wall from above without hand touching
    if (isCharacter && character->getBodyObstacle()->getY() > this->getObstacle()->getY() && character->getLHObstacle() != other && character->getRHObstacle() != other){
        if (activated){
            float speed = 150;
            cugl::Vec2 normal (0.0,1.0);
            double platformAngle = this->getObstacle()->getAngle();
            normal.x = normal.x * std::cos(platformAngle) - normal.y * std::sin(platformAngle);
            normal.y = normal.x * std::sin(platformAngle) + normal.y * std::cos(platformAngle);
            std::cout << platformAngle << std::endl;
            character->getBodyObstacle()->setLinearVelocity(normal * speed);
        }
    }
    return PublishedMessage();
}
