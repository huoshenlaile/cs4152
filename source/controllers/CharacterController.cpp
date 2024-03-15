#include "CharacterController.h"

bool CharacterController::init(const cugl::Vec2& pos, float scale) {
	_name = "TODO_PLACEHOLDER";
	_node = nullptr;
	_offset = pos;
	_drawScale = scale;

	return true;
}

void CharacterController::dispose() {
	_node = nullptr;
	leftHandJoint = nullptr;
	rightHandJoint = nullptr;
	_obstacles.clear();
	_joints.clear();
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
	if (part == PART_BODY) {
		body->setDensity(BODY_DENSITY);
	}
	else {
		body->setDensity(DEFAULT_DENSITY);
	}
	// log friction information:
//    CULog("body part %d friction: %f", part, body->getFriction());
	body->setFriction(2.5f);
	_obstacles.push_back(body);
	// log parts and resitituion
//    CULog("body part %d restitution: %f", part, body->getRestitution());
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
	makePart(PART_JR1, PART_BODY, Vec2(CJOINT_OFFSET, 0));
	makePart(PART_JR2, PART_JR1, Vec2(HALF_CJOINT_OFFSET, 0));
	makePart(PART_JR3, PART_JR2, Vec2(HALF_CJOINT_OFFSET, 0));
	makePart(PART_JR4, PART_JR3, Vec2(0, 0));
	makePart(PART_JR5, PART_JR4, Vec2(HALF_CJOINT_OFFSET, 0));
	makePart(PART_JR6, PART_JR5, Vec2(HALF_CJOINT_OFFSET, 0));
	makePart(PART_RH, PART_JR6, Vec2(0, 0));

	makePart(PART_LR1, PART_BODY, Vec2(-CJOINT_OFFSET, 0));
	makePart(PART_LR2, PART_LR1, Vec2(-HALF_CJOINT_OFFSET, 0));
	makePart(PART_LR3, PART_LR2, Vec2(-HALF_CJOINT_OFFSET, 0));
	makePart(PART_LR4, PART_LR3, Vec2(0, 0));
	makePart(PART_LR5, PART_LR4, Vec2(-HALF_CJOINT_OFFSET, 0));
	makePart(PART_LR6, PART_LR5, Vec2(-HALF_CJOINT_OFFSET, 0));
	makePart(PART_LH, PART_LR6, Vec2(0, 0));

	leftShoulderOffset = Vec2(-CJOINT_OFFSET, 0);
	rightShoulderOffset = Vec2(CJOINT_OFFSET, 0);
	float full_length = 4 * HALF_CJOINT_OFFSET;
	leftHandOffset = Vec2(-full_length, 0);
	rightHandOffset = Vec2(full_length, 0);
	leftElbowOffset = leftHandOffset / 2.0f;
	rightElbowOffset = rightHandOffset / 2.0f;

	_obstacles[PART_JR1]->setSensor(true);
	_obstacles[PART_JR2]->setSensor(true);
	_obstacles[PART_JR3]->setSensor(true);
	_obstacles[PART_JR4]->setSensor(true);
	_obstacles[PART_JR5]->setSensor(true);
	_obstacles[PART_JR6]->setSensor(true);
	_obstacles[PART_LR1]->setSensor(true);
	_obstacles[PART_LR2]->setSensor(true);
	_obstacles[PART_LR3]->setSensor(true);
	_obstacles[PART_LR4]->setSensor(true);
	_obstacles[PART_LR5]->setSensor(true);
	_obstacles[PART_LR6]->setSensor(true);
	return true;
}

std::string CharacterController::getPartName(int part) {
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
	case PART_JR6:
	case PART_LR1:
	case PART_LR2:
	case PART_LR3:
	case PART_LR4:
	case PART_LR5:
	case PART_LR6:
		return CJOINT_TEXTURE;
	default:
		return "UNKNOWN";
	}
}

