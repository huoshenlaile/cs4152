#include "Wall.h"
bool Wall::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    std::shared_ptr<JsonValue> properties = json -> get("properties");
    // find the Publication property
    for (int i = 0; i < properties->size(); i++){
        auto prop = properties->get(i);
        if (prop->getString("propertytype") == "SubMessage" && prop->get("value")->getString("Action") == "show"){
            auto pub = prop->get("value");
            show_subscription_head = pub->getString("Head");
        }else if(prop->getString("propertytype") == "SubMessage" && prop->get("value")->getString("Action") == "hide"){
            auto pub = prop->get("value");
            hide_subscription_head = pub->getString("Head");
        }
    }
    activated = true;
    _selfObstacle->setSensor(false);
    actions[show_subscription_head] = ([=](ActionParams params){
        if (!_selfTexture->isVisible()){
            _selfTexture->setVisible(true);
            _selfObstacle->setSensor(false);
            
            PublishedMessage m = PublishedMessage();
            m.Head = getName() + " shown";
            m.enable = true;
            return m;
        }
        return PublishedMessage();
    });
    
    actions[hide_subscription_head] = ([=](ActionParams params){
        if (_selfTexture->isVisible()){
            
            _selfTexture->setVisible(false);
            _selfObstacle->setSensor(true);
            
            PublishedMessage m = PublishedMessage();
            m.Head = getName() + " hidden";
            m.enable = true;
            return m;
        }
        return PublishedMessage();
    });
    return true;
}


