#include "CharacterController.h"

bool CharacterController::init(const cugl::Vec2& pos, float scale) {
	_name = "TODO_PLACEHOLDER";
	_node = nullptr;
	_offset = pos;
	_drawScale = scale;

	return true;
}

CharacterController::CharacterController(void) : _drawScale(0){
}
/**
* Destroys this level, releasing all resources.
*/
CharacterController::~CharacterController(void) {
    //nothing for now, adding dispose causes an error: resetting the map for the second time, the game would crash because the joint to remove did not exist
    //This may need to be fixed in the future
//    dispose();
}

void CharacterController::dispose() {
    printf("removing joints");
    for(auto it = _joints.begin(); it != _joints.end(); ++it) {
        if (_world != nullptr) {
            _world->removeJoint(*it);
        }
    (*it) = nullptr;
    }
    
    for(auto it = _obstacles.begin(); it != _obstacles.end(); ++it) {
        if (_world != nullptr) {
            _world->removeObstacle((*it));
        }
    (*it) = nullptr;
    }
    _node->removeAllChildren();
	_obstacles.clear();
    _lineNodes.clear();
	_joints.clear();
}

std::shared_ptr<physics2::Obstacle> CharacterController::makePart(int part, int connect, const Vec2& pos) {
	std::shared_ptr<Texture> image = _textures[part];
	Size size = image->getSize();
	size.width /= _drawScale;
	size.height /= _drawScale;
	
	Vec2 pos2 = pos;
	if (connect != PART_NONE) {
		pos2 += _obstacles[connect]->getPosition();
	}

	// std::shared_ptr<physics2::BoxObstacle> body = physics2::BoxObstacle::alloc(pos2, size);
	// create body  polygon as octagon
	// calculate index first
	std::shared_ptr<physics2::Obstacle> body;
	
	if (part == PART_BODY){
		body = physics2::BoxObstacle::alloc(pos2, size);
	}else{
	float w = size.width / 2.0f;
	float h = size.height / 2.0f;
	float oct[] = {	- w, - h * 0.414f,
					- w * 0.414f, - h,
					w * 0.414f, - h,
					w, - h * 0.414f,
					w, h * 0.414f,
					w * 0.414f, h,
					- w * 0.414f, h,
					- w, h * 0.414f};
	

	cugl::Poly2 octpoly = cugl::Poly2(reinterpret_cast<Vec2*>(oct), 8);
	EarclipTriangulator triangulator;
	triangulator.set(octpoly.vertices);
	triangulator.calculate();
	octpoly.setIndices(triangulator.getTriangulation());
	triangulator.clear();

	body = physics2::PolygonObstacle::allocWithAnchor(octpoly, Vec2(0.5,0.5));
	body->setPosition(pos2);
	}
	body->setBodyType(b2_dynamicBody);
	
	// log all information
	//CULog("part: %d", part);
	//CULog("connect: %d", connect);
	//CULog("pos: %f, %f", pos.x, pos.y);
	//CULog("pos2: %f, %f", pos2.x, pos2.y);
	//CULog("size: %f, %f", size.width, size.height);
	body->setName(getPartName(part));
	if (part == PART_BODY) {
		body->setDensity(BODY_DENSITY);
		// CULog("makePart body centroid position: %f, %f", body->getCentroid().x, body->getCentroid().y);
	}
	else if (part == PART_RH || part == PART_LH) {
		body->setDensity(HAND_DENSITY);
	} else
	{
		body->setDensity(DEFAULT_DENSITY);
	}
	// add extra friction to the PART_LH and PART_RH
	// set always upward direction
    if (part == PART_LH || part == PART_RH) {
		
		body->setFriction(HAND_FRICTION);
		body->setFixedRotation(true);
	}else{
		body->setFriction(0.5f);
	}

	// log friction information:
   	CULog("makePart body part %d friction: %f", part, body->getFriction());
	
	
	// log parts and resitituion
   	CULog("makePart body part %d restitution: %f", part, body->getRestitution());
	_obstacles.push_back(body);
	return body;
}

