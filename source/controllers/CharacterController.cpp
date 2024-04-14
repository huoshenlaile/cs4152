#include "CharacterController.h"

#define DURATION 7.0f

bool CharacterController::init(const cugl::Vec2 &pos, float scale)
{
	_name = "Dusty";
	_node = nullptr;
	_offset = pos;
	_drawScale = scale;

	_color = "black";
	_colorchange = false;
    _colorlhchange = false;
    _colorrhchange = false;
    _colorbodychange = false;
	_actions = scene2::ActionManager::alloc();
	const int span = 24;
	std::vector<int> animate;
	for (int ii = 1; ii < span; ii++)
	{
		animate.push_back(ii);
	}
	animate.push_back(0);
	_animate = scene2::Animate::alloc(animate, DURATION);

	const int blackspan = 75;
	std::vector<int> blackanimate;
	for (int ii = 1; ii < blackspan; ii++)
	{
		blackanimate.push_back(ii);
	}
	blackanimate.push_back(0);
	_blackanimate = scene2::Animate::alloc(blackanimate, DURATION);

	return true;
}

CharacterController::CharacterController(void) : _drawScale(0)
{
}

/**
 * Destroys this level, releasing all resources.
 */
CharacterController::~CharacterController(void)
{
	// nothing for now, adding dispose causes an error: resetting the map for the second time, the game would crash because the joint to remove did not exist
	// This may need to be fixed in the future
	//    dispose();
}

void CharacterController::dispose()
{
	printf("Character Controller: removing joints");
	for (auto it = _joints.begin(); it != _joints.end(); ++it)
	{
		if (_world != nullptr)
		{
			_world->removeJoint(*it);
		}
		(*it) = nullptr;
	}

	for (auto it = _obstacles.begin(); it != _obstacles.end(); ++it)
	{
		if (_world != nullptr)
		{
			_world->removeObstacle((*it));
		}
		(*it) = nullptr;
	}
	_node->removeAllChildren();
	_obstacles.clear();
	_lineNodes.clear();
	_joints.clear();
}

std::shared_ptr<physics2::Obstacle> CharacterController::makePart(int part, int connect, const Vec2 &pos)
{
	std::shared_ptr<Texture> image = _textures[part];
	CULog("makePart texture name: %s", image->getName().c_str());
	Size size = image->getSize();
	size.width /= _drawScale;
	size.height /= _drawScale;

	Vec2 pos2 = pos;
	if (connect != PART_NONE)
	{
		pos2 += _obstacles[connect]->getPosition();
	}
	// std::shared_ptr<physics2::BoxObstacle> body = physics2::BoxObstacle::alloc(pos2, size);
	// create body  polygon as octagon
	// calculate index first
	std::shared_ptr<physics2::Obstacle> body;

	if (part == PART_BODY)
	{
		body = physics2::BoxObstacle::alloc(pos2, size);
	}
	else
	{
		float w = size.width / 2.0f;
		float h = size.height / 2.0f;
		float oct[] = {-w, -h * 0.414f,
					   -w * 0.414f, -h,
					   w * 0.414f, -h,
					   w, -h * 0.414f,
					   w, h * 0.414f,
					   w * 0.414f, h,
					   -w * 0.414f, h,
					   -w, h * 0.414f};

		CULog("w: %f, h: %f", w, h);
		cugl::Poly2 octpoly = cugl::Poly2(reinterpret_cast<Vec2 *>(oct), 8);
		EarclipTriangulator triangulator;
		triangulator.set(octpoly.vertices);
		triangulator.calculate();
		octpoly.setIndices(triangulator.getTriangulation());
		triangulator.clear();

		body = physics2::PolygonObstacle::allocWithAnchor(octpoly, Vec2(0.5, 0.5));
		body->setPosition(pos2);
	}
	body->setBodyType(b2_dynamicBody);

	// log all information
	// CULog("part: %d", part);
	// CULog("connect: %d", connect);
	// CULog("pos: %f, %f", pos.x, pos.y);
	// CULog("pos2: %f, %f", pos2.x, pos2.y);
	// CULog("size: %f, %f", size.width, size.height);
	body->setName(getPartName(part));
	if (part == PART_BODY)
	{
		body->setDensity(BODY_DENSITY);
		// CULog("makePart body centroid position: %f, %f", body->getCentroid().x, body->getCentroid().y);
	}
	else if (part == PART_RH || part == PART_LH)
	{
		body->setDensity(HAND_DENSITY);
	}
	else
	{
		body->setDensity(DEFAULT_DENSITY);
	}

	// add extra friction to the PART_LH and PART_RH
	// set always upward direction
	if (part == PART_LH || part == PART_RH)
	{

		body->setFriction(HAND_FRICTION);
		body->setFixedRotation(true);
	}
	else
	{
		body->setFriction(OTHER_FRICTION);
	}

	// add body with BODY_ANULAR_DAMPING
	if (part == PART_BODY)
	{
		body->setAngularDamping(BODY_ANGULAR_DAMPING);
		CULog("makePart body part %d angular damping: %f", part, body->getAngularDamping());
	}

	// log friction information:
	CULog("makePart body part %d friction: %f", part, body->getFriction());

	// log parts and resitituion
	CULog("makePart body part %d restitution: %f", part, body->getRestitution());
    
#pragma mark OBSTACLE ADD
	_obstacles.push_back(body);
	return body;
}

