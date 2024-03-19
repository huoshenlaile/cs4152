#include "LevelLoader.h"
#include "LevelConstants.h"

std::vector<float> LevelLoader::getVertices(const std::shared_ptr<JsonValue>& json) {
    std::vector<float> vertices = std::vector<float>();
    for (auto it = json->get(VERTICES_FIELD)->children().begin(); it != json->get(VERTICES_FIELD)->children().end(); it++) {
        vertices.push_back(((_bounds.getMaxY() * _scale.y) - ((*it)->get(1)->asFloat() + json->getFloat("y"))) / _scale.y);
        vertices.push_back(((*it)->get(0)->asFloat() + json->getFloat("x")) / _scale.x);
    }
    // reversing is necessary so that the resulting polygon is in the right draw order
    std::reverse(vertices.begin(), vertices.end());
    return vertices;
}

Vec2 LevelLoader::getObjectPos(const std::shared_ptr<JsonValue>& json) {
    Vec2 pos = Vec2(json->getFloat("x") / _scale.x, ((_bounds.getMaxY() * _scale.y) - json->getFloat("y") + _scale.y) / _scale.y);
    return pos;
}
/**
* Converts the string to a color
*
* Right now we only support the following colors: yellow, red, blur, green,
* black, and grey.
*
* @param  name the color name
*
* @return the color for the string
*/
Color4 LevelLoader::parseColor(std::string name) {
    if (name == "yellow") {
        return Color4::YELLOW;
    } else if (name == "red") {
        return Color4::RED;
    } else if (name == "green") {
        return Color4::GREEN;
    } else if (name == "blue") {
        printf("BLUE IS HERE!");
        return Color4::BLUE;
    } else if (name == "black") {
        return Color4::BLACK;
    } else if (name == "gray") {
        return Color4::GRAY;
    }
    return Color4::WHITE;
}

/**
 * Loads this game level from the source file
 *
 * This load method should NEVER access the AssetManager.  Assets are loaded in
 * parallel, not in sequence.  If an asset (like a game level) has references to
 * other assets, then these should be connected later, during scene initialization.
 *
 * @return true if successfully loaded the asset from a file
 */
bool LevelLoader::preload(const std::string& file) {
    std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset(file);
    return preload(reader->readJson());
}

/**
 * Loads this game level from the source file
 *
 * This load method should NEVER access the AssetManager.  Assets are loaded in
 * parallel, not in sequence.  If an asset (like a game level) has references to
 * other assets, then these should be connected later, during scene initialization.
 *
 * @return true if successfully loaded the asset from a file
 */
bool LevelLoader::preload(const std::shared_ptr<cugl::JsonValue>& json) {
    if (json == nullptr) {
        CUAssertLog(false, "Failed to load level file");
        return false;
    }
    // Initial geometry
    float w = json->get(WIDTH_FIELD)->asFloat();
    float h = json->get(HEIGHT_FIELD)->asFloat();
//    float g = json->get("properties")->get(0)->getFloat("value", -49);
//    CULog("WELL the level is preloaded");
    _bounds.size.set(w, h);
    _gravity.set(0,-90.0f);
    /** Create the physics world */
      _world = cugl::physics2::net::NetWorld::alloc(getBounds(),Vec2(0,-90.f));
    
//
//    cugl::scene2::TexturedNode();
    // Get each object in each layer, then decide what to do based off of what
    // type the object is.
    for (int i = 0; i < json->get("layers")->size(); i++) {
        // Get the objects per layer
        auto objects = json->get("layers")->get(i)->get("objects");
        for (int j = 0; j < objects->size(); j++) {
            // For each object, determine what it is and load it
            loadObject(objects->get(j));
        }
    }

    
    return true;
}

bool LevelLoader::loadObject(const std::shared_ptr<JsonValue>& json) {
    auto type = json->get("type")->asString();
    if (type == WALLS_FIELD) {
        return loadWall(json);
    }else if (type == GOALDOOR_FIELD) {
        return loadGoalDoor(json);
    }
    return false;
}

void LevelLoader::setBackgroundScene(std::shared_ptr<Texture> background){
    auto sprite = scene2::PolygonNode::allocWithTexture(background, Rect(-100.0f,-100.0f,20000.0f, 20000.0f)); //TODO: don't hard code
    _worldnode->addChild(sprite);
}