bool CharacterController::createJoints() {
	std::shared_ptr<physics2::RevoluteJoint> revjoint;
	std::shared_ptr<physics2::MotorJoint> motjoint;
	std::shared_ptr<physics2::PrismaticJoint> prijoint;
	std::shared_ptr<physics2::DistanceJoint> distjoint;
	std::shared_ptr<physics2::WeldJoint> weldjoint;

	// right arm, forearm, hand
	// BODY->JR1 rev
	revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_JR1]);
	revjoint->setLocalAnchorA(CJOINT_OFFSET, 0);
	revjoint->setLocalAnchorB(0, 0);
	revjoint->enableLimit(true);
	revjoint->setUpperAngle(M_PI / 2.0f);
	revjoint->setLowerAngle(-M_PI / 2.0f);
	_joints.push_back(revjoint);

	// JR1->JR2 pri
	prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR1], _obstacles[PART_JR2]);
	prijoint->setLocalAnchorA(0, 0);
	prijoint->setLocalAnchorB(0, 0);
	prijoint->setLocalAxisA(1, 0);
	prijoint->enableLimit(true);
	prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	_joints.push_back(prijoint);

	// JR2->JR3 pri
	prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR2], _obstacles[PART_JR3]);
	prijoint->setLocalAnchorA(0, 0);
	prijoint->setLocalAnchorB(0, 0);
	prijoint->setLocalAxisA(1, 0);
	prijoint->enableLimit(true);
	prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	_joints.push_back(prijoint);

	// JR3->JR4 rev, both anchor is 0,0
	revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_JR3], _obstacles[PART_JR4]);
	revjoint->setLocalAnchorA(0, 0);
	revjoint->setLocalAnchorB(0, 0);
	revjoint->enableLimit(true);
	revjoint->setUpperAngle(M_PI / 2.0f);
	revjoint->setLowerAngle(-M_PI / 2.0f);
	_joints.push_back(revjoint);

	// JR4->JR5 pri
	prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR4], _obstacles[PART_JR5]);
	prijoint->setLocalAnchorA(0, 0);
	prijoint->setLocalAnchorB(0, 0);
	prijoint->setLocalAxisA(1, 0);
	prijoint->enableLimit(true);
	prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	_joints.push_back(prijoint);

	// JR5->JR6 pri
	prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_JR5], _obstacles[PART_JR6]);
	prijoint->setLocalAnchorA(0, 0);
	prijoint->setLocalAnchorB(0, 0);
	prijoint->setLocalAxisA(1, 0);
	prijoint->enableLimit(true);
	prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	_joints.push_back(prijoint);

	// JR6->RH rev
	revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_JR6], _obstacles[PART_RH]);
	revjoint->setLocalAnchorA(0, 0);
	revjoint->setLocalAnchorB(0, 0);
	revjoint->enableLimit(true);
	revjoint->setUpperAngle(M_PI / 2.0f);
	revjoint->setLowerAngle(-M_PI / 2.0f);
	_joints.push_back(revjoint);

	// left part
	// body->LR1 rev
	revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_LR1]);
	revjoint->setLocalAnchorA(-CJOINT_OFFSET, 0);
	revjoint->setLocalAnchorB(0, 0);
	revjoint->enableLimit(true);
	revjoint->setUpperAngle(M_PI / 2.0f);
	revjoint->setLowerAngle(-M_PI / 2.0f);
	_joints.push_back(revjoint);

	// LR1->LR2 pri
	prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR1], _obstacles[PART_LR2]);
	prijoint->setLocalAnchorA(0, 0);
	prijoint->setLocalAnchorB(0, 0);
	prijoint->setLocalAxisA(-1, 0);
	prijoint->enableLimit(true);
	prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	_joints.push_back(prijoint);

	// LR2->LR3 pri
	prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR2], _obstacles[PART_LR3]);
	prijoint->setLocalAnchorA(0, 0);
	prijoint->setLocalAnchorB(0, 0);
	prijoint->setLocalAxisA(-1, 0);
	prijoint->enableLimit(true);
	prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	_joints.push_back(prijoint);

	// LR3->LR4 rev, both anchor is 0,0
	revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_LR3], _obstacles[PART_LR4]);
	revjoint->setLocalAnchorA(0, 0);
	revjoint->setLocalAnchorB(0, 0);
	revjoint->enableLimit(true);
	revjoint->setUpperAngle(M_PI / 2.0f);
	revjoint->setLowerAngle(-M_PI / 2.0f);
	_joints.push_back(revjoint);

	// LR4->LR5 pri
	prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR4], _obstacles[PART_LR5]);
	prijoint->setLocalAnchorA(0, 0);
	prijoint->setLocalAnchorB(0, 0);
	prijoint->setLocalAxisA(-1, 0);
	prijoint->enableLimit(true);
	prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	_joints.push_back(prijoint);

	// LR5->LR6 pri
	prijoint = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LR5], _obstacles[PART_LR6]);
	prijoint->setLocalAnchorA(0, 0);
	prijoint->setLocalAnchorB(0, 0);
	prijoint->setLocalAxisA(-1, 0);
	prijoint->enableLimit(true);
	prijoint->setUpperTranslation(ARM_EXTEND * HALF_CJOINT_OFFSET);
	prijoint->setLowerTranslation(0.1f * HALF_CJOINT_OFFSET);
	_joints.push_back(prijoint);

	// LR6->LH rev
	revjoint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_LR6], _obstacles[PART_LH]);
	revjoint->setLocalAnchorA(0, 0);
	revjoint->setLocalAnchorB(0, 0);
	revjoint->enableLimit(true);
	revjoint->setUpperAngle(M_PI / 2.0f);
	revjoint->setLowerAngle(-M_PI / 2.0f);
	_joints.push_back(revjoint);

	if (_motorEnabled) {
		/* new version of motor joint direct body <-> hands*/
		rightHandJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_RH]);
		rightHandJoint->setMaxForce(MAX_FORCE);
		rightHandJoint->setLinearOffset(rightShoulderOffset + rightHandOffset);
		rightHandJoint->setMaxTorque(MAX_TORQUE);
		rightHandJoint->setAngularOffset(0);
		_joints.push_back(rightHandJoint);

		leftHandJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_LH]);
		leftHandJoint->setMaxForce(MAX_FORCE);
		leftHandJoint->setLinearOffset(leftShoulderOffset + leftHandOffset);
		leftHandJoint->setMaxTorque(MAX_TORQUE);
		leftHandJoint->setAngularOffset(0);
		_joints.push_back(leftHandJoint);

		leftElbowJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_LR3]);
		leftElbowJoint->setMaxForce(MAX_FORCE);
		leftElbowJoint->setLinearOffset(leftShoulderOffset + leftElbowOffset);
		_joints.push_back(leftElbowJoint);

		rightElbowJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_JR3]);
		rightElbowJoint->setMaxForce(MAX_FORCE);
		rightElbowJoint->setLinearOffset(rightShoulderOffset + rightElbowOffset);
		_joints.push_back(rightElbowJoint);

		leftArmJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_LR2]);
		leftArmJoint->setMaxForce(MAX_FORCE / 10.0f);
		leftArmJoint->setLinearOffset(leftShoulderOffset + (leftElbowOffset / 2.0f));
		_joints.push_back(leftArmJoint);

		rightArmJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_JR2]);
		rightArmJoint->setMaxForce(MAX_FORCE / 10.0f);
		rightArmJoint->setLinearOffset(rightShoulderOffset + (rightElbowOffset / 2.0f));
		_joints.push_back(rightArmJoint);

		leftForearmJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_LR5]);
		leftForearmJoint->setMaxForce(MAX_FORCE / 10.0f);
		leftForearmJoint->setLinearOffset(leftShoulderOffset + ((leftHandOffset + leftElbowOffset) / 2.0f));
		_joints.push_back(leftForearmJoint);

		rightForearmJoint = physics2::MotorJoint::allocWithObstacles(_obstacles[PART_BODY], _obstacles[PART_JR5]);
		rightForearmJoint->setMaxForce(MAX_FORCE / 10.0f);
		rightForearmJoint->setLinearOffset(rightShoulderOffset + ((rightHandOffset + rightElbowOffset) / 2.0f));
		_joints.push_back(rightForearmJoint);
	}

	// auto dummy_joints_right = dummy_jointsmaker({PART_BODY, PART_JR1, PART_JR2, PART_JR3, PART_JR4, PART_JR5, PART_JR6, PART_RH});
	// _joints.insert(_joints.end(), dummy_joints_right.begin(), dummy_joints_right.end());
	// auto dummy_joints_left = dummy_jointsmaker({PART_BODY, PART_LR1, PART_LR2, PART_LR3, PART_LR4, PART_LR5, PART_LR6, PART_LH});
	// _joints.insert(_joints.end(), dummy_joints_left.begin(), dummy_joints_left.end());

	// dummy joints before these obstacles so they collision can be disabled:
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
	for (auto it = _obstacles.begin(); it != _obstacles.end(); ++it) {
		world->removeObstacle(*it);
	}
	for (auto it = _joints.begin(); it != _joints.end(); ++it) {
		world->removeJoint(*it);
	}
}

