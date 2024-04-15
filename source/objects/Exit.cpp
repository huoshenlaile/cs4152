#include "Exit.h"
bool Exit::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    std::shared_ptr<JsonValue> properties = json -> get("properties");
    // find the Publication property
    for (int i = 0; i < properties->size(); i++){
        if (properties->get(i)->getString("propertytype") == "PubMessage"){
            auto pub = properties->get(i)->get("value");
            message_head = pub->getString("Head");
            break;
        }
    }
    activated = true;
    _selfObstacle->setSensor(true);
    OnBeginContactEnabled = true;
    OnEndContactEnabled = true;
    timeUpdateEnabled = true;

    return true;
}

PublishedMessage Exit::onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){
    //CULog("Sensor of [%s] tested", message_head.c_str());
    if (isCharacter){
        // if the sensor is activated
        if (activated){
            if (other->getName() == "body192"){
                
                CULog("on_contact [%s]", message_head.c_str());
                if (this->_colorsCollected.count(character->getColor()) == 0){
                    is_contacting=true;
                    _character = character;
                }
                
            }
        }
    }
    return PublishedMessage();
}

PublishedMessage Exit::onEndContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){
    //CULog("Sensor of [%s] released", message_head.c_str());
    if (isCharacter){
        // if the sensor is activated
        if (activated){
            if (other->getName() == "body192"){
                CULog("on_release [%s]", message_head.c_str());
                if (this->_colorsCollected.count(character->getColor()) != 0){
                    is_contacting=false;
                    this->contact_time = -2.0f;
                    _character = character; // Gonna leave this here for now
                }
                
            }
        }
    }
    return PublishedMessage();
}

PublishedMessage Exit::timeUpdate(float timestep){
    if (this->contact_time < 0 || (this->is_contacting && !this->contactedLongEnough())){
            this->contact_time += timestep;
            std::cout << this->contact_time;
            std::cout << "Contact time: " << this->contact_time << "\n";
        }
    else if (this->is_contacting && this->contactedLongEnough()){
        std::string color = _character->getColor();
        if (this->getColorsCollected().count(color) == 0){
            this->addColor(color);
            std::cout << "Found color " << color << "\n";
            _character->setColor("black");
        }
        if (this->getRemainingColors().size()==0){
            CULog("Winner! - Setting Complete.");
            auto a = PublishedMessage();
            a.Head = message_head;
            return a;
        }
    }
    return PublishedMessage();
}