bool CharacterController::buildParts(const std::shared_ptr<AssetManager>& assets) {
	CUAssertLog(_obstacles.empty(), "Bodies are already initialized");

	// Get the images from the asset manager
	bool success = true;
	for (int ii = 0; ii <= PART_LH; ii++) {
		std::string name = getPartName(ii);
		std::shared_ptr<Texture> image = assets->get<Texture>(name);
		if (image == nullptr) {
			success = false;
		}
		else {
			_textures.push_back(image);
		}
	}
	if (!success) {
		return false;
	}

	Vec2 pos = _offset;
	CULog("CharacterController buildParts _offset: %f, %f", _offset.x, _offset.y);
	makePart(PART_BODY, PART_NONE, pos);
	// makePart(PART_JR1, PART_BODY, Vec2(CJOINT_OFFSET, 0));
	// makePart(PART_JR2, PART_JR1, Vec2(HALF_CJOINT_OFFSET, 0));
	// makePart(PART_JR3, PART_JR2, Vec2(HALF_CJOINT_OFFSET, 0));
	// makePart(PART_JR4, PART_JR3, Vec2(0, 0));
	// makePart(PART_JR5, PART_JR4, Vec2(HALF_CJOINT_OFFSET, 0));
	// makePart(PART_JR6, PART_JR5, Vec2(HALF_CJOINT_OFFSET, 0));
	// makePart(PART_RH, PART_JR6, Vec2(0, 0));

	// makePart(PART_LR1, PART_BODY, Vec2(-CJOINT_OFFSET, 0));
	// makePart(PART_LR2, PART_LR1, Vec2(-HALF_CJOINT_OFFSET, 0));
	// makePart(PART_LR3, PART_LR2, Vec2(-HALF_CJOINT_OFFSET, 0));
	// makePart(PART_LR4, PART_LR3, Vec2(0, 0));
	// makePart(PART_LR5, PART_LR4, Vec2(-HALF_CJOINT_OFFSET, 0));
	// makePart(PART_LR6, PART_LR5, Vec2(-HALF_CJOINT_OFFSET, 0));
	// makePart(PART_LH, PART_LR6, Vec2(0, 0));


	makePart(PART_RH, PART_BODY, Vec2(4 * HALF_CJOINT_OFFSET, 0));
	makePart(PART_LH, PART_BODY, Vec2(-4 * HALF_CJOINT_OFFSET , 0));


	leftShoulderOffset = Vec2(-CJOINT_OFFSET, 0);
	rightShoulderOffset = Vec2(CJOINT_OFFSET, 0);
	float full_length = 4 * HALF_CJOINT_OFFSET;
	leftHandOffset = Vec2(-full_length, 0);
	rightHandOffset = Vec2(full_length, 0);
	leftElbowOffset = leftHandOffset / 2.0f;
	rightElbowOffset = rightHandOffset / 2.0f;

	// _obstacles[PART_JR1]->setSensor(true);
	// _obstacles[PART_JR2]->setSensor(true);
	// _obstacles[PART_JR3]->setSensor(true);
	// _obstacles[PART_JR4]->setSensor(true);
	// _obstacles[PART_JR5]->setSensor(true);
	// _obstacles[PART_JR6]->setSensor(true);
	// _obstacles[PART_LR1]->setSensor(true);
	// _obstacles[PART_LR2]->setSensor(true);
	// _obstacles[PART_LR3]->setSensor(true);
	// _obstacles[PART_LR4]->setSensor(true);
	// _obstacles[PART_LR5]->setSensor(true);
	// _obstacles[PART_LR6]->setSensor(true);
	return true;
}

std::string CharacterController::getPartName(int part) {
	switch (part) {
	case PART_BODY:
		return BODY_TEXTURE;
    case PART_RH:
        return R_HAND_TEXTURE;
    case PART_LH:
        return L_HAND_TEXTURE;
	// case PART_JR1:
	// case PART_JR2:
	// case PART_JR3:
	// case PART_JR4:
	// case PART_JR5:
	// case PART_JR6:
	// case PART_LR1:
	// case PART_LR2:
	// case PART_LR3:
	// case PART_LR4:
	// case PART_LR5:
	// case PART_LR6:
	// 	return CJOINT_TEXTURE;
	default:
		return "UNKNOWN";
	}
}

