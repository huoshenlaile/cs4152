#include "InteractableUI.h"

bool InteractableUI::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    Interactable::init(json, scale, bounds);
    _selfTexture->setColor(Color4(0,0,0,0));
    std::shared_ptr<JsonValue> properties = json -> get("properties");
    // find the Publication property
    for (int i = 0; i < properties->size(); i++){
        auto prop = properties->get(i);
        if (prop->getString("propertytype") == "SubMessage" && prop->get("value")->getString("Action") == "show"){
            auto pub = prop->get("value");
            show_subscription_head = pub->getString("Head");
            std::cout << "found show_subscription_head " << show_subscription_head << std::endl;
            _show_cd = pub->getFloat("Float1");
        }else if(prop->getString("propertytype") == "SubMessage" && prop->get("value")->getString("Action") == "hide"){
            auto pub = prop->get("value");
            hide_subscription_head = pub->getString("Head");
            _hide_cd = pub->getFloat("Float1");
        }
        else if(prop->getString("propertytype") == "SubMessage" && prop->get("value")->getString("Action") == "toggle"){
            auto pub = prop->get("value");
            toggle_subscription_head = pub->getString("Head");
        }
        else if(properties->get(i)->getString("name") == "AnimationDetails"){
            _animated = true;
            auto p = properties->get(i)->get("value");
            _textureName = p->getString("texture");
            _cells = p->getInt("cells");
            _cols = p->getInt("columns");
            _rows = p->getInt("rows");
            _repeat = p->getBool("repeat", true);
            _aniname = p->getString("name");
            _scale = p->getFloat("scale");

        }
    }
    activated = true;
    _selfObstacle->setSensor(true);
    timeUpdateEnabled = true;
    _actions = scene2::ActionManager::alloc();
    //TODO: May change per animation, if it does then parameterize in tiled
    _animate = scene2::Animate::alloc(0, _cells-1, 1.0f, 1);

    return true;
}

bool InteractableUI::linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode> &sceneNode, float scale){
    _world = physicsWorld;
    _scene = sceneNode;
    
    _world->addObstacle(_selfObstacle);
    _selfTexture->setPosition(_selfObstacle->getPosition() * scale);
    _scene->addChild(_selfTexture);
    if (_animated){
        _animation = scene2::SpriteNode::allocWithSheet(_assets->get<Texture>(_textureName), _rows, _cols, _cells);
        _animation->setScale(_scale);
        _animation->setPosition(_selfObstacle->getPosition() * scale);
        
        _scene->addChild(_animation);
        _actions->activate(_aniname, _animate, _animation);
        _animation->setVisible(false);

    }
    
    _selfTexture->setVisible(false);
    
    
    // add message
    

    actions[show_subscription_head] = ([&](ActionParams params){
        std::cout << "SHOW IS TRIGGERED?????? _cd >= _show_cd " << _cd << " >= " << _show_cd << std::endl;
        if (!_shown_already && _cd >= _show_cd && !_selfTexture->isVisible()){
            std::cout << "UI TEXT IS SHOWN!!!!" << std::endl;
            _cd=0;
            _shown_already = true;
            _selfTexture->setVisible(true);
            if (_animated){
                _animation->setVisible(true);
            }
            PublishedMessage m = PublishedMessage();
            m.Head = getName() + " shown";
            m.enable = true;
            return m;
        }
        return PublishedMessage();
    });
    if(hide_subscription_head != ""){
        actions[hide_subscription_head] = ([=](ActionParams params){
            if (_cd >= _hide_cd && _selfTexture->isVisible()){
                _cd=0;
                _selfTexture->setVisible(false);
                if(_animated){
                    _animation->setVisible(false);
                }
                PublishedMessage m = PublishedMessage();
                m.Head = getName() + " hidden";
                m.enable = true;
                return m;
            }
            return PublishedMessage();
        });
    }
    if(toggle_subscription_head != ""){
        actions[toggle_subscription_head] = ([=](ActionParams params){
            _selfTexture->setVisible(!_selfTexture->isVisible());
            PublishedMessage m = PublishedMessage();
            m.Head = getName() + " toggle";
            m.enable = true;
            return m;
        });
    }
    return true;
}

PublishedMessage InteractableUI::timeUpdate(float timestep){
    // instead... -- only one timer needs to run at time, true.
    // if showCDTimerTriggered (from action), start incrementing _cd. When it hits _show_cd, set _shown_already, and activate the animation
    // if hideCDTimerTriggered (from action), start incrementing _cd. When it hits _hide_cd, set invisble everything.
    if ((_cd <= _show_cd || _cd <= _hide_cd) /*&& _selfTexture->isVisible()*/){
        _cd+=timestep;
    }
    if (_animated && _shown_already){
        _actions->update(timestep);
        if (!_actions->isActive(_aniname) && _repeat){
            _actions->activate(_aniname, _animate, _animation);
        }
    }
    
    return PublishedMessage();
}