/**
* Loads the singular exit door
*
* The exit door will will be stored in _goalDoor field and retained.
* If the exit fails to load, then _goalDoor will be nullptr.
*
* @param  reader   a JSON reader with cursor ready to read the exit
*
* @retain the exit door
* @return true if the exit door was successfully loaded
*/
bool LevelLoader::loadGoalDoor(const std::shared_ptr<JsonValue>& json) {
    bool success = false;
    auto goal = json->get("properties")->get(0)->get("value");
    if (goal != nullptr) {
        success = true;

        Vec2 goalPos = getObjectPos(json);
        Vec2 goalSize = Vec2(json->getFloat("width") / _scale.x, json->getFloat("height") / _scale.y);
        // For whatever reason, the goal objects in particular are offset by this amount.
        goalPos.x += goalSize.x / 2;
        goalPos.y += goalSize.y / 6;

        // Get the object, which is automatically retained
        _goalDoor = ExitModel::alloc(goalPos,(Size)goalSize);
        _goalDoor->setName(GOALDOOR_FIELD);

        _goalDoor->setDensity(goal->get("obstacle")->getDouble(DENSITY_FIELD));
        _goalDoor->setFriction(goal->get("obstacle")->getDouble(FRICTION_FIELD));
        _goalDoor->setRestitution(goal->get("obstacle")->getDouble(RESTITUTION_FIELD));
        _goalDoor->setSensor(true);

        _goalDoor->setBodyType((b2BodyType)goal->get("obstacle")->getInt(BODYTYPE_FIELD));

        // Set the texture value
        success = success && goal->get("obstacle")->get(TEXTURE_FIELD)->isString();
        _goalDoor->setTextureKey(goal->get("obstacle")->get(TEXTURE_FIELD)->asString());
        _goalDoor->setDebugColor(parseColor(goal->get("obstacle")->getString(DEBUG_COLOR_FIELD)));

        if (success) {
//            _world->addObstacle(_goalDoor);
        }
        else {
            _goalDoor = nullptr;
        }
    }
    return success;
}

/**
* Loads a single wall object
*
* The wall will be retained and stored in the vector _walls.  If the
* wall fails to load, then it will not be added to _walls.
*
* @param  reader   a JSON reader with cursor ready to read the wall
*
* @retain the wall
* @return true if the wall was successfully loaded
*/
bool LevelLoader::loadWall(const std::shared_ptr<JsonValue>& json) {
    bool success = true;
    auto walljson = json->get("properties")->get(0)->get("value");
    
    int polysize = (int)json->get(VERTICES_FIELD)->children().size();
    success = success && polysize > 0;

    std::vector<float> vertices = getVertices(json);
    success = success && 2*polysize == vertices.size();

    Vec2* verts = reinterpret_cast<Vec2*>(&vertices[0]);
    Poly2 wall(verts,(int)vertices.size()/2);
    EarclipTriangulator triangulator;
    triangulator.set(wall.vertices);
    triangulator.calculate();
    wall.setIndices(triangulator.getTriangulation());
    triangulator.clear();
    
    // Get the object, which is automatically retained
    std::shared_ptr<WallModel> wallobj = WallModel::alloc(wall);
    wallobj->setName(WALLS_FIELD+json->getString("id"));

    wallobj->setBodyType((b2BodyType)walljson->get("obstacle")->getInt(BODYTYPE_FIELD));

    wallobj->setDensity(walljson->get("obstacle")->getDouble(DENSITY_FIELD));
    wallobj->setFriction(walljson->get("obstacle")->getDouble(FRICTION_FIELD));
    wallobj->setRestitution(walljson->get("obstacle")->getDouble(RESTITUTION_FIELD));

    // Set the texture value
    success = success && walljson->get("obstacle")->get(TEXTURE_FIELD)->isString();
    wallobj->setTextureKey(walljson->get("obstacle")->getString(TEXTURE_FIELD));
    wallobj->setDebugColor(parseColor(walljson->get("obstacle")->getString(DEBUG_COLOR_FIELD)));

    if(walljson->get("obstacle")->getBool(ROTATION_FIELD)){
        wallobj->setBodyType(b2_dynamicBody);
        wallobj->setAngularVelocity(10.0);
    }
    
    if (success) {
        _walls.push_back(wallobj);
    } else {
        wallobj = nullptr;
    }

    vertices.clear();

    return success;
}

LevelLoader::LevelLoader(void) : Asset(),
_debugnode(nullptr),
_root(nullptr),
_world(nullptr),
_worldnode(nullptr),
_scale(32, 32)
{
    _bounds.size.set(1.0f, 1.0f);
}

/**
* Destroys this level, releasing all resources.
*/
LevelLoader::~LevelLoader(void) {
    clearRootNode();
}

