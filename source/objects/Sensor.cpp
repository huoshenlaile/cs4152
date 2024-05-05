#include "Sensor.h"
/*

    SENSOR CLASS IS UPDATED on Monday afternoon, it will break some behavior. please check the google doc for usage. 

*/
bool Sensor::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    _selfTexture->setColor(Color4(0,0,0,0));
    std::shared_ptr<JsonValue> properties = json -> get("properties");
    // find the Publication property
    for (int i = 0; i < properties->size(); i++){
        if (properties->get(i)->getString("propertytype") == "PubMessage"){
            auto pub = properties->get(i)->get("value");
            if (pub->getBool("Enabled", true) && (pub->getString("Trigger") != "" )){
                triggers.push_back(pub->getString("Trigger"));
                message_heads.push_back(pub->getString("Head"));
                message_bodies.push_back(pub->getString("Body"));
            }
        }
    }
    activated = true;
    _selfObstacle->setSensor(true);
    OnBeginContactEnabled = true;
    return true;
}

PublishedMessage Sensor::onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){
    if (activated){
        auto other_name = other->getName();
        for (int i = 0; i < triggers.size(); i++){
            if (other_name == triggers[i]){
                auto a = PublishedMessage();
                a.Head = message_heads[i];
                a.Body = message_bodies[i];
                return a;
            }
        }
    }
    return PublishedMessage();
}