bool CharacterController::moveLeftHand(cugl::Vec2 offset) {
	if (!_motorEnabled) return false;
	float a = _obstacles[PART_BODY]->getAngle();
	leftHandOffset = leftHandOffset + Vec2(offset.x * cos(a) + offset.y * sin(a), -offset.x * sin(a) + offset.y * cos(a));
	// normalize leftHandOffset to 4 * HALF_CJOINT_OFFSET if it is too long
	float full_length = 4 * HALF_CJOINT_OFFSET;
	float length = leftHandOffset.length();
	if (length > full_length) {
		leftHandOffset = leftHandOffset * (full_length / length);
	}
	if (length < HALF_CJOINT_OFFSET) {
		leftHandOffset = leftHandOffset * (HALF_CJOINT_OFFSET / length);
	}
	if (leftHandOffset.x > 0) {
		leftHandOffset.x = 0;
	}
	leftHandJoint->setLinearOffset(leftShoulderOffset + leftHandOffset);
	float a2 = -atan2(leftHandOffset.y, -leftHandOffset.x);
	// leftHandJoint->setAngularOffset(a2); // set angle as hand rotation
	leftHandJoint->setAngularOffset(-a); // set angle as body rotation, keep hand horizontal

	Vec2 leftElbowPosition = armMiddleExp_R(Vec2(-leftHandOffset.x, leftHandOffset.y));
	leftElbowPosition.x = -leftElbowPosition.x;
	leftElbowOffset = leftElbowPosition;
	leftElbowJoint->setLinearOffset(leftShoulderOffset + leftElbowOffset);

	leftArmJoint->setLinearOffset((leftShoulderOffset + (leftElbowOffset / 2.0f)));
	leftForearmJoint->setLinearOffset((leftShoulderOffset + ((leftHandOffset + leftElbowOffset) / 2.0f)));
	return true;
}

