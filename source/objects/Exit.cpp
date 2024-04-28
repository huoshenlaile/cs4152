#include "Exit.h"
bool Exit::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    _selfTexture->setColor(Color4(0,0,0,0));
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
                _colorReqs.push_back(substr);
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

bool Exit::goodEnding(std::vector<std::string> _colorReqs, std::vector<std::string> _colorsCollected) {
        std::sort(_colorReqs.begin(),_colorReqs.end());
        std::sort(_colorsCollected.begin(), _colorsCollected.end());

    if (_colorReqs.size() != _colorsCollected.size()){
        return false;
    }

        for (int i = 0; i < _colorReqs.size(); i++){
            std::cout << _colorReqs[i] << std::endl;
            if(_colorReqs[i] != _colorsCollected[i]){
                return false;
            }
        }
    return true;
}

PublishedMessage Exit::onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){
    //CULog("Sensor of [%s] tested", message_head_gameend.c_str());
    if (isCharacter){
        // if the sensor is activated
        if (activated){
            if (other->getName() == "body192"){
                CULog("on_contact [%s]", message_head_gameend.c_str());
                if ((character->getColor() != "black") || (character->getColor() == "black" && _colorReqs.size() == 0)){
                    if(!this->is_contacting){
                        this->is_contacting=true;
                        _character = character;
                        auto a = PublishedMessage();
                        a.Head = message_head_contact;
                        a.enable = true;
                        return a;
                    }
                    else{
                        this->debouncing=true;
                        //If i am touching the exit and i touch it again, do not run exit code
                    }
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
                if(this->is_contacting && !this->debouncing){
                    this->is_contacting=false;
                    auto a = PublishedMessage();
                    a.Head = message_head_release;
                    a.enable = true;
                    a.float1 = this->contact_time / this->_ttcolor;
                    return a;
                }
                else if(this->debouncing){
                    this->debouncing = false;
                    // If i cross the inner boundary in the exit
                    // onStartContact may run first -> then onEndContact overrides it
                    // Make sure not to run onEndContact in this case -> player doesnt exit while inside sensor
                }
            }
        }
    }
    return PublishedMessage();
}

// only called if color is new to the palette
void Exit::addColor(std::string color, Vec2 character_scene_pos) {
    _colorsCollected.push_back(color);
    std::shared_ptr<scene2::PolygonNode> splatterpoly;
    if(color == "green"){
        splatterpoly = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("green_splatter"));
    } else if ( color == "blue" ){
        splatterpoly = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("blue_splatter"));
    } else if ( color == "orange" ) {
        splatterpoly = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("orange_splatter"));
    } else if ( color == "purple" ) {
        splatterpoly = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("purple_splatter"));
    } else if ( color == "pink" ) {
        splatterpoly = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("pink_splatter"));
    }
    
    if(splatterpoly != nullptr){
        splatterpoly->setScale(0.5f, 1.0f);
        splatterpoly->setPosition(character_scene_pos.x, _selfTexture->getPositionY());
        _scene->addChild(splatterpoly);
    }
    
}

PublishedMessage Exit::timeUpdate(float timestep){
    if (this->contact_time < 0 || (this->is_contacting && !this->contactedLongEnough())){
            this->contact_time += timestep;
           // std::cout << "Contact time: " << this->contact_time << "\n";
    }
    else if (this->is_contacting && this->contactedLongEnough()){
        this->contact_time = this->_ttcolor;
        std::string color = _character->getColor();
        if (color != "black"){
            this->addColor(color, _character->getBodySceneNode()->getPosition());
            _character->setColor("black");
        }
//        if (color != "black" && this->getColorsCollected().count(color) == 0){
//            std::cout << "Found color (from Exit timeUpdate):" << color << "\n";
//            _character->setColor("black");
//        }
        
        std::cout << this->getColorsCollected().size() << " phone " << this->getColorReqs().size() << std::endl;
        if (this->getColorsCollected().size() >= this->getColorReqs().size()){
            CULog("Game over! - Setting Complete.");
            auto a = PublishedMessage();
            a.Head = message_head_gameend;
            if (goodEnding(_colorReqs, _colorsCollected)){
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
    else if (!this->is_contacting){
        this->contact_time = 0.0f;
    }
    auto b = PublishedMessage();
    b.Head = "ExitTimeTick";
    b.enable = true;
    b.float1 = this->contact_time / this->_ttcolor;

    return b;
}