bool CharacterController::createJoints() {
//	std::shared_ptr<physics2::RevoluteJoint> revjoint;
//	std::shared_ptr<physics2::MotorJoint> motjoint;
//	std::shared_ptr<physics2::PrismaticJoint> prijoint;
//	std::shared_ptr<physics2::DistanceJoint> distjoint;
//	std::shared_ptr<physics2::WeldJoint> weldjoint;

	// right arm, forearm, hand
	// BODY->JR1 rev
	// revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_JR1]);
	// revjoint->setLocalAnchorA(CJOINT_OFFSET, 0);
	// revjoint->setLocalAnchorB(0, 0);
	// revjoint->enableLimit(true);
	// revjoint->setUpperAngle(M_PI / 2.0f);
	// revjoint->setLowerAngle(-M_PI / 2.0f);
	// _joints.push_back(revjoint);

	// JR1->JR2 pri
	// prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR1], _obstacles[PART_JR2]);
	// prijoint->setLocalAnchorA(0, 0);
	// prijoint->setLocalAnchorB(0, 0);
	// prijoint->setLocalAxisA(1, 0);
	// prijoint->enableLimit(true);
	// prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	// prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	// _joints.push_back(prijoint);

	// JR2->JR3 pri
	// prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR2], _obstacles[PART_JR3]);
	// prijoint->setLocalAnchorA(0, 0);
	// prijoint->setLocalAnchorB(0, 0);
	// prijoint->setLocalAxisA(1, 0);
	// prijoint->enableLimit(true);
	// prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	// prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	// _joints.push_back(prijoint);

	// JR3->JR4 rev, both anchor is 0,0
	// revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_JR3], _obstacles[PART_JR4]);
	// revjoint->setLocalAnchorA(0, 0);
	// revjoint->setLocalAnchorB(0, 0);
	// revjoint->enableLimit(true);
	// revjoint->setUpperAngle(M_PI / 2.0f);
	// revjoint->setLowerAngle(-M_PI / 2.0f);
	// _joints.push_back(revjoint);

	// JR4->JR5 pri
	// prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR4], _obstacles[PART_JR5]);
	// prijoint->setLocalAnchorA(0, 0);
	// prijoint->setLocalAnchorB(0, 0);
	// prijoint->setLocalAxisA(1, 0);
	// prijoint->enableLimit(true);
	// prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	// prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	// _joints.push_back(prijoint);

	// JR5->JR6 pri
	// prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR5], _obstacles[PART_JR6]);
	// prijoint->setLocalAnchorA(0, 0);
	// prijoint->setLocalAnchorB(0, 0);
	// prijoint->setLocalAxisA(1, 0);
	// prijoint->enableLimit(true);
	// prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	// prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	// _joints.push_back(prijoint);

	// JR6->RH rev
	// revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_JR6], _obstacles[PART_RH]);
	// revjoint->setLocalAnchorA(0, 0);
	// revjoint->setLocalAnchorB(0, 0);
	// revjoint->enableLimit(true);
	// revjoint->setUpperAngle(M_PI / 2.0f);
	// revjoint->setLowerAngle(-M_PI / 2.0f);
	// _joints.push_back(revjoint);

	// left part
	// body->LR1 rev
	// revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_LR1]);
	// revjoint->setLocalAnchorA(-CJOINT_OFFSET, 0);
	// revjoint->setLocalAnchorB(0, 0);
	// revjoint->enableLimit(true);
	// revjoint->setUpperAngle(M_PI / 2.0f);
	// revjoint->setLowerAngle(-M_PI / 2.0f);
	// _joints.push_back(revjoint);

	// LR1->LR2 pri
	// prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR1], _obstacles[PART_LR2]);
	// prijoint->setLocalAnchorA(0, 0);
	// prijoint->setLocalAnchorB(0, 0);
	// prijoint->setLocalAxisA(-1, 0);
	// prijoint->enableLimit(true);
	// prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	// prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	// _joints.push_back(prijoint);

	// LR2->LR3 pri
	// prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR2], _obstacles[PART_LR3]);
	// prijoint->setLocalAnchorA(0, 0);
	// prijoint->setLocalAnchorB(0, 0);
	// prijoint->setLocalAxisA(-1, 0);
	// prijoint->enableLimit(true);
	// prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	// prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	// _joints.push_back(prijoint);

	// LR3->LR4 rev, both anchor is 0,0
	// revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_LR3], _obstacles[PART_LR4]);
	// revjoint->setLocalAnchorA(0, 0);
	// revjoint->setLocalAnchorB(0, 0);
	// revjoint->enableLimit(true);
	// revjoint->setUpperAngle(M_PI / 2.0f);
	// revjoint->setLowerAngle(-M_PI / 2.0f);
	// _joints.push_back(revjoint);

	// LR4->LR5 pri
	// prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR4], _obstacles[PART_LR5]);
	// prijoint->setLocalAnchorA(0, 0);
	// prijoint->setLocalAnchorB(0, 0);
	// prijoint->setLocalAxisA(-1, 0);
	// prijoint->enableLimit(true);
	// prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	// prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	// _joints.push_back(prijoint);

	// LR5->LR6 pri
	// prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR5], _obstacles[PART_LR6]);
	// prijoint->setLocalAnchorA(0, 0);
	// prijoint->setLocalAnchorB(0, 0);
	// prijoint->setLocalAxisA(-1, 0);
	// prijoint->enableLimit(true);
	// prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	// prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	// _joints.push_back(prijoint);

	// LR6->LH rev
	// revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_LR6], _obstacles[PART_LH]);
	// revjoint->setLocalAnchorA(0, 0);
	// revjoint->setLocalAnchorB(0, 0);
	// revjoint->enableLimit(true);
	// revjoint->setUpperAngle(M_PI / 2.0f);
	// revjoint->setLowerAngle(-M_PI / 2.0f);
	// _joints.push_back(revjoint);

	if (_motorEnabled) {
		/* new version of motor joint direct body <-> hands*/
		rightHandJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_RH]);
		rightHandJoint->setMaxForce(MAX_FORCE);
		rightHandJoint->setLinearOffset(rightShoulderOffset + rightHandOffset);
		// rightHandJoint->setMaxTorque(MAX_TORQUE);
		// rightHandJoint->setAngularOffset(0);
		_joints.push_back(rightHandJoint);

		leftHandJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_LH]);
		leftHandJoint->setMaxForce(MAX_FORCE);
		leftHandJoint->setLinearOffset(leftShoulderOffset + leftHandOffset);
		// leftHandJoint->setMaxTorque(MAX_TORQUE);
		// leftHandJoint->setAngularOffset(0);
		_joints.push_back(leftHandJoint);

		// leftElbowJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_LR3]);
		// leftElbowJoint->setMaxForce(MAX_FORCE);
		// leftElbowJoint->setLinearOffset(leftShoulderOffset + leftElbowOffset);
		// _joints.push_back(leftElbowJoint);

		// rightElbowJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_JR3]);
		// rightElbowJoint->setMaxForce(MAX_FORCE);
		// rightElbowJoint->setLinearOffset(rightShoulderOffset + rightElbowOffset);
		// _joints.push_back(rightElbowJoint);

		// leftArmJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_LR2]);
		// leftArmJoint->setMaxForce(MAX_FORCE / 10.0f);
		// leftArmJoint->setLinearOffset(leftShoulderOffset + (leftElbowOffset / 2.0f));
		// _joints.push_back(leftArmJoint);

		// rightArmJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_JR2]);
		// rightArmJoint->setMaxForce(MAX_FORCE / 10.0f);
		// rightArmJoint->setLinearOffset(rightShoulderOffset + (rightElbowOffset / 2.0f));
		// _joints.push_back(rightArmJoint);

		// leftForearmJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_LR5]);
		// leftForearmJoint->setMaxForce(MAX_FORCE / 10.0f);
		// leftForearmJoint->setLinearOffset(leftShoulderOffset + ((leftHandOffset + leftElbowOffset) / 2.0f));
		// _joints.push_back(leftForearmJoint);

		// rightForearmJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_JR5]);
		// rightForearmJoint->setMaxForce(MAX_FORCE / 10.0f);
		// rightForearmJoint->setLinearOffset(rightShoulderOffset + ((rightHandOffset + rightElbowOffset) / 2.0f));
		// _joints.push_back(rightForearmJoint);
	}


	return true;
}