bool CharacterController::moveRightHand(cugl::Vec2 offset) {
	if (!_motorEnabled) return false;
	float a = _obstacles[PART_BODY]->getAngle();
	rightHandOffset = rightHandOffset + Vec2(offset.x * cos(a) + offset.y * sin(a), -offset.x * sin(a) + offset.y * cos(a));
	// normalize rightHandOffset to 4 * HALF_CJOINT_OFFSET if it is too long
	float full_length = 4 * HALF_CJOINT_OFFSET;
	float length = rightHandOffset.length();
	if (length > full_length) {
		rightHandOffset = rightHandOffset * (full_length / length);
	}
	// if it is shorter than 2 * HALF_CJOINT_OFFSET, then it is too short
	if (length < HALF_CJOINT_OFFSET) {
		rightHandOffset = rightHandOffset * (HALF_CJOINT_OFFSET / length);
	}
	if (rightHandOffset.x < 0) {
		rightHandOffset.x = 0;
	}
	rightHandJoint->setLinearOffset(rightShoulderOffset + rightHandOffset);
	float a2 = atan2(rightHandOffset.y, rightHandOffset.x);
	rightHandJoint->setAngularOffset(a2); // set angle as hand rotation
	rightHandJoint->setAngularOffset(-a); // set angle as body rotation, keep hand horizontal
	Vec2 rightElbowPosition = armMiddleExp_R(rightHandOffset);
	rightElbowOffset = rightElbowPosition;
	rightElbowJoint->setLinearOffset(rightShoulderOffset + rightElbowPosition);

	rightArmJoint->setLinearOffset((rightShoulderOffset + (rightElbowOffset / 2.0f)));
	rightForearmJoint->setLinearOffset((rightShoulderOffset + ((rightHandOffset + rightElbowOffset) / 2.0f)));
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
	_node = _scenenode;
	for (int i = 0; i < _obstacles.size(); i++) {
		auto obj = _obstacles[i];
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
				});
		}
	}
	for (int i = 0; i < _joints.size(); i++) {
		auto joint = _joints[i];
		_world->addJoint(joint);
	}
}