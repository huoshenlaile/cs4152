//
//  DrippingPaint.cpp
//  DustyPaints
//
//  Created by Emily on 4/20/24.
//

#include "DrippingPaint.hpp"

bool DrippingPaint::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
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
        }else if(properties->get(i)->getString("name") == "AnimationAsset"){
            auto p = properties->get(i)->get("value")->asString();
            _textureName = p;
        } else if(properties->get(i)->getString("name") == "speed"){
            auto p = properties->get(i)->get("value")->asFloat();
            _dripSpeed = p;
        }
    }
    
    std::cout << "FINISHED DRIPPING PAINT!!!!!!!! ======= " << std::endl;
    
    activated = true;
    _selfObstacle->setSensor(true);
    OnBeginContactEnabled = true;
    timeUpdateEnabled = true;
    is_out = true;
    
    _actions = scene2::ActionManager::alloc();
    //TODO: May change per animation, if it does then parameterize in tiled
    _animate = scene2::Animate::alloc(0, 7, 1.0f, 1);
    
    return true;
}

bool DrippingPaint::linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode> &sceneNode, float scale){
    std::cout << "DRIP PAINT LINKING TO WORLD" << std::endl;
    bool success = Interactable::linkToWorld(physicsWorld, sceneNode, scale);
    scene2::SceneNode* weak = _selfTexture.get();
    _selfObstacle->setListener([=](physics2::Obstacle* obs){
        weak->setPosition(obs->getPosition() * scale);
        weak->setAngle(obs->getAngle());
        

    });
    return success;

    /*_world = physicsWorld;
    _scene = sceneNode;
    
    _world->addObstacle(_selfObstacle);
    _selfTexture->setVisible(true);
    _selfTexture->setPosition(_selfObstacle->getPosition() * scale);
    _selfTexture->setColor(Color4::BLACK);

    _scene->addChild(_selfTexture);
    is_out = true;*/
    //return true;
}

PublishedMessage DrippingPaint::onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character){
    if (isCharacter){
        // if the sensor is activated
        if (activated && is_out){
            if (other->getName() == "body192"){
                auto a = PublishedMessage();
                a.Head = pub_message_head;
                a.Body = color;
                std::cout << "=======Published a message being like: " << a.Head << " " << a.Body << std::endl;
                return a;
            }
        }
    }
    return PublishedMessage();
}

PublishedMessage DrippingPaint::timeUpdate(float timestep){
    // Calculate the next position along the path
    Vec2 currentPosition = _selfObstacle->getPosition();
    Vec2 currentBounds = _selfObstacle->getSize();
    float step = _dripSpeed * timestep;
    _selfTexture->setAnchor(0, 1);
    _selfTexture->setScale(_selfTexture->getScale().x, _selfTexture->getScale().y * _dripSpeed);

    _selfObstacle->setSize(currentBounds.x, _selfTexture->getScale().y * _dripSpeed);
   
    //currentPosition.y -= step;
    
    //_selfTexture->setContentSize(_selfObstacle->getWidth() * scale2d.x * 1.01, _selfObstacle->getHeight() * scale2d.y * 1.01);

    //_selfObstacle->setPosition(currentPosition);

    return PublishedMessage();
}
