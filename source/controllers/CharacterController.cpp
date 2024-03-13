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
    
    Vec2 pos = _offset;
    CULog("CharacterController buildParts _offset: %f, %f",_offset.x, _offset.y);
    makePart(PART_BODY, PART_NONE, pos);
    makePart(PART_JR1, PART_BODY, Vec2(CJOINT_OFFSET, 0));
    makePart(PART_JR2, PART_JR1, Vec2(CJOINT_OFFSET, 0));
    makePart(PART_JR3, PART_JR2, Vec2(CJOINT_OFFSET, 0));
    makePart(PART_JR4, PART_JR3, Vec2(0, 0));
    makePart(PART_JR5, PART_JR4, Vec2(CJOINT_OFFSET, 0));
    makePart(PART_RH, PART_JR4, Vec2(CJOINT_OFFSET, 0));
    
    makePart(PART_LR1, PART_BODY, Vec2(-CJOINT_OFFSET, 0));
    makePart(PART_LR2, PART_LR1, Vec2(-CJOINT_OFFSET, 0));
    makePart(PART_LR3, PART_LR2, Vec2(-CJOINT_OFFSET, 0));
    makePart(PART_LR4, PART_LR3, Vec2(0, 0));
    makePart(PART_LR5, PART_LR4, Vec2(-CJOINT_OFFSET, 0));
    makePart(PART_LH, PART_LR5, Vec2(-CJOINT_OFFSET, 0));
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

