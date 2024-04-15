#include "Sensor.h"
bool Sensor::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    std::shared_ptr<JsonValue> properties = json -> get("properties");
    // find the Publication property
    for (int i = 0; i < properties->size(); i++){
        if (properties->get(i)->getString("propertytype") == "PubMessage"){
            auto pub = properties->get(i)->get("value");
            message_head = pub->getString("Head");
            pub_message_body = pub->getString("Body");
            break;
        }
    }
    activated = true;
    _selfObstacle->setSensor(true);
    OnBeginContactEnabled = true;
    return true;
}

PublishedMessage Sensor::onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){
    CULog("Sensor of [%s] tested", message_head.c_str());
    if (isCharacter){
        // if the sensor is activated
        if (activated){
            if (other->getName() == "body192"){
                CULog("Sensor of [%s] activated", message_head.c_str());
                auto a = PublishedMessage();
                a.Head = message_head;
                a.Body = pub_message_body;
                return a;
            }
        }
    }
    return PublishedMessage();
}
