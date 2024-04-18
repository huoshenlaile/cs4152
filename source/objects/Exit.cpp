#include "Exit.h"
bool Exit::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    //_selfTexture->setColor(Color4(255,0,0,125));
    std::shared_ptr<JsonValue> properties = json -> get("properties");
    // find the Publication property
    for (int i = 0; i < properties->size(); i++){
        auto prop = properties->get(i);
        if (prop->getString("propertytype") == "PubMessage" ){
            if (prop->getString("name") == "Pub1"){
                auto pub = prop->get("value");
                message_head_gameend = pub->getString("Head");
            }
            if (prop->getString("name") == "PubContacting"){
                auto pub = prop->get("value");
                message_head_contact = pub->getString("Head");
            }
            if (prop->getString("name") == "PubReleasing"){
                auto pub = prop->get("value");
                message_head_release = pub->getString("Head");
            }
        }
    }
    if (properties->get(properties->size()-1)->getString("propertytype") == "Physics"){
        std::stringstream reqs(properties->get(properties->size()-1)->get("value")->get("obstacle")->get("color-requirements")->asString());
        while(reqs.good()){
            std::string substr;
            std::getline(reqs, substr, ',');
            if (substr.length()>0){
                _colorReqs.insert(substr);
            }
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
    //CULog("Sensor of [%s] tested", message_head_gameend.c_str());
    if (isCharacter){
        // if the sensor is activated
        if (activated){
            if (other->getName() == "body192"){
                CULog("on_contact [%s]", message_head_gameend.c_str());
                if ((character->getColor() != "black" && this->_colorsCollected.count(character->getColor()) == 0) || (character->getColor() == "black" && _colorReqs.size() == 0)){
                    this->is_contacting=true;
                    _character = character;
                    auto a = PublishedMessage();
                    a.Head = message_head_contact;
                    a.enable = true;
                    return a;
                }
                
            }
        }
    }
    return PublishedMessage();
}

PublishedMessage Exit::onEndContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){
    //CULog("Sensor of [%s] released", message_head_gameend.c_str());
    if (isCharacter){
        // if the sensor is activated
        if (activated){
            if (other->getName() == "body192"){
                CULog("on_release [%s]", message_head_gameend.c_str());
                this->is_contacting=false;
                this->contact_time = 0.0f;
                auto a = PublishedMessage();
                a.Head = message_head_release;
                a.enable = true;
                a.float1 = this->contact_time / this->_ttcolor;
                return a;
            }
        }
    }
    return PublishedMessage();
}

PublishedMessage Exit::timeUpdate(float timestep){
    if (this->contact_time < 0 || (this->is_contacting && !this->contactedLongEnough())){
            this->contact_time += timestep;
           // std::cout << "Contact time: " << this->contact_time << "\n";
        }
    else if (this->is_contacting && this->contactedLongEnough()){
        this->contact_time = this->_ttcolor;
        std::string color = _character->getColor();
        if (color != "black" && this->getColorsCollected().count(color) == 0){
            this->addColor(color);
            std::cout << "Found color " << color << "\n";
            _character->setColor("black");
        }
        if (this->getColorsCollected().size() >= this->getColorReqs().size()){
            CULog("Game over! - Setting Complete.");
            auto a = PublishedMessage();
            a.Head = message_head_gameend;
            if (this->getRemainingColors().size()==0){
                CULog("Good ending!");
                a.Body = "Good";
            }
            else{
                CULog("Bad ending!");
                a.Body = "Bad";
            }
            a.enable = true;

            return a;
        }
        
    }
    auto b = PublishedMessage();
    b.Head = "ExitTimeTick";
    b.enable = true;
    b.float1 = this->contact_time / this->_ttcolor;

    return b;
}