bool CharacterController::buildParts(const std::shared_ptr<AssetManager> &assets)
{
	CUAssertLog(_obstacles.empty(), "Bodies are already initialized");

	// Get the images from the asset manager
	bool success = true;
	for (int ii = 0; ii <= PART_LH; ii++)
	{
		if (ii == PART_BODY)
		{
			std::shared_ptr<Texture> bodySpriteSheet = assets->get<Texture>("pink_animation");
			std::shared_ptr<Texture> decorationSheet = assets->get<Texture>("pink_decoration");
			std::shared_ptr<Texture> lefthandSheet = assets->get<Texture>("pink_lh");
			std::shared_ptr<Texture> righthandSheet = assets->get<Texture>("pink_rh");
			if (bodySpriteSheet == nullptr || decorationSheet == nullptr || lefthandSheet == nullptr || righthandSheet == nullptr)
			{
				success = false;
			}
			else
			{
				_pinktextures = bodySpriteSheet;
				_pinkdecoration = decorationSheet;
				_pinklh = lefthandSheet;
				_pinkrh = righthandSheet;
			}
			bodySpriteSheet = assets->get<Texture>("black_animation");
			lefthandSheet = assets->get<Texture>("black_lh");
			righthandSheet = assets->get<Texture>("black_rh");
			if (bodySpriteSheet == nullptr || lefthandSheet == nullptr || righthandSheet == nullptr)
			{
				success = false;
			}
			else
			{
				_blacktextures = bodySpriteSheet;
				_blacklh = lefthandSheet;
				_blackrh = righthandSheet;
			}
			bodySpriteSheet = assets->get<Texture>("blue_animation");
			decorationSheet = assets->get<Texture>("blue_decoration");
			lefthandSheet = assets->get<Texture>("blue_lh");
			righthandSheet = assets->get<Texture>("blue_rh");
			if (bodySpriteSheet == nullptr || decorationSheet == nullptr || lefthandSheet == nullptr || righthandSheet == nullptr)
			{
				success = false;
			}
			else
			{
				_bluetextures = bodySpriteSheet;
				_bluedecoration = decorationSheet;
				_bluelh = lefthandSheet;
				_bluerh = righthandSheet;
			}
			bodySpriteSheet = assets->get<Texture>("green_animation");
			decorationSheet = assets->get<Texture>("green_decoration");
			lefthandSheet = assets->get<Texture>("green_lh");
			righthandSheet = assets->get<Texture>("green_rh");
			if (bodySpriteSheet == nullptr || decorationSheet == nullptr || lefthandSheet == nullptr || righthandSheet == nullptr)
			{
				success = false;
			}
			else
			{
				_greentextures = bodySpriteSheet;
				_greendecoration = decorationSheet;
				_greenlh = lefthandSheet;
				_greenrh = righthandSheet;
			}
			bodySpriteSheet = assets->get<Texture>("orange_animation");
			decorationSheet = assets->get<Texture>("orange_decoration");
			lefthandSheet = assets->get<Texture>("orange_lh");
			righthandSheet = assets->get<Texture>("orange_rh");
			if (bodySpriteSheet == nullptr || decorationSheet == nullptr || lefthandSheet == nullptr || righthandSheet == nullptr)
			{
				success = false;
			}
			else
			{
				_orangetextures = bodySpriteSheet;
				_orangedecoration = decorationSheet;
				_orangelh = lefthandSheet;
				_orangerh = righthandSheet;
			}
			bodySpriteSheet = assets->get<Texture>("purple_animation");
			decorationSheet = assets->get<Texture>("purple_decoration");
			lefthandSheet = assets->get<Texture>("purple_lh");
			righthandSheet = assets->get<Texture>("purple_rh");
			if (bodySpriteSheet == nullptr)
			{
				success = false;
			}
			else
			{
				_purpletextures = bodySpriteSheet;
				_purpledecoration = decorationSheet;
				_purplelh = lefthandSheet;
				_purplerh = righthandSheet;
			}
		}
		std::string name = getPartName(ii);
		CULog("get character texture: %s", name.c_str());
		std::shared_ptr<Texture> image = assets->get<Texture>(name);
		if (image == nullptr)
		{
			success = false;
			CULog("get character texture failed: %s", name.c_str());
		}
		else
		{
			_textures.push_back(image);
		}
	}
	if (!success)
	{
		return false;
	}

	Vec2 pos = _offset;
	//    std::cout << _offset.x<< " aaa "<< _offset.y << std::endl;
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
	makePart(PART_LH, PART_BODY, Vec2(-4 * HALF_CJOINT_OFFSET, 0));

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

std::string CharacterController::getPartName(int part)
{
	switch (part)
	{
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

bool CharacterController::createJoints()
{
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

	if (_motorEnabled)
	{
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

/** This method is UNUSED right now. */
std::vector<std::shared_ptr<cugl::physics2::Joint>> CharacterController::dummy_jointsmaker(std::vector<int> part_indices)
{
	std::vector<std::shared_ptr<cugl::physics2::Joint>> dummy_joints;
	for (int i = 0; i < part_indices.size(); i++)
	{
		for (int j = i + 1; j < part_indices.size(); j++)
		{
			std::shared_ptr<physics2::MotorJoint> dummy_joint = physics2::MotorJoint::allocWithObstacles(_obstacles[i], _obstacles[j]);
			dummy_joint->setMaxForce(0);
			dummy_joint->setMaxTorque(0);
			dummy_joints.push_back(dummy_joint);
		}
	}
	return dummy_joints;
}

void CharacterController::setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode> &node)
{
	_node = node;
	for (int ii = 0; ii <= PART_LH; ii++)
	{
		if (ii == PART_BODY)
		{
			std::shared_ptr<scene2::SpriteNode> bodySprite = scene2::SpriteNode::allocWithSheet(_blacktextures, 6, 4, 24);
			bodySprite->setAnchor(Vec2::ANCHOR_CENTER);
			_node->addChild(bodySprite);
		}
		else
		{
			std::shared_ptr<Texture> image = _textures[ii];
			std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
			_node->addChild(sprite);
		}
	}
}

void CharacterController::setDrawScale(float scale)
{
	_drawScale = scale;
}

// DEPRECATED
void CharacterController::activate(const std::shared_ptr<cugl::physics2::net::NetWorld> &world)
{
	for (auto it = _obstacles.begin(); it != _obstacles.end(); ++it)
	{
		world->addObstacle(*it);
	}
	for (auto it = _joints.begin(); it != _joints.end(); ++it)
	{
		world->addJoint(*it);
	}
}

// DEPRECATED
void CharacterController::deactivate(const std::shared_ptr<cugl::physics2::net::NetWorld> &world)
{
	//	for (auto it = _obstacles.begin(); it != _obstacles.end(); ++it) {
	//		world->removeObstacle(*it);
	//	}
	//	for (auto it = _joints.begin(); it != _joints.end(); ++it) {
	//		world->removeJoint(*it);
	//	}
}

bool CharacterController::moveLeftHand(cugl::Vec2 offset, bool inverse)
{
	if (!_motorEnabled)
		return false;
	if (inverse)
	{
		offset = -offset;
	}
	float a = _obstacles[PART_BODY]->getAngle();
	leftHandOffset = leftHandOffset + Vec2(offset.x * cos(a) + offset.y * sin(a), -offset.x * sin(a) + offset.y * cos(a));

	float Hypotenuse = sqrt(1.0f + HAND_K * HAND_K);

	float length = leftHandOffset.length();

	if (leftHandOffset.x > 0)
	{
		if (leftHandOffset.y >= 0)
		{
			float k = leftHandOffset.y / leftHandOffset.x;
			if (k < HAND_K)
			{
				// need correction of hand position

				leftHandOffset.x = length / Hypotenuse;
				leftHandOffset.y = length * HAND_K / Hypotenuse;
			}
		}
		else
		{
			float k = -leftHandOffset.y / leftHandOffset.x;
			if (k < HAND_K)
			{

				leftHandOffset.x = length / Hypotenuse;
				leftHandOffset.y = -length * HAND_K / Hypotenuse;
			}
		}
	}

	// if it is too close to right hand, set min distance
	Vec2 leftHandTargetPos = leftShoulderOffset + leftHandOffset;
	Vec2 rightHandTargetPos = rightShoulderOffset + rightHandOffset;
	float distance = leftHandTargetPos.distance(rightHandTargetPos);
	// CULog("moveLeftHand distance: %f", distance);
	if (distance < MIN_HAND_DISTANCE)
	{
		Vec2 rightHandToLeftHand = leftHandTargetPos - rightHandTargetPos;
		rightHandToLeftHand.normalize();
		leftHandOffset = rightHandToLeftHand * MIN_HAND_DISTANCE + rightHandTargetPos - leftShoulderOffset;
	}

	length = leftHandOffset.length();
	if (length > MAX_ARM_LENGTH)
	{
		leftHandOffset = leftHandOffset * (MAX_ARM_LENGTH / length);
	}
	if (length < MIN_ARM_LENGTH)
	{
		leftHandOffset = leftHandOffset * (MIN_ARM_LENGTH / length);
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

bool CharacterController::moveRightHand(cugl::Vec2 offset, bool inverse)
{
	if (!_motorEnabled)
		return false;
	if (inverse)
	{
		offset = -offset;
	}
	float a = _obstacles[PART_BODY]->getAngle();
	rightHandOffset = rightHandOffset + Vec2(offset.x * cos(a) + offset.y * sin(a), -offset.x * sin(a) + offset.y * cos(a));

	float Hypotenuse = sqrt(1.0f + HAND_K * HAND_K);

	float length = rightHandOffset.length();

	// constraint the hand position
	if (rightHandOffset.x < 0)
	{
		if (rightHandOffset.y >= 0)
		{
			float k = -rightHandOffset.y / rightHandOffset.x;
			if (k < HAND_K)
			{
				rightHandOffset.x = -length / Hypotenuse;
				rightHandOffset.y = length * HAND_K / Hypotenuse;
			}
		}
		else
		{
			float k = rightHandOffset.y / rightHandOffset.x;
			if (k < HAND_K)
			{

				rightHandOffset.x = -length / Hypotenuse;
				rightHandOffset.y = -length * HAND_K / Hypotenuse;
			}
		}
	}

	// if it is too close to left hand, set min distance
	Vec2 leftHandTargetPos = leftShoulderOffset + leftHandOffset;
	Vec2 rightHandTargetPos = rightShoulderOffset + rightHandOffset;
	float distance = leftHandTargetPos.distance(rightHandTargetPos);
	if (distance < MIN_HAND_DISTANCE)
	{
		Vec2 leftHandToRightHand = rightHandTargetPos - leftHandTargetPos;
		leftHandToRightHand.normalize();
		rightHandOffset = leftHandToRightHand * MIN_HAND_DISTANCE + leftHandTargetPos - rightShoulderOffset;
	}





	length = rightHandOffset.length();
	if (length > MAX_ARM_LENGTH)
	{
		rightHandOffset = rightHandOffset * (MAX_ARM_LENGTH / length);
	}
	if (length < MIN_ARM_LENGTH)
	{
		rightHandOffset = rightHandOffset * (MIN_ARM_LENGTH / length);
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

Vec2 CharacterController::SolveAngleMiddleBisectorLine(Vec2 p)
{
	float a = atan2(p.y, p.x);
	float hald_angle = a / 2;
	return Vec2(cos(hald_angle), sin(hald_angle));
}

Vec2 CharacterController::armMiddleExp_R(Vec2 end)
{
	Vec2 p1 = SolveAngleMiddleBisectorLine(end);
	float x1 = p1.x;
	float y1 = p1.y;
	float x2 = end.x;
	float y2 = end.y;
	float x = (x1 * (x2 * x2 + y2 * y2)) / (2 * (x1 * x2 + y1 * y2));
	float y = (y1 * (x2 * x2 + y2 * y2)) / (2 * (x1 * x2 + y1 * y2));
	return Vec2(x, y);
}

void CharacterController::enableMotor()
{
	_motorEnabled = true;
}

void CharacterController::disableMotor()
{
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
void CharacterController::linkPartsToWorld(const std::shared_ptr<cugl::physics2::net::NetWorld> &_world, const std::shared_ptr<cugl::scene2::SceneNode> &_scenenode, float _scale)
{
	this->_world = _world;
	_node = _scenenode;
	drawCharacterLines(_scale);
	drawDecoration(_scale);
	for (int i = 0; i < _obstacles.size(); i++)
	{
		auto obj = _obstacles[i];
		// if ((i >= PART_JR1 && i <= PART_JR6) || (i >= PART_LR1 && i <= PART_LR6)) {
		//     _world->addObstacle(obj);
		//     continue;
		// }
		std::shared_ptr<Texture> image = _textures[i];
		std::shared_ptr<scene2::PolygonNode> sprite;
		std::shared_ptr<scene2::SpriteNode> bodySprite;

		// TODO: Put this in macros
		if (i == PART_BODY)
		{
			bodySprite = getTextureForPart(i, _color);
			_bodyNode = bodySprite;
			_scenenode->addChild(bodySprite);
		}
		else if (i == PART_LH)
		{
			sprite = getTextureForHand(i, _color);
			_LHNode = sprite;
		}
		else if (i == PART_RH)
		{
			sprite = getTextureForHand(i, _color);
			_RHNode = sprite;
		}
		_world->addObstacle(obj);
		// obj->setDebugScene(_debugnode);
		if (i == PART_BODY)
		{
		}
		else
		{
			_scenenode->addChild(sprite);
		}
		// Dynamic objects need constant updating
		// if (obj->getBodyType() == b2_dynamicBody)
		// {
		// 	scene2::SceneNode *weak;
		// 	if (i == PART_BODY)
		// 	{
		// 		weak = bodySprite.get();
		// 	}
		// 	else
		// 	{
		// 		weak = sprite.get();
		// 	} // No need for smart pointer in callback
		// 	obj->setListener([=](physics2::Obstacle *obs) mutable
		// 					 {
		// 		//float leftover = Application::get()->getFixedRemainder() / 1000000.f;
		// 		//Vec2 pos = obs->getPosition() + leftover * obs->getLinearVelocity();
		// 		//float angle = obs->getAngle() + leftover * obs->getAngularVelocity();
		// 		if(_colorchange == true){
		// 			if (i == PART_BODY)
		// 			{
		//                 _scenenode->removeChild(bodySprite);
		//                 bodySprite = getTextureForPart(i, _color);
		//                 weak = bodySprite.get();
		// 				_bodyNode = bodySprite;
		// 				_scenenode->addChild(bodySprite);
		// 			}
		// 			else if (i == PART_LH)
		// 			{
		// 				_scenenode->removeChild(sprite);
		// 				sprite = getTextureForHand(i, _color);
		//                 weak = bodySprite.get();
		// 				_LHNode = sprite;
		// 				_scenenode->addChild(sprite);
		// 			}
		// 			else if (i == PART_RH)
		// 			{
		// 				_scenenode->removeChild(sprite);
		// 				sprite = getTextureForHand(i, _color);
		//                 weak = bodySprite.get();
		// 				_RHNode = sprite;
		// 				_scenenode->addChild(sprite);

		// 			}
		// 		}
		// 		weak->setPosition(obs->getPosition() * _scale);
		// 		weak->setAngle(obs->getAngle());
		//         drawCharacterLines(_scale);
		//         drawDecoration(_scale); });
		// }
		if (obj->getBodyType() == b2_dynamicBody)
		{
			obj->setListener([=](physics2::Obstacle *obs) mutable
							 {
        if (_colorchange) {
            // Remove the old sprite and add the new sprite based on the part
            if (i == PART_BODY) {
                auto newBodySprite = getTextureForPart(i, _color);
                _scenenode->removeChild(_bodyNode);
                _bodyNode = newBodySprite;
                _scenenode->addChild(_bodyNode);
                _colorbodychange = true;
            } else if (i == PART_LH) {
                auto newHandSprite = getTextureForHand(i, _color);
                _scenenode->removeChild(_LHNode);
                _LHNode = newHandSprite;
                _scenenode->addChild(_LHNode);
                _colorlhchange = true;
            } else if (i == PART_RH) {
                auto newHandSprite = getTextureForHand(i, _color);
                _scenenode->removeChild(_RHNode);
                _RHNode = newHandSprite;
                _scenenode->addChild(_RHNode);
                _colorrhchange = true;
            }
        }

        if (i == PART_BODY) {
            _bodyNode->setPosition(obs->getPosition() * _scale);
            _bodyNode->setAngle(obs->getAngle());
        } else if (i == PART_LH) {
            _LHNode->setPosition(obs->getPosition() * _scale);
            _LHNode->setAngle(obs->getAngle());
        } else if (i == PART_RH) {
            _RHNode->setPosition(obs->getPosition() * _scale);
            _RHNode->setAngle(obs->getAngle());
        }
        drawCharacterLines(_scale);
        drawDecoration(_scale); });
		}
	}
	for (int i = 0; i < _joints.size(); i++)
	{
		printf("adding joints again");
		auto joint = _joints[i];
		_world->addJoint(joint);
	}
	std::cout << "BODY LOCATION=====" << _bodyNode->getPositionX() << ", " << _bodyNode->getPositionY() << std::endl;
}

void CharacterController::drawCharacterLines(float scale)
{
	if (_obstacles.size() >= PART_LH + 1)
	{
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

		auto createLine = [&](const Vec2 &start, const Vec2 &end, size_t index)
		{
			float length = (start - end).length();
			float angle = atan2(end.y - start.y, end.x - start.x);
			Rect lineRect(0, 0, length * 1.1 * scale, 5.0f);
			Vec2 center = (start + end) / 2.0f;

			std::shared_ptr<scene2::PolygonNode> line;
			if (index < _lineNodes.size())
			{
				line = _lineNodes[index];
				line->setPolygon(Rect(0, 0, length * 1.1 * scale, 15.0f));
			}
			else
			{
				line = scene2::PolygonNode::allocWithPoly(lineRect);
				_lineNodes.push_back(line);
				_node->addChild(line);
			}

			line->setPosition(center * scale);
			line->setAngle(angle);
			if (_color == "black")
			{
				line->setColor(Color4(1, 1, 1));
			}
			else if (_color == "orange")
			{
				line->setColor(Color4(230, 111, 72));
			}
			else if (_color == "pink")
			{
				line->setColor(Color4(225, 89, 125));
			}
			else if (_color == "purple")
			{
				line->setColor(Color4(154, 101, 214));
			}
			else if (_color == "green")
			{
				line->setColor(Color4(77, 168, 88));
			}
			else if (_color == "blue")
			{
				line->setColor(Color4(74, 144, 241));
			}
		};

		createLine(lr1Pos - 0.2 * (lr3Pos - lr1Pos), lr3Pos, 0);
		createLine(lr3Pos, lhPos, 1);
		createLine(jr1Pos - 0.2 * (jr3Pos - jr1Pos), jr3Pos, 2);
		createLine(jr3Pos, rhPos, 3);
		//        createLine(lr1Pos, lr3Pos, 0);
		//        createLine(lr4Pos, lhPos, 1);
		//        createLine(jr1Pos, jr3Pos, 2);
		//        createLine(jr4Pos, rhPos, 3);
	}
}

void CharacterController::drawDecoration(float scale)
{
	if (_obstacles.size() >= PART_BODY + 1)
	{
		Vec2 bodyPos = _obstacles[PART_BODY]->getPosition();
		float bodyAngle = _obstacles[PART_BODY]->getAngle();

		if (_decorationNode == nullptr)
		{
			std::shared_ptr<scene2::SpriteNode> decorationSprite;
			if (_color == "orange")
			{
				decorationSprite = scene2::SpriteNode::allocWithSheet(_orangedecoration, 5, 5, 24);
				decorationSprite->setAnchor(Vec2::ANCHOR_CENTER);
				_decorationNode = decorationSprite;
				_node->addChild(_decorationNode);
			}
			else if (_color == "pink")
			{
				decorationSprite = scene2::SpriteNode::allocWithSheet(_pinkdecoration, 5, 5, 24);
				decorationSprite->setAnchor(Vec2::ANCHOR_CENTER);
				_decorationNode = decorationSprite;
				_node->addChild(_decorationNode);
			}
			else if (_color == "purple")
			{
				decorationSprite = scene2::SpriteNode::allocWithSheet(_purpledecoration, 5, 5, 24);
				decorationSprite->setAnchor(Vec2::ANCHOR_CENTER);
				_decorationNode = decorationSprite;
				_node->addChild(_decorationNode);
			}
			else if (_color == "green")
			{
				decorationSprite = scene2::SpriteNode::allocWithSheet(_greendecoration, 5, 5, 24);
				decorationSprite->setAnchor(Vec2::ANCHOR_CENTER);
				_decorationNode = decorationSprite;
				_node->addChild(_decorationNode);
			}
			else if (_color == "blue")
			{
				decorationSprite = scene2::SpriteNode::allocWithSheet(_bluedecoration, 5, 5, 24);
				decorationSprite->setAnchor(Vec2::ANCHOR_CENTER);
				_decorationNode = decorationSprite;
				_node->addChild(_decorationNode);
			}
		}
		if (_decorationNode != nullptr)
		{
			_decorationNode->setPosition(bodyPos * scale);
			_decorationNode->setAngle(bodyAngle);
		}
	}
}

void CharacterController::update(float dt)
{
	if (_color != "black")
	{
		_actions->activate("other", _animate, _bodyNode);
        _actions->activate("decoration", _animate, _decorationNode);
		
	}
	else
	{
		_actions->activate("black", _blackanimate, _bodyNode);
	}
    _actions->update(dt);
    
    if(_colorlhchange == true && _colorrhchange == true && _colorbodychange == true){
        _colorchange = false;
    }

}

void CharacterController::setColor(std::string color)
{
	if (color != _color)
	{
		_color = color;
		_colorchange = true;
	}
}

std::string CharacterController::getColor()
{
	return this->_color;
}

std::shared_ptr<scene2::SpriteNode> CharacterController::getTextureForPart(int partId, const std::string &color)
{
	std::shared_ptr<scene2::SpriteNode> textureNode;

	if (partId == PART_BODY)
	{
		if (color == "black")
		{
			textureNode = scene2::SpriteNode::allocWithSheet(_blacktextures, 9, 9, 75);
		}
		else if (color == "orange")
		{
			textureNode = scene2::SpriteNode::allocWithSheet(_orangetextures, 5, 5, 24);
		}
		else if (color == "pink")
		{
			textureNode = scene2::SpriteNode::allocWithSheet(_pinktextures, 5, 5, 24);
		}
		else if (color == "purple")
		{
			textureNode = scene2::SpriteNode::allocWithSheet(_purpletextures, 5, 5, 24);
		}
		else if (color == "green")
		{
			textureNode = scene2::SpriteNode::allocWithSheet(_greentextures, 5, 5, 24);
		}
		else if (color == "blue")
		{
			textureNode = scene2::SpriteNode::allocWithSheet(_bluetextures, 5, 5, 24);
		}
	}

	if (textureNode)
	{
		textureNode->setAnchor(Vec2::ANCHOR_CENTER);
	}

	return textureNode;
}

std::shared_ptr<scene2::PolygonNode> CharacterController::getTextureForHand(int partId, const std::string &color)
{
	std::shared_ptr<scene2::PolygonNode> textureNode;

	if (partId == PART_LH)
	{
		if (color == "black")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_blacklh);
		}
		else if (color == "orange")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_orangelh);
		}
		else if (color == "pink")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_pinklh);
		}
		else if (color == "purple")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_purplelh);
		}
		else if (color == "green")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_greenlh);
		}
		else if (color == "blue")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_bluelh);
		}
	}
	else if (partId == PART_RH)
	{
		if (color == "black")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_blackrh);
		}
		else if (color == "orange")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_orangerh);
		}
		else if (color == "pink")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_pinkrh);
		}
		else if (color == "purple")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_purplerh);
		}
		else if (color == "green")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_greenrh);
		}
		else if (color == "blue")
		{
			textureNode = scene2::PolygonNode::allocWithTexture(_bluerh);
		}
	}

	return textureNode;
}
