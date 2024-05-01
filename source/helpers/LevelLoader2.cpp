#include "LevelLoader2.h"

bool LevelLoader2::preload(const std::shared_ptr<cugl::JsonValue> &json) {
    if (json == nullptr) {
        CUAssertLog(false, "Failed to load level file");
        return false;
    }

    _json = json;

    float w = json->get(WIDTH_FIELD)->asFloat();
    float h = json->get(HEIGHT_FIELD)->asFloat();
    _bounds.size.set(w, h);
    _gravity.set(0, -90.0f); // TODO: make this a field in the json file
    _world = cugl::physics2::net::NetWorld::alloc(getBounds(), Vec2(0, -90.f));
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

            // load background name
            if (objects->get(j)->getString("name") == "METADATA") {
                auto props = objects->get(j)->get("properties");
                for (auto prop : props->children()) {
                    if (prop->getString("name") == "METADATA") {
                        goodBg = prop->get("value")->getString("Good_bg");
                        badBg = prop->get("value")->getString("Bad_bg");
                        defaultBg = prop->get("value")->getString("Default_bg");
                        levelWidth = prop->get("value")->getInt("level_width");
                        levelHeight = prop->get("value")->getInt("level_height");
                        if (defaultBg == "") {
                            CULog("ERROR: Background not loaded! Look at your METADATA!");
                        }
                    }
                }
            }
        }
    }

    // TODO: get the background name from the json file
    // _background_name = ???
    return true;
}

bool LevelLoader2::loadObject(const std::shared_ptr<JsonValue> &json) {
    auto type = json->get("type")->asString();
    std::cout << "===== PARSING " << json->get("name")->asString() << " ======" << std::endl;
    if (type == WALLS_FIELD) {
        auto wall = Wall::alloc(json, _scale, _bounds);
        _interactables.push_back(wall);
    } else if (type == SENSOR_FIELD) {
        auto sensor = Sensor::alloc(json, _scale, _bounds);
        _interactables.push_back(std::static_pointer_cast<Interactable>(sensor));
    } else if (type == EXIT_FIELD) {
        auto sensor = Exit::alloc(json, _scale, _bounds);
        _interactables.push_back(std::static_pointer_cast<Interactable>(sensor));
    } else if (type == PAINT_FIELD) {
        auto paint = GrowingPaint::alloc(json, _scale, _bounds);
        _interactables.push_back(std::static_pointer_cast<Interactable>(paint));
    } else if (type == MWALLS_FIELD) {
        auto mwall = MovingWall::alloc(json, _scale, _bounds);
        _interactables.push_back(std::static_pointer_cast<Interactable>(mwall));
    } else if (type == GRAVITYWALL_FIELD) {
        auto gravitywall = GravityReversePlatform::alloc(json, _scale, _bounds);
        _interactables.push_back(std::static_pointer_cast<Interactable>(gravitywall));
    } else if (type == CHARACTER_POS) {
        _charPos = getObjectPos(json);
    } else if (type == BOUNCY_WALL_FIELD) {
        auto mwall = BouncyWall::alloc(json, _scale, _bounds);
        _interactables.push_back(std::static_pointer_cast<Interactable>(mwall));
    } else if (type == DRIPPAINT_FIELD) {
        auto drip = DrippingPaint::alloc(json, _scale, _bounds);
        _interactables.push_back(std::static_pointer_cast<Interactable>(drip));
    } else if (type == UI_FIELD) {
        auto int_ui = InteractableUI::alloc(json, _scale, _bounds);
        _interactables.push_back(std::static_pointer_cast<Interactable>(int_ui));
    }
    else {
        // log type not recognized
        // CUAssertLog(false, "Object type not recognized");
        CULog("unknown type: %s", type.c_str());
        return false;
    }
    return true;
}

bool LevelLoader2::construct(std::shared_ptr<cugl::AssetManager> &_assets) {
    Size dimen = computeActiveSize();
    //    _scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width : dimen.height / rect.size.height;
    Vec2 offset{(dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f};

    // ============== construct scene ==============
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);
    _worldnode->setContentSize(Size(SCENE_WIDTH, SCENE_HEIGHT));

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
    _platformNode->setVisible(true);

    // set background
    defaultBgTexture = _assets->get<Texture>(defaultBg);
    goodBgTexture = _assets->get<Texture>(goodBg);
    badBgTexture = _assets->get<Texture>(badBg);
    std::cout << "Default Background in LevelLoader2::Construct: " << defaultBg << std::endl;
    _defaultBgNode = scene2::PolygonNode::allocWithTexture(defaultBgTexture);
    _defaultBgNode->setTexture(defaultBgTexture);
    _defaultBgNode->setAbsolute(true);
    _defaultBgNode->setPosition(-10.0f, 0.0f);
    std::cout << "level width and height in LevelLoader2::Construct: (" << levelWidth << ", " << levelHeight << ")" << "\n";
    _defaultBgNode->setContentSize(levelWidth, levelHeight);

    CULog("level loader construction scale: %f %f", _scale.x, _scale.y);
    // create character
    _character = CharacterController::alloc(_charPos, _scale.x);
    _character->buildParts(_assets);
    _character->createJoints();
    _character->linkPartsToWorld(_world, _charNode, _scale.x);

    // add interactables to world
    for (auto inter : _interactables) {
        inter->bindAssets(_assets, _scale);
        inter->linkToWorld(_world, _platformNode, _scale.x);
        inter->setup();
    }

    _worldnode->addChild(_defaultBgNode);
    _worldnode->addChild(_platformNode);
    _worldnode->addChild(_charNode);

    return true;
}

void LevelLoader2::unload() {
    _json = nullptr;
    _root = nullptr;
    _worldnode = nullptr;
    _charNode = nullptr;
    _platformNode = nullptr;
    _uinode = nullptr;
    _levelCompletenode = nullptr;
    _world = nullptr;
    _character = nullptr;
    _interactables.clear();
}

// ========================================= private helper functions =========================================
Size LevelLoader2::computeActiveSize() const {
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width / dimen.height;
    float ratio2 = ((float)SCENE_WIDTH) / ((float)SCENE_HEIGHT);
    if (ratio1 < ratio2) {
        dimen *= SCENE_WIDTH / dimen.width;
    } else {
        dimen *= SCENE_HEIGHT / dimen.height;
    }
    return dimen;
}

Vec2 LevelLoader2::getObjectPos(const std::shared_ptr<JsonValue> &json) {
    Vec2 pos = Vec2(json->getFloat("x") / _scale.x, ((_bounds.getMaxY() * _scale.y) - json->getFloat("y") + _scale.y) / _scale.y);
    return pos;
}

void LevelLoader2::changeBackground(int defaultGoodorBad){
    if (defaultGoodorBad == -1) {
        _defaultBgNode -> setTexture(defaultBgTexture);
    } else if (defaultGoodorBad == 0) {
        _defaultBgNode -> setTexture(goodBgTexture);
    } else if (defaultGoodorBad == 1) {
        _defaultBgNode -> setTexture(badBgTexture);
    }
//    _root->setPosition(-100.0f, 0.0f);
    _defaultBgNode->setAbsolute(true);
    _defaultBgNode->setPosition(-10.0f, 0.0f);
    _defaultBgNode->setContentSize(_defaultBgNode->getContentSize().width, _defaultBgNode->getContentSize().height);
    
    return;
}
