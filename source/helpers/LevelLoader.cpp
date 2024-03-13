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
bool LevelLoader:: preload(const std::shared_ptr<cugl::JsonValue>& json) {
    if (json == nullptr) {
        CUAssertLog(false, "Failed to load level file");
        return false;
    }
    // Initial geometry
    float w = json->get(WIDTH_FIELD)->asFloat();
    float h = json->get(HEIGHT_FIELD)->asFloat();
    float g = json->get("properties")->get(0)->getFloat("value", -4.9);
    _bounds.size.set(w, h);
    _gravity.set(0,g);
    /** Create the physics world */
    // _world = cugl::physics2::net::NetWorld::alloc(getBounds(),getGravity());

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
    }
    return false;
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
    
    _root->addChild(_worldnode);
    _root->addChild(_debugnode);
    
    for(auto it = _walls.begin(); it != _walls.end(); ++it) {
        std::shared_ptr<WallModel> wall = *it;
        auto sprite = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(wall->getTextureKey()),wall->getPolygon() * _scale);
        if (wall == nullptr){
            continue;
        }
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
}
