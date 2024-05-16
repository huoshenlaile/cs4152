#include "BouncyWall.h"
bool BouncyWall::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    std::shared_ptr<JsonValue> properties = json->get("properties");
    for (int i = 0; i < properties->size(); i++){
        if(properties->get(i)->getString("name") == "jumpscale"){
            std::cout << "jumpscale found" << std::endl;
            auto js = properties->get(i)->get("value")->asFloat();
            jumpscale = js;
        }
    }
    activated = true;
    _selfObstacle->setSensor(false);
    OnBeginContactEnabled = true;
    return true;
}

PublishedMessage BouncyWall::onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){
    //if the character body collides with the bouncy wall from above without hand touching
    if (isCharacter && character->getLHObstacle() != other && character->getRHObstacle() != other){
        if (activated){
            double platformAngle = this->getObstacle()->getAngle();
            cugl::Vec2 normal (0.0,1.0);
            normal.x = normal.x * std::cos(platformAngle) - normal.y * std::sin(platformAngle);
            normal.y = normal.x * std::sin(platformAngle) + normal.y * std::cos(platformAngle);
            float speed = this->jumpscale;
            
            float dot = (character->getBodyObstacle()->getPosition() - this->getObstacle()->getPosition()).dot(normal);
            if (dot > -0.1){
                character->getBodyObstacle()->setLinearVelocity(normal * speed);
            }
        }
    }
    return PublishedMessage();
}
