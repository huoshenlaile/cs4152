#include "CharacterController.h"

bool CharacterController::init(const cugl::Vec2 &pos, float scale){
    _name = "TODO_PLACEHOLDER";
    _node = nullptr;
    _offset = pos;
    _drawScale = scale;
    
    return true;
}

void CharacterController::dispose(){
    _node = nullptr;
    _obstacles.clear();
}

std::shared_ptr<physics2::BoxObstacle> CharacterController::makePart(int part, int connect, const Vec2& pos) {
    std::shared_ptr<Texture> image = _textures[part];
    Size size = image->getSize();
    size.width /= _drawScale;
    size.height /= _drawScale;

    Vec2 pos2 = pos;
    if (connect != PART_NONE) {
        pos2 += _obstacles[connect]->getPosition();
    }

    std::shared_ptr<physics2::BoxObstacle> body = physics2::BoxObstacle::alloc(pos2, size);
    // log all information
    CULog("part: %d", part);
    CULog("connect: %d", connect);
    CULog("pos: %f, %f", pos.x, pos.y);
    CULog("pos2: %f, %f", pos2.x, pos2.y);
    CULog("size: %f, %f", size.width, size.height);
    body->setName(getPartName(part));
    body->setDensity(DEFAULT_DENSITY);
    
    _obstacles.push_back(body);
    return body;
}

bool CharacterController::buildParts(const std::shared_ptr<AssetManager>& assets){
    CUAssertLog(_obstacles.empty(), "Bodies are already initialized");
    
    // Get the images from the asset manager
        bool success = true;
        for(int ii = 0; ii <= PART_LH; ii++) {
            std::string name = getPartName(ii);
            std::shared_ptr<Texture> image = assets->get<Texture>(name);
            if (image == nullptr) {
                success = false;
            } else {
                _textures.push_back(image);
            }
        }
        if (!success) {
            return false;
        }
    
    return true;
}


std::string CharacterController::getPartName(int part){
    switch (part) {
        case PART_BODY:
            return BODY_TEXTURE;
        case PART_RH:
        case PART_LH:
            return HAND_TEXTURE;
        case PART_JR1:
        case PART_JR2:
        case PART_JR3:
        case PART_JR4:
        case PART_JR5:
        case PART_LR1:
        case PART_LR2:
        case PART_LR3:
        case PART_LR4:
        case PART_LR5:
            return CJOINT_TEXTURE;
        default:
            return "UNKNOWN";
    }
}
