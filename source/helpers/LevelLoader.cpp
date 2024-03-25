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
    std::cout << "Loading file " << file << "\n";;
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
    }else if (type == SENSOR_FIELD){
        std::cout << "Loading sensor\n";
        return loadSensor(json);
    }
    return false;
}

void LevelLoader::setBackgroundScene(){
    auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(ALPHA_RELEASE_BACKGROUND));
    sprite->setAbsolute(true);
    sprite->setPosition(-10.0f, 0.0f);
    sprite->setContentSize(sprite->getContentSize().width*5, sprite->getContentSize().height*4);
//    auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(alpharelease_background), Rect(-100.0f,-100.0f,20000.0f, 20000.0f)); //TODO: don't hard code
//    sprite->setPosition(Vec2(0.0f,0.0f));
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
        _goalDoor->setName(json->getString("name"));

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

bool LevelLoader::loadSensor(const std::shared_ptr<JsonValue>& json){
    bool success = false;
    auto sensor_json = json->get("properties")->get(0)->get("value");
    if (sensor_json != nullptr) {
        bool success = true;
        
        int polysize = (int)json->get(VERTICES_FIELD)->children().size();
        success = success && polysize > 0;
        
        std::vector<float> vertices = getVertices(json);
        success = success && 2*polysize == vertices.size();
        
        Vec2* verts = reinterpret_cast<Vec2*>(&vertices[0]);
        Poly2 sensor(verts,(int)vertices.size()/2);
        EarclipTriangulator triangulator;
        triangulator.set(sensor.vertices);
        triangulator.calculate();
        sensor.setIndices(triangulator.getTriangulation());
        triangulator.clear();
        
        // Get the object, which is automatically retained
        std::shared_ptr<SensorModel> sensorobj = SensorModel::alloc(sensor);
        sensorobj->setName(json->getString("name"));
        
        sensorobj->setBodyType((b2BodyType)sensor_json->get("obstacle")->getInt(BODYTYPE_FIELD));
        
        sensorobj->setDensity(sensor_json->get("obstacle")->getDouble(DENSITY_FIELD));
        sensorobj->setFriction(sensor_json->get("obstacle")->getDouble(FRICTION_FIELD));
        sensorobj->setRestitution(sensor_json->get("obstacle")->getDouble(RESTITUTION_FIELD));
        
        // Set the texture value
        success = success && sensor_json->get("obstacle")->get(TEXTURE_FIELD)->isString();
        sensorobj->setTextureKey(sensor_json->get("obstacle")->getString(TEXTURE_FIELD));
        sensorobj->setDebugColor(parseColor(sensor_json->get("obstacle")->getString(DEBUG_COLOR_FIELD)));
        
        if (success) {
            _sensors.push_back(sensorobj);
        } else {
            sensorobj = nullptr;
        }
        
        vertices.clear();
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
//    std::cout << "loading wall " << std::endl;
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
    
    if(json->getFloat("rotation") != 0.0f){
        printf("inside");
//        wallobj->setAngle((-1.0f*json->getFloat("rotation")+90.0f) % 360.0f);
    }
//    wallobj->setAngle(json->getFloat("rotation"));
    // Set the texture value
    success = success && walljson->get("obstacle")->get(TEXTURE_FIELD)->isString();
    wallobj->setTextureKey(walljson->get("obstacle")->getString(TEXTURE_FIELD));
    wallobj->setDebugColor(parseColor(walljson->get("obstacle")->getString(DEBUG_COLOR_FIELD)));

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
    unload();
    clearRootNode();
}

/**
* Unloads this game level, releasing all sources
*
* This load method should NEVER access the AssetManager.  Assets are loaded and
* unloaded in parallel, not in sequence.  If an asset (like a game level) has
* references to other assets, then these should be disconnected earlier.
*/
void LevelLoader::unload() {
    if (_goalDoor != nullptr) {
        if (_world != nullptr) {
            _world->removeObstacle(_goalDoor);
        }
        _goalDoor = nullptr;
    }
    for(auto it = _walls.begin(); it != _walls.end(); ++it) {
        if (_world != nullptr) {
            _world->removeObstacle((*it));
        }
    (*it) = nullptr;
    }
    _walls.clear();
    for(auto it = _sensors.begin(); it != _sensors.end(); ++it) {
        if (_world != nullptr) {
            _world->removeObstacle((*it));
        }
    (*it) = nullptr;
    }
    _sensors.clear();
    if (_world != nullptr) {
        _world->clear();
        _world = nullptr;
    }
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
    
    _root->addChild(_worldnode);
    _root->addChild(_debugnode);
    
    setBackgroundScene();
    
    if (_goalDoor != nullptr) {
        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(_goalDoor->getTextureKey()));
        addObstacle(_goalDoor,sprite); // Put this at the very back
    }
    for(auto it = _sensors.begin(); it != _sensors.end(); ++it) {
        std::shared_ptr<SensorModel> sensor = *it;
        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(sensor->getTextureKey()));
        addObstacle(sensor,sprite); // Put this at the very back
    }
    for(auto it = _walls.begin(); it != _walls.end(); ++it) {
        std::shared_ptr<WallModel> wall = *it;
        auto txt = _assets->get<Texture>(wall->getTextureKey());
//        std::cout<< wall->getTextureKey() << std::endl;
        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(wall->getTextureKey()));
        sprite->setContentSize(wall->getWidth()*_scale.x, wall->getHeight()*_scale.y);
        sprite->setAngle(wall->getAngle());
//        sprite->setAngle(-1.0f * wall->getAngle());
        
//        sprite->setAngle(90.0f);
        if (wall == nullptr){
            continue;
        }
//        std::cout << "drawing wall and adding to worlds" << std::endl;
        addObstacle(wall,sprite);  // All walls share the same texture
    }
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
 */
void LevelLoader::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj, const std::shared_ptr<cugl::scene2::SceneNode>& node) {
    _world->addObstacle(obj);
    obj->setDebugScene(_debugnode);
    
    // Position the scene graph node (enough for static objects)
    node->setPosition(obj->getPosition()*_scale);
    _worldnode->addChild(node);
    
    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs){
            weak->setPosition(obs->getPosition()*_scale);
            weak->setAngle(obs->getAngle());
        });
    }
    if (obj->getName() == "sensor1"){
        std::cout << "x: " << obj->getCentroid().x << " y:" << obj->getCentroid().y << "\n";
        std::cout << "X: " << obj->getX() << " Y: " << obj->getY() << "\n";
    }
//    std::cout<<"success"<<std::endl;
}
