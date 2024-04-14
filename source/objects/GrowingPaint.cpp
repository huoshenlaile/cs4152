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
        }else if(properties->get(i)->getString("name") == "AnimationAsset"){
            auto p = properties->get(i)->get("value")->asString();
            _textureName = p;

        }
    }
        
    activated = true;
    _selfObstacle->setSensor(true);
    OnBeginContactEnabled = true;
    timeUpdateEnabled = true;
    is_out = false;
    
    _actions = scene2::ActionManager::alloc();
    //TODO: May change per animation, if it does then parameterize in tiled
    _animate = scene2::Animate::alloc(0, 7, 1.0f, 1);
    
    return true;
}

bool GrowingPaint::linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode> &sceneNode, float scale){
    _world = physicsWorld;
    _scene = sceneNode;
    
    _world->addObstacle(_selfObstacle);
    _selfTexture->setPosition(_selfObstacle->getPosition() * scale);
    // add message
    
    _animation = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(_textureName), 3, 3, 8);
    _animation->setScale(2.5f);
    _animation->setPosition(_selfObstacle->getPosition() * scale);

    actions[sub_message_head] = ([=](ActionParams params){
        if (is_out){
            return PublishedMessage();
        }
        _scene->addChild(_selfTexture);
        _scene->addChild(_animation);
        _actions->activate("other", _animate, _animation);

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
                a.Body = color;
                std::cout << "=======Published a message being like: " << a.Head << " " << a.Body << std::endl;
                return a;
            }
        }
    }
    return PublishedMessage();
}

PublishedMessage GrowingPaint::timeUpdate(float timestep){
    if(is_out){
        _actions->update(timestep);
    }
    return PublishedMessage();
}
