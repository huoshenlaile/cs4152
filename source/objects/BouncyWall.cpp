#include "BouncyWall.h"
bool BouncyWall::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    activated = true;
    _selfObstacle->setSensor(true);
    OnBeginContactEnabled = true;
    return true;
}

PublishedMessage BouncyWall::onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){
    double speed = 100;
    if (isCharacter){
        // if the sensor is activated
        if (activated){
            double magnitude = std::sqrt(character->getBodyObstacle()->getLinearVelocity().x * character->getBodyObstacle()->getLinearVelocity().x + character->getBodyObstacle()->getLinearVelocity().y * character->getBodyObstacle()->getLinearVelocity().y);
            std::cout << "contacted bouncy platform with body" << std::endl;
            if (magnitude == 0 ){
                character->getBodyObstacle()->setLinearVelocity({0.0,100.0});
            } else {
                float x_vel = -character->getBodyObstacle()->getLinearVelocity().x / magnitude * speed;
                float y_vel = -character->getBodyObstacle()->getLinearVelocity().y / magnitude * speed;
                character->getBodyObstacle()->setLinearVelocity(x_vel, y_vel);
            }
        }
    }
    return PublishedMessage();
}