bool CharacterController::createJoints(){
    std::shared_ptr<physics2::RevoluteJoint> revjoint;
    std::shared_ptr<physics2::MotorJoint> motjoint;
    std::shared_ptr<physics2::PrismaticJoint> prijoint;
    std::shared_ptr<physics2::DistanceJoint> distjoint;
    std::shared_ptr<physics2::WeldJoint> weldjoint;

    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY],_obstacles[PART_RH]);
    motjoint->setMaxForce(10000.0f);
    motjoint->setMaxTorque(1000.0f);
    motjoint->setAngularOffset(0);
    motjoint->setLinearOffset(4,-1);
    _joints.push_back(motjoint);
    rightHandJoint = motjoint;

    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY],_obstacles[PART_LH]);
    motjoint->setMaxForce(10000.0f);
    motjoint->setMaxTorque(1000.0f);
    motjoint->setAngularOffset(0);
    motjoint->setLinearOffset(-4,-1);
    _joints.push_back(motjoint);
    leftHandJoint = motjoint;
    return true;


    // right arm, forearm, hand
    // BODY->JR1 rev
    revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_BODY],_obstacles[PART_JR1]);
    revjoint->setLocalAnchorA(CJOINT_OFFSET, 0);
    revjoint->setLocalAnchorB(0, 0);
    revjoint->enableLimit(true);
    revjoint->setUpperAngle(M_PI / 2.0f);
    revjoint->setLowerAngle(-M_PI / 2.0f);
    _joints.push_back(revjoint);
    
    // JR1->JR2 pri
    prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR1],_obstacles[PART_JR2]);
    prijoint->setLocalAnchorA(0, 0);
    prijoint->setLocalAnchorB(0, 0);
    prijoint->setLocalAxisA(1, 0);
    prijoint->enableLimit(true);
    prijoint->setUpperTranslation(1.5f * CJOINT_OFFSET);
    prijoint->setLowerTranslation(0.2f * CJOINT_OFFSET);
    _joints.push_back(prijoint);

    // JR2->JR3 pri
    prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR2],_obstacles[PART_JR3]);
    prijoint->setLocalAnchorA(0, 0);
    prijoint->setLocalAnchorB(0, 0);
    prijoint->setLocalAxisA(1, 0);
    prijoint->enableLimit(true);
    prijoint->setUpperTranslation(1.5f * CJOINT_OFFSET);
    prijoint->setLowerTranslation(0.2f * CJOINT_OFFSET);
    _joints.push_back(prijoint);

    // JR3->JR4 rev, both anchor is 0,0
    revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_JR3],_obstacles[PART_JR4]);
    revjoint->setLocalAnchorA(0, 0);
    revjoint->setLocalAnchorB(0, 0);
    revjoint->enableLimit(true);
    revjoint->setUpperAngle(M_PI / 2.0f);
    revjoint->setLowerAngle(-M_PI / 2.0f);
    _joints.push_back(revjoint);

    // JR4->JR5 pri
    prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR4],_obstacles[PART_JR5]);
    prijoint->setLocalAnchorA(0, 0);
    prijoint->setLocalAnchorB(0, 0);
    prijoint->setLocalAxisA(1, 0);
    prijoint->enableLimit(true);
    prijoint->setUpperTranslation(1.5f * CJOINT_OFFSET);
    prijoint->setLowerTranslation(0.2f * CJOINT_OFFSET);
    _joints.push_back(prijoint);

    // JR5->RH pri
    prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR5],_obstacles[PART_RH]);
    prijoint->setLocalAnchorA(0, 0);
    prijoint->setLocalAnchorB(0, 0);
    prijoint->setLocalAxisA(1, 0);
    prijoint->enableLimit(true);
    prijoint->setUpperTranslation(1.5f * CJOINT_OFFSET);
    prijoint->setLowerTranslation(0.2f * CJOINT_OFFSET);
    _joints.push_back(prijoint);

    // left part
    // body->LR1 rev
    revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_BODY],_obstacles[PART_LR1]);
    revjoint->setLocalAnchorA(-CJOINT_OFFSET, 0);
    revjoint->setLocalAnchorB(0, 0);
    revjoint->enableLimit(true);
    revjoint->setUpperAngle(M_PI / 2.0f);
    revjoint->setLowerAngle(-M_PI / 2.0f);
    _joints.push_back(revjoint);

    // LR1->LR2 pri
    prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR1],_obstacles[PART_LR2]);
    prijoint->setLocalAnchorA(0, 0);
    prijoint->setLocalAnchorB(0, 0);
    prijoint->setLocalAxisA(-1, 0);
    prijoint->enableLimit(true);
    prijoint->setUpperTranslation(1.5f * CJOINT_OFFSET);
    prijoint->setLowerTranslation(0.2f * CJOINT_OFFSET);
    _joints.push_back(prijoint);

    // LR2->LR3 pri
    prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR2],_obstacles[PART_LR3]);
    prijoint->setLocalAnchorA(0, 0);
    prijoint->setLocalAnchorB(0, 0);
    prijoint->setLocalAxisA(-1, 0);
    prijoint->enableLimit(true);
    prijoint->setUpperTranslation(1.5f * CJOINT_OFFSET);
    prijoint->setLowerTranslation(0.2f * CJOINT_OFFSET);
    _joints.push_back(prijoint);

    // LR3->LR4 rev, both anchor is 0,0
    revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_LR3],_obstacles[PART_LR4]);
    revjoint->setLocalAnchorA(0, 0);
    revjoint->setLocalAnchorB(0, 0);
    revjoint->enableLimit(true);
    revjoint->setUpperAngle(M_PI / 2.0f);
    revjoint->setLowerAngle(-M_PI / 2.0f);
    _joints.push_back(revjoint);

    // LR4->LR5 pri
    prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR4],_obstacles[PART_LR5]);
    prijoint->setLocalAnchorA(0, 0);
    prijoint->setLocalAnchorB(0, 0);
    prijoint->setLocalAxisA(-1, 0);
    prijoint->enableLimit(true);
    prijoint->setUpperTranslation(1.5f * CJOINT_OFFSET);
    prijoint->setLowerTranslation(0.2f * CJOINT_OFFSET);
    _joints.push_back(prijoint);

    // right hand
    // JR5->RH pri
    prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR5],_obstacles[PART_LH]);
    prijoint->setLocalAnchorA(0, 0);
    prijoint->setLocalAnchorB(0, 0);
    prijoint->setLocalAxisA(-1, 0);
    prijoint->enableLimit(true);
    prijoint->setUpperTranslation(1.5f * CJOINT_OFFSET);
    prijoint->setLowerTranslation(0.2f * CJOINT_OFFSET);
    _joints.push_back(prijoint);

    // now create motor joints.

    // BODY->JR1 revolute motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY],_obstacles[PART_JR1]);
    motjoint->setMaxTorque(1000.0f);
    motjoint->setAngularOffset(0);
    _joints.push_back(motjoint);

    // JR1->JR2 prismatic motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_JR1],_obstacles[PART_JR2]);
    motjoint->setMaxForce(1000.0f);
    motjoint->setLinearOffset(1.0f, 0);
    _joints.push_back(motjoint);

    // JR2->JR3 prismatic motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_JR2],_obstacles[PART_JR3]);
    motjoint->setMaxForce(1000.0f);
    motjoint->setLinearOffset(1.0f, 0);
    _joints.push_back(motjoint);

    // JR3->JR4 revolute motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_JR3],_obstacles[PART_JR4]);
    motjoint->setMaxTorque(1000.0f);
    motjoint->setAngularOffset(0);
    _joints.push_back(motjoint);

    // JR4->JR5 prismatic motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_JR4],_obstacles[PART_JR5]);
    motjoint->setMaxForce(1000.0f);
    motjoint->setLinearOffset(1.0f, 0);
    _joints.push_back(motjoint);

    // JR5->RH prismatic motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_JR5],_obstacles[PART_RH]);
    motjoint->setMaxForce(1000.0f);
    motjoint->setLinearOffset(1.0f, 0);
    _joints.push_back(motjoint);

    // left part
    // BODY->LR1 revolute motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY],_obstacles[PART_LR1]);
    motjoint->setMaxTorque(1000.0f);
    motjoint->setAngularOffset(0);
    _joints.push_back(motjoint);

    // LR1->LR2 prismatic motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_LR1],_obstacles[PART_LR2]);
    motjoint->setMaxForce(1000.0f);
    motjoint->setLinearOffset(-1.0f, 0);
    _joints.push_back(motjoint);

    // LR2->LR3 prismatic motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_LR2],_obstacles[PART_LR3]);
    motjoint->setMaxForce(1000.0f);
    motjoint->setLinearOffset(-1.0f, 0);
    _joints.push_back(motjoint);

    // LR3->LR4 revolute motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_LR3],_obstacles[PART_LR4]);
    motjoint->setMaxTorque(1000.0f);
    motjoint->setAngularOffset(0);
    _joints.push_back(motjoint);

    // LR4->LR5 prismatic motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_LR4],_obstacles[PART_LR5]);
    motjoint->setMaxForce(1000.0f);
    motjoint->setLinearOffset(-1.0f, 0);
    _joints.push_back(motjoint);

    // LR5->LH prismatic motor joint
    motjoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_LR5],_obstacles[PART_LH]);
    motjoint->setMaxForce(1000.0f);
    motjoint->setLinearOffset(-1.0f, 0);
    _joints.push_back(motjoint);

    
    



    return true;
}


