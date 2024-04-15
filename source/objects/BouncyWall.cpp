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
    float speed = 150;
    //if the character body collides with the bouncy wall
    std::cout << (other != character->getLHObstacle() && other != character->getLHObstacle());
    if (isCharacter && character->getBodyObstacle()->getY() > this->getObstacle()->getY() && character->getLHObstacle() != other && character->getRHObstacle() != other){
        // if the sensor is activated
        if (activated){
            double magnitude = std::sqrt(character->getBodyObstacle()->getLinearVelocity().x * character->getBodyObstacle()->getLinearVelocity().x + character->getBodyObstacle()->getLinearVelocity().y * character->getBodyObstacle()->getLinearVelocity().y);
//            std::cout << "contacted bouncy platform with body" << std::endl;
            if (magnitude == 0 ){
                character->getBodyObstacle()->setLinearVelocity({0.0, speed});
            } else {
                float x_vel = -character->getBodyObstacle()->getLinearVelocity().x / magnitude * speed;
                float y_vel = -character->getBodyObstacle()->getLinearVelocity().y / magnitude * speed;
                character->getBodyObstacle()->setLinearVelocity(x_vel, y_vel);
            }
        }
    }
    return PublishedMessage();
}
