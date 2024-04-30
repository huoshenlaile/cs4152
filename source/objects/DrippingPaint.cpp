//
//  DrippingPaint.cpp
//  DustyPaints
//
//  Created by Emily on 4/20/24.
//

#include "DrippingPaint.hpp"

bool DrippingPaint::init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds){
    // call super.init
    bool success = true;
    _scale = scale;
    _bounds = bounds;
    _name = json->getString("name");
    std::shared_ptr<JsonValue> properties = json -> get("properties");
    // auto physicalProperties = properties -> get(properties -> size() - 1) -> get("value");
    std::shared_ptr<JsonValue> physicalProperties = nullptr;
    for (int i = 0; i < properties->size(); i++){
        if (properties->get(i)->getString("propertytype") == "Physics"){
            physicalProperties = properties->get(i)->get("value");
            //break;
        } else if (properties->get(i)->getString("propertytype") == "SubMessage"){
            auto pub = properties->get(i)->get("value");
            sub_message_head = pub->getString("Head");
        } else if(properties->get(i)->getString("propertytype") == "PubMessage"){
            auto p = properties->get(i)->get("value");
            pub_message_head = p->getString("Head");
            color = p->getString("Body");
        } else if(properties->get(i)->getString("name") == "AnimationAsset"){
            auto p = properties->get(i)->get("value")->asString();
            _textureName = p;
        } else if(properties->get(i)->getString("name") == "speed"){
            std::cout << "speed found" << std::endl;
            auto p = properties->get(i)->get("value")->asFloat();
            _dripSpeed = p;
        }
    }
    int polysize = (int)json->get(VERTICES_FIELD)->children().size();
    success = success && polysize > 0;
    
    if (!success) return false;

    std::vector<float> vertices = getVertices(json);
    success = success && 2*polysize == vertices.size();
    if (!success) return false;

    Vec2* verts = reinterpret_cast<Vec2*>(&vertices[0]);
    Poly2 shape(verts,(int)vertices.size()/2);
    EarclipTriangulator triangulator;
    triangulator.set(shape.vertices);
    triangulator.calculate();
    shape.setIndices(triangulator.getTriangulation());
    triangulator.clear();
    
    _selfTexture = scene2::PolygonNode::allocWithPoly(shape);
    //_selfObstacle = cugl::physics2::PolygonObstacle::alloc(shape);
    _selfObstacle = cugl::physics2::PolygonObstacle::allocWithAnchor(shape, Vec2(0.0f,1.0f));
    _selfObstacle->setName(json->getString("name"));
    if (physicalProperties != nullptr){
        _selfObstacle->setBodyType((b2BodyType)physicalProperties->get("obstacle")->getInt(BODYTYPE_FIELD));
        _selfObstacle->setDensity(physicalProperties->get("obstacle")->getDouble(DENSITY_FIELD));
        _selfObstacle->setFriction(physicalProperties->get("obstacle")->getDouble(FRICTION_FIELD));
        _selfObstacle->setRestitution(physicalProperties->get("obstacle")->getDouble(RESTITUTION_FIELD));
    }else{
        _selfObstacle->setBodyType(b2_staticBody);
        _selfObstacle->setDensity(1.0f);
        _selfObstacle->setFriction(0.0f);
        _selfObstacle->setRestitution(0.0f);
    }

    _selfObstacle->setAnchor(0.0f, 1.0f);
    _selfObstacle->setPosition(getObjectPos(json));
    _selfObstacle->setAngle((360.0f - json->getFloat("rotation")) * M_PI / 180.0f);

    for (auto prop : properties -> children()) {
        if (prop -> getString("name") == "Grab") {
            canBeGrabbed = prop -> getBool("value");
        }
    }
    if (physicalProperties!=nullptr){
        _texture_name = physicalProperties->get("obstacle")->getString(TEXTURE_FIELD);
    }
    
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

bool DrippingPaint::bindAssets(const std::shared_ptr<cugl::AssetManager>& assets, Vec2 scale2d){
    _assets = assets;
    _scale2d = scale2d;
    _selfTexture = scene2::PolygonNode::allocWithTexture(assets->get<Texture>(_texture_name));
    _selfTexture->setAnchor(0.0f, 0.0f);
    _selfTexture->setContentSize(_selfObstacle->getWidth() * scale2d.x * 1.01, _selfObstacle->getHeight() * scale2d.y * 1.01);
    _selfTexture->setAngle(_selfObstacle->getAngle());
    return true;
}

bool DrippingPaint::linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode> &sceneNode, float scale){
    bool success = Interactable::linkToWorld(physicsWorld, sceneNode, scale);
    //bool success = true;
    scene2::SceneNode* weak = _selfTexture.get();
    _selfObstacle->setListener([=](physics2::Obstacle* obs){
        weak->setContentSize(weak->getContentWidth(), _selfObstacle->getHeight() * _scale2d.y * 1.01);
        weak->setPosition(obs->getPosition() * scale);

    });
    return success;
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
    //float step = _dripSpeed * timestep;
    //std::cout << "THIS FUCKING PAINTS pos " << currentPosition.x << ", " << currentPosition.y << " and size " << currentBounds.x  << ", " << currentBounds.y << std::endl;
    
    //std::cout << "THIS FUCKING PAINTS pos " << _selfTexture->getPositionY() << " and size " << _selfTexture->getHeight() << std::endl;
    _selfObstacle->setPosition(currentPosition.x, currentPosition.y - _dripSpeed);
    _selfObstacle->setHeight(_selfObstacle->getHeight() + _dripSpeed);
    //_selfTexture->setScale(_selfTexture->getScale().x, _selfTexture->getScale().y + _dripSpeed);

    return PublishedMessage();
}