std::vector<std::shared_ptr<cugl::physics2::Joint>> CharacterController::dummy_jointsmaker(std::vector<int> part_indices) {
	std::vector<std::shared_ptr<cugl::physics2::Joint>> dummy_joints;
	for (int i = 0; i < part_indices.size(); i++) {
		for (int j = i + 1; j < part_indices.size(); j++) {
			std::shared_ptr<physics2::MotorJoint> dummy_joint = physics2::MotorJoint::allocWithObstacles(_obstacles[i], _obstacles[j]);
			dummy_joint->setMaxForce(0);
			dummy_joint->setMaxTorque(0);
			dummy_joints.push_back(dummy_joint);
		}
	}
	return dummy_joints;
}

void CharacterController::setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node) {
	_node = node;
	for (int ii = 0; ii <= PART_LH; ii++) {
		std::shared_ptr<Texture> image = _textures[ii];
		std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
		_node->addChild(sprite);
	}
}

void CharacterController::setDrawScale(float scale) {
	_drawScale = scale;
}

//DEPRECATED
void CharacterController::activate(const std::shared_ptr<cugl::physics2::net::NetWorld>& world) {
	for (auto it = _obstacles.begin(); it != _obstacles.end(); ++it) {
		world->addObstacle(*it);
	}
	for (auto it = _joints.begin(); it != _joints.end(); ++it) {
		world->addJoint(*it);
	}
}

