#include "interactable.h"



bool Interactable::init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds){
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
            break;
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
    _selfObstacle = cugl::physics2::PolygonObstacle::allocWithAnchor(shape, Vec2(0.5f,0.5f));
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

    _selfObstacle->setPosition(getObjectPos(json));
    _selfObstacle->setAnchor(0.5f, 0.5f);
    _selfObstacle->setAngle((360.0f - json->getFloat("rotation")) * M_PI / 180.0f);

    for (auto prop : properties -> children()) {
        if (prop -> getString("name") == "Grab") {
            canBeGrabbed = prop -> getBool("value");
        }
        if (prop -> getString("name") == "JumpScale") {
            jumpscale = prop -> getFloat("value");
        }
        if(prop -> getString("name") == "LoadOnce") {
            loadOnce = prop -> getBool("value");
        }
    }
    if (physicalProperties!=nullptr){
        _texture_name = physicalProperties->get("obstacle")->getString(TEXTURE_FIELD);
    }
    return success;
}

bool Interactable::bindAssets(const std::shared_ptr<cugl::AssetManager>& assets, Vec2 scale2d){
    _assets = assets;
    if (_texture_name == ""){
        _selfTexture->setContentSize(_selfObstacle->getWidth() * scale2d.x * 1.01, _selfObstacle->getHeight() * scale2d.y * 1.01);
        //_selfTexture->setColor(Color4::BLACK);
        _selfTexture->setAnchor(0.5f, 0.5f);
        _selfTexture->setAngle(_selfObstacle->getAngle());
        _selfTexture->setColor(Color4(0,0,0,0));
        return true;
    }
    _selfTexture = scene2::PolygonNode::allocWithTexture(assets->get<Texture>(_texture_name));
    _selfTexture->setContentSize(_selfObstacle->getWidth() * scale2d.x * 1.01, _selfObstacle->getHeight() * scale2d.y * 1.01);
    _selfTexture->setAnchor(0.5f, 0.5f);
    _selfTexture->setAngle(_selfObstacle->getAngle());
    return true;
}

bool Interactable::linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode>& sceneNode, float scale){
    _world = physicsWorld;
    _scene = sceneNode;
    
    _world->addObstacle(_selfObstacle);

    if (_selfTexture == nullptr){
        return false;
    }

    _selfTexture->setPosition(_selfObstacle->getPosition() * scale);
    _selfTexture->setVisible(true);
    _scene->addChild(_selfTexture);
    if (_selfObstacle->getBodyType() == b2_dynamicBody){
        scene2::SceneNode* weak = _selfTexture.get();
        _selfObstacle->setListener([=](physics2::Obstacle* obs){
            weak->setPosition(obs->getPosition() * scale);
            weak->setAngle(obs->getAngle());
        });
    }

    return true;
}


// ================================ private helper functions ================================
std::vector<float> Interactable::getVertices(const std::shared_ptr<JsonValue>& json) {
    std::vector<float> vertices = std::vector<float>();
    for (auto it = json->get(VERTICES_FIELD)->children().begin(); it != json->get(VERTICES_FIELD)->children().end(); it++) {
        vertices.push_back(((_bounds.getMaxY() * _scale.y) - ((*it)->get(1)->asFloat() + json->getFloat("y"))) / _scale.y);
        vertices.push_back(((*it)->get(0)->asFloat() + json->getFloat("x")) / _scale.x);
    }
    // reversing is necessary so that the resulting polygon is in the right draw order
    std::reverse(vertices.begin(), vertices.end());
    return vertices;
}

Vec2 Interactable::getObjectPos(const std::shared_ptr<JsonValue>& json) {
    Vec2 pos = Vec2(json->getFloat("x") / _scale.x, ((_bounds.getMaxY() * _scale.y) - json->getFloat("y") + _scale.y) / _scale.y);
    return pos;
}