void CharacterController::setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node){
	_node = node;
	for (int ii = 0; ii <= PART_LH; ii++) {
        std::shared_ptr<Texture> image = _textures[ii];
		std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
//		if (ii == PART_RIGHT_ARM || ii == PART_RIGHT_FOREARM) {
//			sprite->flipHorizontal(true); // More reliable than rotating 90 degrees.
//		}
		_node->addChild(sprite);
	}

}

void CharacterController::setDrawScale(float scale){
    _drawScale = scale;
}

void CharacterController::activate(const std::shared_ptr<cugl::physics2::net::NetWorld>& world) {
    for(auto it = _obstacles.begin(); it != _obstacles.end(); ++it) {
        world->addObstacle(*it);
    }
    for(auto it = _joints.begin(); it != _joints.end(); ++it) {
        world->addJoint(*it);
    }
}

void CharacterController::deactivate(const std::shared_ptr<cugl::physics2::net::NetWorld>& world) {
    for(auto it = _obstacles.begin(); it != _obstacles.end(); ++it) {
        world->removeObstacle(*it);
    }
    for(auto it = _joints.begin(); it != _joints.end(); ++it) {
        world->removeJoint(*it);
    }
}


bool CharacterController::moveRightHand(cugl::Vec2 offset){
    float a = _obstacles[PART_BODY]->getAngle();
    leftHandOffset = Vec2(leftHandOffset.x * cos(a) - leftHandOffset.y * sin(a), leftHandOffset.x * sin(a) + leftHandOffset.y * cos(a));
    leftHandJoint->setLinearOffset(leftHandOffset);
    return true;
}


bool CharacterController::moveLeftHand(cugl::Vec2 offset){
    float a = _obstacles[PART_BODY]->getAngle();
    rightHandOffset = Vec2(rightHandOffset.x * cos(a) - rightHandOffset.y * sin(a), rightHandOffset.x * sin(a) + rightHandOffset.y * cos(a));
    rightHandJoint->setLinearOffset(rightHandOffset);
    
    return true;
}
