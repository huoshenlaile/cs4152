#include "Levelloader2.h"

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

    return true;
}

bool LevelLoader2::constructRenderScene(std::shared_ptr<cugl::AssetManager>& _assets){
    Size dimen = computeActiveSize();
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width : dimen.height / rect.size.height;
    Vec2 offset{ (dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f };

    // ============== construct scene ==============
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);

}

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