/**
* Clears the root scene graph node for this level
*/
void LevelLoader::clearRootNode() {
    if (_root == nullptr) {
        return;
    }
    _worldnode->removeFromParent();
    _worldnode->removeAllChildren();
    _debugnode->removeFromParent();
    _debugnode->removeAllChildren();
    _worldnode = nullptr;
    _debugnode = nullptr;
    _root = nullptr;
}


/**
* Toggles whether to show the debug layer of this game world.
*
* The debug layer displays wireframe outlines of the physics fixtures.
*
* @param  flag whether to show the debug layer of this game world
*/
void LevelLoader::showDebug(bool flag) {
    if (_debugnode != nullptr) {
        _debugnode->setVisible(flag);
    }
}

/**
* Sets the scene graph node for drawing purposes.
*
* The scene graph is completely decoupled from the physics system.  The node
* does not have to be the same size as the physics body. We only guarantee
* that the node is positioned correctly according to the drawing scale.
*
* @param value  the scene graph node for drawing purposes.
*
* @retain  a reference to this scene graph node
* @release the previous scene graph node used by this object
*/
void LevelLoader::setRootNode(const std::shared_ptr<scene2::SceneNode>& node){
    if (_root != nullptr) {
        clearRootNode();
    }

    // Set content size to match the size of the game level
    _root = node;
    float xScale = (_world->getBounds().getMaxX() * _scale.x) / _root->getContentSize().width;
    float yScale = (_world->getBounds().getMaxY() * _scale.y) / _root->getContentSize().height;
    _root->setContentSize(_root->getContentSize().width * xScale, _root->getContentSize().height * yScale);
    
    _scale.set(_root->getContentSize().width/_bounds.size.width,
             _root->getContentSize().height/_bounds.size.height);

    // Create, but transfer ownership to root
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(Vec2::ZERO);
        
    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(Vec2::ZERO);
    
    _wallnode = scene2::SceneNode::alloc();
    _wallnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _wallnode->setPosition(Vec2::ZERO);
    
    _root->addChild(_worldnode);
    _root->addChild(_debugnode);
    _root->addChild(_wallnode);
    
    setBackgroundScene(_assets->get<Texture>(LEVEL_ONE_BACKGROUND));
    
    if (_goalDoor != nullptr) {
        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(_goalDoor->getTextureKey()));
        addObstacle(_goalDoor,sprite, _worldnode); // Put this at the very back
    }
    
    for(auto it = _walls.begin(); it != _walls.end(); ++it) {
        std::shared_ptr<WallModel> wall = *it;
        auto txt = _assets->get<Texture>(wall->getTextureKey());
        std::cout<< wall->getTextureKey() << std::endl;
        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(wall->getTextureKey()));
        if (wall == nullptr){
            continue;
        }
        addObstacle(wall,sprite, _wallnode);  // All walls share the same texture
    }
    
//    for(auto it = _revJoints.begin(); it != _revJoints.end(); ++it){
//        std::shared_ptr<cugl::physics2::RevoluteJoint> joint = *it;
//        if(joint == nullptr){
//            continue;
//        }
//        _world->addJoint(joint);
//    }
}

/**
 * Adds the physics object to the physics world and loosely couples it to the scene graph
 *
 * There are two ways to link a physics object to a scene graph node on the
 * screen.  One way is to make a subclass of a physics object, like we did
 * with rocket.  The other is to use callback functions to loosely couple
 * the two.  This function is an example of the latter.
 *
 *
 * @param obj    The physics object to add
 * @param node   The scene graph node to attach it to
 * @param parentnode The parent graph node to attach the node to
 */
void LevelLoader::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj, const std::shared_ptr<cugl::scene2::SceneNode>& node, const std::shared_ptr<cugl::scene2::SceneNode>& parentnode) {
    _world->addObstacle(obj);
    obj->setDebugScene(_debugnode);
    
    node->setPosition(obj->getPosition()*_scale);
    parentnode->addChild(node);
    
    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs){
            weak->setPosition(obs->getPosition()*_scale);
            weak->setAngle(obs->getAngle());
        });
    }
}

void LevelLoader::update(float dt){
    if (_wallnode != nullptr) {
        std::vector<std::shared_ptr<scene2::SceneNode>> children = _wallnode->getChildren();
        int ii = 0;
        
        for (auto it = children.begin(); it != children.end(); ++it) {
            Vec2 pos = _walls[ii]->getPosition();
            pos += _walls[ii]->getLinearVelocity()*dt;
            (*it)->setPosition(pos*_scale);
            
            float angle = _walls[ii]->getAngle();
            angle += _walls[ii]->getAngularVelocity()*dt;
            (*it)->setAngle(angle);

            ii++;
        }
    }
}