//DEPRECATED
void CharacterController::deactivate(const std::shared_ptr<cugl::physics2::net::NetWorld>& world) {
//	for (auto it = _obstacles.begin(); it != _obstacles.end(); ++it) {
//		world->removeObstacle(*it);
//	}
//	for (auto it = _joints.begin(); it != _joints.end(); ++it) {
//		world->removeJoint(*it);
//	}
}

bool CharacterController::moveLeftHand(cugl::Vec2 offset) {
	if (!_motorEnabled) return false;
	float a = _obstacles[PART_BODY]->getAngle();
	leftHandOffset = leftHandOffset + Vec2(offset.x * cos(a) + offset.y * sin(a), -offset.x * sin(a) + offset.y * cos(a));

	float length = leftHandOffset.length();
	if (length > MAX_ARM_LENGTH) {
		leftHandOffset = leftHandOffset * (MAX_ARM_LENGTH / length);
	}
	if (length < MIN_ARM_LENGTH) {
		leftHandOffset = leftHandOffset * (MIN_ARM_LENGTH / length);
	}
	if (leftHandOffset.x > 0) {
		leftHandOffset.x = 0;
	}
	leftHandJoint->setLinearOffset(leftShoulderOffset + leftHandOffset);
	float a2 = -atan2(leftHandOffset.y, -leftHandOffset.x);
	// _obstacles[PART_LH]->setAngle(a2);
	// leftHandJoint->setAngularOffset(a2); // set angle as hand rotation
	// leftHandJoint->setAngularOffset(-a); // set angle as body rotation, keep hand horizontal
	Vec2 leftElbowPosition = armMiddleExp_R(Vec2(-leftHandOffset.x, leftHandOffset.y));
	leftElbowPosition.x = -leftElbowPosition.x;
	leftElbowOffset = leftElbowPosition;
	// move elbow and arms
	// leftElbowJoint->setLinearOffset(leftShoulderOffset + leftElbowOffset);
	// leftArmJoint->setLinearOffset((leftShoulderOffset + (leftElbowOffset / 2.0f)));
	// leftForearmJoint->setLinearOffset((leftShoulderOffset + ((leftHandOffset + leftElbowOffset) / 2.0f)));
	return true;
}

