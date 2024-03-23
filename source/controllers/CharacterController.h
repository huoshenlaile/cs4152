#ifndef __CHARACTER_CONTROLLER_H__
#define __CHARACTER_CONTROLLER_H__

#include <cugl/cugl.h>
#include <vector>
#include "../models/CharacterModel.h"

/*

 O--o--oo--o--o()o--o--oo--o--O

 */

// #define PART_NONE   -1
// #define PART_BODY   0
// #define PART_JR1    1
// #define PART_JR2    2
// #define PART_JR3    3
// #define PART_JR4    4
// #define PART_JR5    5
// #define PART_JR6    6
// #define PART_RH     7
// #define PART_LR1    8
// #define PART_LR2    9
// #define PART_LR3    10
// #define PART_LR4    11
// #define PART_LR5    12
// #define PART_LR6    13
// #define PART_LH     14

#define PART_NONE   -1
#define PART_BODY   0
#define PART_RH     1
#define PART_LH     2


#define BODY_TEXTURE "body192"
#define CJOINT_TEXTURE "joint32"
#define HAND_TEXTURE "hand64"
#define L_HAND_TEXTURE "lhand64"
#define R_HAND_TEXTURE "rhand64"

#define MAX_TORQUE 10000000.0f
#define MAX_FORCE  30000.0f


#define CJOINT_OFFSET 3.0f
#define HALF_CJOINT_OFFSET 1.5f
#define MAX_ARM_LENGTH 5.8f
#define MIN_ARM_LENGTH 0.0f


 /** The density for each body part */
#define DEFAULT_DENSITY  1.0f
/** The density for the center of mass */
#define CENTROID_DENSITY 0.1f
/** The radius for the center of mass */
#define CENTROID_RADIUS  0.1f

#define BODY_DENSITY 1.0f
#define HAND_DENSITY 1.0f
#define HAND_FRICTION 0.3f

using namespace cugl;

class CharacterController {
protected:
	std::string _name;
	std::vector<std::shared_ptr<cugl::Texture>> _textures;
	std::vector<std::shared_ptr<cugl::physics2::Obstacle>> _obstacles;
	std::vector<std::shared_ptr<cugl::physics2::Joint>> _joints;
    /** The physics world; part of the model (though listeners elsewhere) */
    std::shared_ptr<cugl::physics2::net::NetWorld> _world;

	cugl::Vec2 _offset;
	std::shared_ptr<cugl::scene2::SceneNode> _node;
	cugl::Mat4 _affine;
	float _drawScale;
	std::string getPartName(int part);
	bool _motorEnabled = true;
	std::shared_ptr<cugl::physics2::Obstacle> makePart(int part, int connect, const cugl::Vec2& pos);
	Vec2 SolveAngleMiddleBisectorLine(Vec2 p);
	Vec2 armMiddleExp_R(Vec2 end);
	std::vector<std::shared_ptr<cugl::physics2::Joint>> dummy_jointsmaker(std::vector<int> part_indices);
	cugl::Vec2 leftShoulderOffset;
	cugl::Vec2 leftHandOffset;
	cugl::Vec2 leftElbowOffset;
	cugl::Vec2 rightShoulderOffset;
	cugl::Vec2 rightHandOffset;
	cugl::Vec2 rightElbowOffset;
	// TODO fix memory leak, when dispose, should be none.
	std::shared_ptr<physics2::MotorJoint> leftHandJoint;
	std::shared_ptr<physics2::MotorJoint> rightHandJoint;
	std::shared_ptr<physics2::MotorJoint> leftElbowJoint;
	std::shared_ptr<physics2::MotorJoint> rightElbowJoint;

	// arm joints
	std::shared_ptr<physics2::MotorJoint> leftArmJoint;
	std::shared_ptr<physics2::MotorJoint> rightArmJoint;
	std::shared_ptr<physics2::MotorJoint> leftForearmJoint;
	std::shared_ptr<physics2::MotorJoint> rightForearmJoint;
    
    
    std::vector<std::shared_ptr<scene2::PolygonNode>> _lineNodes;
	std::shared_ptr<scene2::PolygonNode> _bodyNode;
	std::shared_ptr<scene2::PolygonNode> _LHNode;
	std::shared_ptr<scene2::PolygonNode> _RHNode;
public:
    CharacterController(void);
    virtual ~CharacterController(void);
	void dispose();

	bool init() {
		return init(cugl::Vec2::ZERO, 1.0f);
	}

	bool init(const cugl::Vec2& pos) {
		return init(pos, 1.0f);
	}

	bool init(const cugl::Vec2& pos, float scale);

#pragma mark Static Constructors

	static std::shared_ptr<CharacterController> alloc() {
		std::shared_ptr<CharacterController> result = std::make_shared<CharacterController>();
		return (result->init() ? result : nullptr);
	}
	static std::shared_ptr<CharacterController> alloc(const cugl::Vec2& pos) {
		std::shared_ptr<CharacterController> result = std::make_shared<CharacterController>();
		return (result->init(pos, 1.0f) ? result : nullptr);
	}
	static std::shared_ptr<CharacterController> alloc(const cugl::Vec2& pos, float scale) {
		std::shared_ptr<CharacterController> result = std::make_shared<CharacterController>();
		return (result->init(pos, scale) ? result : nullptr);
	}

	void activate(const std::shared_ptr<cugl::physics2::net::NetWorld>& world);
	void deactivate(const std::shared_ptr<cugl::physics2::net::NetWorld>& world);

	bool createJoints();
	bool buildParts(const std::shared_ptr<cugl::AssetManager>& assets);

	void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node);
	void setDrawScale(float scale);

	void enableMotor();
	void disableMotor();

	std::shared_ptr<scene2::PolygonNode> getBodySceneNode() { return _bodyNode; };

	Vec2 getLHPos() { return _LHNode->getPosition(); }
	Vec2 getRHPos() { return _RHNode->getPosition(); }

#pragma mark HAND CONTROLLER
	bool moveRightHand(cugl::Vec2 offset);
	bool moveLeftHand(cugl::Vec2 offset);

	Vec2 getLeftHandPosition() {
		auto p = _obstacles[PART_LH]->getPosition();
		//CULog("Left Hand Position: %f, %f", p.x, p.y);
		return p;
	}
	Vec2 getRightHandPosition() {
		auto p = _obstacles[PART_RH]->getPosition();
		//CULog("Right Hand Position: %f, %f", p.x, p.y);
		return p;
	}
	const std::vector<std::shared_ptr<cugl::physics2::Obstacle>> getObstacles() const { return _obstacles; }
	const std::vector<std::shared_ptr<cugl::physics2::Joint>> getJoints() const { return _joints; }

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
	void linkPartsToWorld(const std::shared_ptr<cugl::physics2::net::NetWorld>& _world, const std::shared_ptr<cugl::scene2::SceneNode>& _scenenode, float _scale);
    
    
    
    void drawCharacterLines(float scale);
};

#endif /* CharacterController_h */
