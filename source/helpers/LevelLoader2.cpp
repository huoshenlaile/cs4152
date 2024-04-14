#include "LevelLoader2.h"

bool LevelLoader2::preload(const std::shared_ptr<cugl::JsonValue>& json) {
    if (json == nullptr) {
        CUAssertLog(false, "Failed to load level file");
        return false;
    }

    _json = json;

    float w = json->get(WIDTH_FIELD)->asFloat();
    float h = json->get(HEIGHT_FIELD)->asFloat();
    _bounds.size.set(w, h);
    _gravity.set(0,-90.0f); // TODO: make this a field in the json file
    _world = cugl::physics2::net::NetWorld::alloc(getBounds(),Vec2(0,-90.f));
    _worldnode = cugl::scene2::SceneNode::alloc();
    _uinode = cugl::scene2::SceneNode::alloc();
    _levelCompletenode = cugl::scene2::SceneNode::alloc();

    // other allocs

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

bool LevelLoader2::loadObject(const std::shared_ptr<JsonValue>& json){
    auto type = json->get("type")->asString();
    if (type == WALLS_FIELD){
        auto wall = Interactable::alloc(json, _scale);
        _interactables.push_back(wall);
    }else if (type == GOALDOOR_FIELD){
        auto door = Exitdoot::alloc(json, _scale);
        _interactables.push_back(std::static_pointer_cast<Interactable>(door));
    }else if (type == SENSOR_FIELD){
        auto sensor = Sensor::alloc(json, _scale);
        _interactables.push_back(std::static_pointer_cast<Interactable>(sensor));
    }else if (type == PAINT_FIELD){
        auto paint = GrowingPaint::alloc(json, _scale);
        _interactables.push_back(std::static_pointer_cast<Interactable>(paint));
    } else if (type == CHARACTER_POS){
        _charPos = getObjectPos(json);
    } else {
        CUAssertLog(false, "Object type not recognized");
        return false;
    }
    return true;
}

bool LevelLoader2::construct(std::shared_ptr<cugl::AssetManager>& _assets){
    Size dimen = computeActiveSize();
//    _scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width : dimen.height / rect.size.height;
    Vec2 offset{ (dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f };

    // ============== construct scene ==============
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);
    
    // DONT KNOW WHY... ALL COPIED FROM THE old LEVELLOADER
    float xScale = (_world->getBounds().getMaxX() * _scale.x) / _worldnode->getContentSize().width;
    float yScale = (_world->getBounds().getMaxY() * _scale.y) / _worldnode->getContentSize().height;
    _worldnode->setContentSize(_worldnode->getContentSize().width * xScale, _worldnode->getContentSize().height * yScale);
    _scale.set(_worldnode->getContentSize().width / _bounds.size.width, _worldnode->getContentSize().height / _bounds.size.height);

    // create scene node for character and platform, bind them to world node
    _charNode = cugl::scene2::SceneNode::alloc();
    _platformNode = cugl::scene2::SceneNode::alloc();
    _platformNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _platformNode->setPosition(Vec2::ZERO);
    _worldnode->addChild(_platformNode);
    _worldnode->addChild(_charNode);

    // create character
    _character = CharacterController::alloc(_charPos, _scale);
    _character->buildParts(_assets);
    _character->createJoints();
    _character->linkPartsToWorld(_world, _charNode, _scale);

    // add interactables to world
    for (auto inter : _interactables){
        inter->bindAssets(_assets);
        inter->linkToWorld(_world, _platformNode, _scale);
    }
    return true;
}


void LevelLoader2::unload() {
    // TODO: 
}


// ========================================= private helper functions =========================================
Size LevelLoader2::computeActiveSize() const {
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width / dimen.height;
    float ratio2 = ((float)SCENE_WIDTH) / ((float)SCENE_HEIGHT);
    if (ratio1 < ratio2) {
        dimen *= SCENE_WIDTH / dimen.width;
    }
    else {
        dimen *= SCENE_HEIGHT / dimen.height;
    }
    return dimen;
}

Vec2 LevelLoader2::getObjectPos(const std::shared_ptr<JsonValue>& json) {
    Vec2 pos = Vec2(json->getFloat("x") / _scale.x, ((_bounds.getMaxY() * _scale.y) - json->getFloat("y") + _scale.y) / _scale.y);
    return pos;
}