bool CharacterController::moveRightHand(cugl::Vec2 offset) {
	if (!_motorEnabled) return false;
	float a = _obstacles[PART_BODY]->getAngle();
	rightHandOffset = rightHandOffset + Vec2(offset.x * cos(a) + offset.y * sin(a), -offset.x * sin(a) + offset.y * cos(a));

	float length = rightHandOffset.length();
	if (length > MAX_ARM_LENGTH) {
		rightHandOffset = rightHandOffset * (MAX_ARM_LENGTH / length);
	}
	if (length < MIN_ARM_LENGTH) {
		rightHandOffset = rightHandOffset * (MIN_ARM_LENGTH / length);
	}
	if (rightHandOffset.x < 0) {
		rightHandOffset.x = 0;
	}
	rightHandJoint->setLinearOffset(rightShoulderOffset + rightHandOffset);
	float a2 = atan2(rightHandOffset.y, rightHandOffset.x);
	// _obstacles[PART_RH]->setAngle(a2);
	// rightHandJoint->setAngularOffset(a2); // set angle as hand rotation
	// rightHandJoint->setAngularOffset(-a); // set angle as body rotation, keep hand horizontal
	Vec2 rightElbowPosition = armMiddleExp_R(rightHandOffset);
	rightElbowOffset = rightElbowPosition;
	// move elbow and arms
	// rightElbowJoint->setLinearOffset(rightShoulderOffset + rightElbowPosition);
	// rightArmJoint->setLinearOffset((rightShoulderOffset + (rightElbowOffset / 2.0f)));
	// rightForearmJoint->setLinearOffset((rightShoulderOffset + ((rightHandOffset + rightElbowOffset) / 2.0f)));
	return true;
}

Vec2 CharacterController::SolveAngleMiddleBisectorLine(Vec2 p) {
	float a = atan2(p.y, p.x);
	float hald_angle = a / 2;
	return Vec2(cos(hald_angle), sin(hald_angle));
}

Vec2 CharacterController::armMiddleExp_R(Vec2 end) {
	Vec2 p1 = SolveAngleMiddleBisectorLine(end);
	float x1 = p1.x;
	float y1 = p1.y;
	float x2 = end.x;
	float y2 = end.y;
	float x = (x1 * (x2 * x2 + y2 * y2)) / (2 * (x1 * x2 + y1 * y2));
	float y = (y1 * (x2 * x2 + y2 * y2)) / (2 * (x1 * x2 + y1 * y2));
	return Vec2(x, y);
}

void CharacterController::enableMotor() {
	_motorEnabled = true;
}

void CharacterController::disableMotor() {
	_motorEnabled = false;
}

/**
 * Sets the scene graph node representing this Ragdoll.
 *
 * Note that this method also handles creating the nodes for the body parts
 * of this Ragdoll. Since the obstacles are decoupled from the scene graph,
 * initialization (which creates the obstacles) occurs prior to the call to
 * this method. Therefore, to be drawn to the screen, the nodes of the attached
 * bodies must be added here.
 *
 * The bubbler also uses the world node when adding bubbles to the scene, so
 * the input node must be added to the world BEFORE this method is called.
 *
 * By storing a reference to the scene graph node, the model can update
 * the node to be in sync with the physics info. It does this via the
 * {@link Obstacle#update(float)} method.
 *
 * @param node  The scene graph node representing this Ragdoll, which has been added to the world node already.
 */
