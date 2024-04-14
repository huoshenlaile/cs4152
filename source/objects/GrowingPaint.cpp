#include "GrowingPaint.h"
bool GrowingPaint::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    std::shared_ptr<JsonValue> properties = json -> get("properties");
    // find the Publication property
    for (int i = 0; i < properties->size(); i++){
        if (properties->get(i)->getString("propertytype") == "SubMessage"){
            auto pub = properties->get(i)->get("value");
            sub_message_head = pub->getString("Head");
        }else if(properties->get(i)->getString("propertytype") == "PubMessage"){
            auto p = properties->get(i)->get("value");
            pub_message_head = p->getString("Head");
            color = p->getString("Body");
        }
    }
    activated = true;
    _selfObstacle->setSensor(true);
    OnBeginContactEnabled = true;
    is_out = false;
    
    return true;
}

bool GrowingPaint::linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode> &sceneNode, float scale){
    _world = physicsWorld;
    _scene = sceneNode;
    
    _world->addObstacle(_selfObstacle);
    _selfTexture->setPosition(_selfObstacle->getPosition() * scale);
    // add message
    actions[sub_message_head] = ([=](ActionParams params){
        if (is_out){
            return PublishedMessage();
        }
        _scene->addChild(_selfTexture);
        is_out = true;
        return PublishedMessage();
    });
    return true;
}

PublishedMessage GrowingPaint::onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){
    if (isCharacter){
        // if the sensor is activated
        if (activated && is_out){
            if (other->getName() == "body192"){
                auto a = PublishedMessage();
                a.Head = pub_message_head;
                return a;
            }
        }
    }
    return PublishedMessage();
}
