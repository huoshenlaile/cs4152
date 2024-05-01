#include "GrowingPaint.h"
bool GrowingPaint::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    std::shared_ptr<JsonValue> properties = json -> get("properties");
    // find the Publication property
    is_out = false;
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
        } else if(properties->get(i)->getString("name") == "instant"){
            auto p = properties->get(i)->get("value")->asBool();
            std::cout << "INSTANT PAINT? " << p << std::endl;
            is_out = p;
        } else if(properties->get(i)->getString("name") == "rotation"){
            auto r = properties->get(i)->get("value")->asFloat();
            angle_splash = CU_MATH_DEG_TO_RAD(r);
        }
    }
        
    activated = true;
    _selfObstacle->setSensor(true);
    OnBeginContactEnabled = true;
    timeUpdateEnabled = true;
    
    _actions = scene2::ActionManager::alloc();
    //TODO: May change per animation, if it does then parameterize in tiled
    _animate = scene2::Animate::alloc(0, 8, 1.0f, 1);
    
    return true;
}

bool GrowingPaint::linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode> &sceneNode, float scale){
    _world = physicsWorld;
    _scene = sceneNode;
    
    _world->addObstacle(_selfObstacle);
    _selfTexture->setPosition(_selfObstacle->getPosition() * scale);
    // add message
    
    std::cout << "animation asset: " <<_textureName + "_splash" << std::endl;
    _animation = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(_textureName + "_splash"), 3, 3, 9);
    _animation->setScale(2.5f);
    _animation->setAngle(angle_splash);
    _animation->setPosition(_selfObstacle->getPosition() * scale);
    
    if(is_out){ // if its initially out, it should have no sub
        _scene->addChild(_selfTexture);
        _scene->addChild(_animation);
        _actions->activate("initial", _animate, _animation);
    } else {
        std::cout << "ADDDING AN ACTION FOR " <<sub_message_head << std::endl;
        actions[sub_message_head] = ([=](ActionParams params){
            if (is_out){
                return PublishedMessage();
            }
            _scene->addChild(_selfTexture);
            _scene->addChild(_animation);
            _actions->activate("initial", _animate, _animation);
            
            is_out = true;
            return PublishedMessage();
        });
    }
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
                std::cout << "======= Published a message like: " << a.Head << " " << a.Body << std::endl;
                return a;
            }
        }
    }
    return PublishedMessage();
}

PublishedMessage GrowingPaint::timeUpdate(float timestep){
    if(is_out){
        _actions->update(timestep);
        if(!_actions->isActive("initial") && !_initial_splash_done){ // either its complete, or it no longer exists.
            std::cout << "activating second paint animation" << std::endl;
            _initial_splash_done = true;
            auto new_anim = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(_textureName + "_splat"), 3, 3, 9);
            new_anim->setScale(1.5f);
            new_anim->setPosition(_animation->getPosition());
            _scene->removeChild(_animation);
            _scene->addChild(new_anim);
            _actions->activate("splat", scene2::Animate::alloc(0, 8, 1.0f, 1), new_anim);
            
        }
    }
    return PublishedMessage();
}