void CharacterController::linkPartsToWorld(const std::shared_ptr<cugl::physics2::net::NetWorld>& _world, const std::shared_ptr<cugl::scene2::SceneNode>& _scenenode, float _scale) {
    this->_world = _world;
	_node = _scenenode;
    drawCharacterLines(_scale);
	for (int i = 0; i < _obstacles.size(); i++) {
		auto obj = _obstacles[i];
        // if ((i >= PART_JR1 && i <= PART_JR6) || (i >= PART_LR1 && i <= PART_LR6)) {
        //     _world->addObstacle(obj);
        //     continue;
        // }
		std::shared_ptr<Texture> image = _textures[i];
		std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);

		if (i == PART_BODY) {
			_bodyNode = sprite;
		}
		else if (i == PART_LH) _LHNode = sprite;
		else if (i == PART_RH) _RHNode = sprite;

		_world->addObstacle(obj);
		//obj->setDebugScene(_debugnode);
		_scenenode->addChild(sprite);
		// Dynamic objects need constant updating
		if (obj->getBodyType() == b2_dynamicBody) {
			scene2::SceneNode* weak = sprite.get(); // No need for smart pointer in callback
			obj->setListener([=](physics2::Obstacle* obs) {
				//float leftover = Application::get()->getFixedRemainder() / 1000000.f;
				//Vec2 pos = obs->getPosition() + leftover * obs->getLinearVelocity();
				//float angle = obs->getAngle() + leftover * obs->getAngularVelocity();

				weak->setPosition(obs->getPosition() * _scale);
				weak->setAngle(obs->getAngle());
                drawCharacterLines(_scale);
				});
		}
	}
	for (int i = 0; i < _joints.size(); i++) {
        printf("adding joints again");
		auto joint = _joints[i];
		_world->addJoint(joint);
	}
}



void CharacterController::drawCharacterLines(float scale) {
    if (_obstacles.size() >= PART_LH + 1) {
		float body_angle = _obstacles[PART_BODY]->getAngle();
		Vec2 leftshoulder = _obstacles[PART_BODY]->getPosition() + Vec2(leftShoulderOffset.x * cos(body_angle) - leftShoulderOffset.y * sin(body_angle), leftShoulderOffset.x * sin(body_angle) + leftShoulderOffset.y * cos(body_angle));
		Vec2 rightshoulder = _obstacles[PART_BODY]->getPosition() + Vec2(rightShoulderOffset.x * cos(body_angle) - rightShoulderOffset.y * sin(body_angle), rightShoulderOffset.x * sin(body_angle) + rightShoulderOffset.y * cos(body_angle));
        
		Vec2 lr1Pos = leftshoulder;
        Vec2 lr3Pos = lr1Pos + Vec2(leftElbowOffset.x * cos(body_angle) - leftElbowOffset.y * sin(body_angle), leftElbowOffset.x * sin(body_angle) + leftElbowOffset.y * cos(body_angle));
        Vec2 lhPos = _obstacles[PART_LH]->getPosition();
        Vec2 jr1Pos = rightshoulder;
        Vec2 jr3Pos = jr1Pos + Vec2(rightElbowOffset.x * cos(body_angle) - rightElbowOffset.y * sin(body_angle), rightElbowOffset.x * sin(body_angle) + rightElbowOffset.y * cos(body_angle));
        Vec2 rhPos = _obstacles[PART_RH]->getPosition();
//        CULog("lr1Pos is : %f %f \n", lr1Pos.x, lr1Pos.y);
//        CULog("lr3Pos is : %f %f \n", lr3Pos.x, lr3Pos.y);

        auto createLine = [&](const Vec2& start, const Vec2& end, size_t index) {
            float length = (start - end).length();
            float angle = atan2(end.y - start.y, end.x - start.x);
            Rect lineRect(0, 0, length*1.1 * scale, 5.0f);
            Vec2 center = (start + end) / 2.0f;

            std::shared_ptr<scene2::PolygonNode> line;
            if (index < _lineNodes.size()) {
                line = _lineNodes[index];
                line->setPolygon(Rect(0, 0, length*1.1 * scale, 15.0f ));
            } else {
                line = scene2::PolygonNode::allocWithPoly(lineRect);
                _lineNodes.push_back(line);
                _node->addChild(line);
            }

            line->setPosition(center * scale);
            line->setAngle(angle);
            line->setColor(Color4::BLACK);
        };

        createLine(lr1Pos-0.2*(lr3Pos - lr1Pos), lr3Pos, 0);
        createLine(lr3Pos, lhPos, 1);
        createLine(jr1Pos - 0.2*(jr3Pos - jr1Pos), jr3Pos, 2);
        createLine(jr3Pos, rhPos, 3);
//        createLine(lr1Pos, lr3Pos, 0);
//        createLine(lr4Pos, lhPos, 1);
//        createLine(jr1Pos, jr3Pos, 2);
//        createLine(jr4Pos, rhPos, 3);

    }
}

