//
//  RGRagdollModel.h
//  Ragdoll Demo
//
//  This module provides the infamous Walker White ragdoll from 3152.  This time it
//  is fully assembled for you.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design. This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together. However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  Pay close attention to how this class designed. This class uses our standard
//  shared-pointer architecture which is common to the entire engine.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden (2007).
//  This file has been refactored to support the physics changes in CUGL 2.5.
//
//  Author: Walker White and Anthony Perello
//  Version:  2/9/24
//
#include "RGRagdollModel.h"
#include <box2d/b2_revolute_joint.h>
#include <box2d/b2_weld_joint.h>
#include <box2d/b2_world.h>


using namespace cugl;

#pragma mark -
#pragma mark Animation and Physics Constants

/** This is adjusted by screen aspect ratio to get the height */
#define GAME_WIDTH 1024

/** The offset of the snorkel from the doll's head (in Box2d units) */
float BUBB_OFF[] = { 0.55f,  1.9f };

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new Ragdoll with the given position
 *
 * The Ragdoll is 1 unit by 1 unit in size. The Ragdoll is scaled so that
 * 1 pixel = 1 Box2d unit
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos		Initial position in world coordinates
 * @param  scale    The drawing scale to convert between world and screen coordinates
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool RagdollModel::init(const Vec2& pos, float scale) {
    _name = "ragdoll";
	_node = nullptr;
    _offset = pos;
    _drawScale = scale;

	return true;
}

/**
 * Disposes all resources and assets of this Ragdoll
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a Ragdoll may not be used until it is initialized again.
 */
void RagdollModel::dispose() {
    _node = nullptr;
    _obstacles.clear();
}


#pragma mark -
#pragma mark Part Initialization
/**
 * Creates the individual body parts for this ragdoll
 *
 * The size of the body parts is determined by the scale together with
 * the assets (as part of the asset manager).  This will fail if any
 * body part assets are missing.
 *
 * @param assets The program asset manager
 *
 * @return true if the body parts were successfully created
 */
bool RagdollModel::buildParts(const std::shared_ptr<AssetManager>& assets) {
    CUAssertLog(_obstacles.empty(), "Bodies are already initialized");
   
    // Get the images from the asset manager
    bool success = true;
    for(int ii = 0; ii <= NUM_OF_PARTS; ii++) {
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
    
    // Now make everything
    std::shared_ptr<physics2::BoxObstacle> part;
    
    // TORSO
    Vec2 pos = _offset;
    part = makePart(PART_BODY, PART_NONE, pos);
    part->setFixedRotation(true);
    
//    // HEAD
//    makePart(PART_HEAD, PART_BODY, Vec2(0, TORSO_OFFSET));
    
    // ARMS
    makePart(PART_LEFT_ARM, PART_BODY, Vec2(-ARM_XOFFSET, ARM_YOFFSET));
    makePart(PART_RIGHT_ARM, PART_BODY, Vec2(ARM_XOFFSET, ARM_YOFFSET));
    
    // FOREARMS
    makePart(PART_LEFT_FOREARM, PART_LEFT_ARM, Vec2(-FOREARM_OFFSET, 0));
    makePart(PART_RIGHT_FOREARM, PART_RIGHT_ARM, Vec2(FOREARM_OFFSET, 0));
    
    // HANDS
    makePart(PART_LEFT_HAND, PART_LEFT_FOREARM, Vec2(-HAND_OFFSET, 0));
    makePart(PART_RIGHT_HAND, PART_RIGHT_FOREARM, Vec2(HAND_OFFSET, 0));
    
//    // THIGHS
//    makePart(PART_LEFT_THIGH, PART_BODY, Vec2(-THIGH_XOFFSET, -THIGH_YOFFSET));
//    makePart(PART_RIGHT_THIGH, PART_BODY, Vec2(THIGH_XOFFSET, -THIGH_YOFFSET));
//    
//    // SHINS
//    makePart(PART_LEFT_SHIN, PART_LEFT_THIGH, Vec2(0, -SHIN_OFFSET));
//    makePart(PART_RIGHT_SHIN, PART_RIGHT_THIGH, Vec2(0, -SHIN_OFFSET));

    return true;
}

/**
 * Sets the texture for the given body part.
 *
 * As some body parts are symmetrical, we may reuse textures.
 *
 * @param part      The part identifier
 * @param texture   The texture for the given body part
 */
void RagdollModel::setPart(int part, const std::shared_ptr<Texture>& texture) {
    if (_textures.size() <= NUM_OF_PARTS) {
        _textures.resize(NUM_OF_PARTS+1, nullptr);
    }
    _textures[part] = texture;
}


/**
* Returns a single body part
*
* While it looks like this method "connects" the pieces, it does not really.
* It puts them in position to be connected by joints, but they will fall apart
* unless you make the joints.
*
* @param  part     Part to create
* @param  connect  Part to connect it to
* @param  pos      Position RELATIVE to connecting part
*
* @return the created body part
*/
std::shared_ptr<physics2::BoxObstacle> RagdollModel::makePart(int part, int connect, const Vec2& pos) {
	std::shared_ptr<Texture> image = _textures[part];
	Size size = image->getSize();
	size.width /= _drawScale;
	size.height /= _drawScale;

	Vec2 pos2 = pos;
	if (connect != PART_NONE) {
		pos2 += _obstacles[connect]->getPosition();
	}

	std::shared_ptr<physics2::BoxObstacle> body = physics2::BoxObstacle::alloc(pos2, size);
	body->setName(getPartName(part));
	body->setDensity(DEFAULT_DENSITY);
	
    _obstacles.push_back(body);
	return body;
}

/**
 * Returns the texture key for the given body part.
 *
 * As some body parts are symmetrical, we reuse textures.
 *
 * @return the texture key for the given body part
 */
std::string RagdollModel::getPartName(int part) {
	switch (part) {
//	case PART_HEAD:
//		return HEAD_TEXTURE;
	case PART_BODY:
		return BODY_TEXTURE;
	case PART_LEFT_ARM:
	case PART_RIGHT_ARM:
		return ARM_TEXTURE;
	case PART_LEFT_FOREARM:
	case PART_RIGHT_FOREARM:
		return FOREARM_TEXTURE;
    case PART_LEFT_HAND:
    case PART_RIGHT_HAND:
        return HAND_TEXTURE;
//	case PART_LEFT_THIGH:
//	case PART_RIGHT_THIGH:
//		return THIGH_TEXTURE;
//	case PART_LEFT_SHIN:
//	case PART_RIGHT_SHIN:
//		return SHIN_TEXTURE;
	default:
		return "UNKNOWN";
	}
}



#pragma mark -
#pragma mark Physics
/**
 * Activates all of the obstacles in this model.
 *
 * This method invokes {@link Obstacle#activatePhysics} for the each
 * of the obstacles in this model.
 *
 * @param world    The associated obstacle world
 */
void RagdollModel::activate(const std::shared_ptr<physics2::ObstacleWorld>& world) {
    for(auto it = _obstacles.begin(); it != _obstacles.end(); ++it) {
        world->addObstacle(*it);
    }
    for(auto it = _joints.begin(); it != _joints.end(); ++it) {
        world->addJoint(*it);
    }
}

/**
 * Destroys the obstacles in this model, if appropriate,
 * removing them from the world.
 *
 * @param world    The associated obstacle world
 */
void RagdollModel::deactivate(const std::shared_ptr<physics2::ObstacleWorld>& world) {
    for(auto it = _obstacles.begin(); it != _obstacles.end(); ++it) {
        world->removeObstacle(*it);
    }
    for(auto it = _joints.begin(); it != _joints.end(); ++it) {
        world->removeJoint(*it);
    }
}

/**
 * Creates the joints for this object.
 *
 * This method defines the joints but does not activate them. That is the
 * purpose of {@link #activate}.
 *
 * @return true if object allocation succeeded
 */
bool RagdollModel::createJoints() {
    std::shared_ptr<physics2::RevoluteJoint> joint;
    std::shared_ptr<physics2::WeldJoint> jointW;
    std::shared_ptr<physics2::PrismaticJoint> jointP;
    float small = 0.001f;


    // SHOULDERS
    joint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_LEFT_ARM],
                                                        _obstacles[PART_BODY]);
    joint->setLocalAnchorA(ARM_XOFFSET / 2, 0);
    joint->setLocalAnchorB(-ARM_XOFFSET / 2, ARM_YOFFSET);
    joint->enableLimit(true);
    joint->setUpperAngle(M_PI / 2.0f);
    joint->setLowerAngle(-M_PI / 2.0f);
    _joints.push_back(joint);

    joint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_RIGHT_ARM],
                                                        _obstacles[PART_BODY]);
    joint->setLocalAnchorA(-ARM_XOFFSET / 2, 0);
    joint->setLocalAnchorB(ARM_XOFFSET / 2, ARM_YOFFSET);
    joint->enableLimit(true);
    joint->setUpperAngle(M_PI / 2.0f);
    joint->setLowerAngle(-M_PI / 2.0f);
    _joints.push_back(joint);

    // ELBOWS
    joint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_LEFT_FOREARM],
                                                        _obstacles[PART_LEFT_ARM]);
    joint->setLocalAnchorA(FOREARM_OFFSET / 2, 0);
    joint->setLocalAnchorB(-FOREARM_OFFSET / 2, 0);
    joint->enableLimit(true);
    joint->setUpperAngle(M_PI / 2.0f);
    joint->setLowerAngle(-M_PI / 2.0f);
    _joints.push_back(joint);

    joint = physics2::RevoluteJoint::allocWithObstacles(_obstacles[PART_RIGHT_FOREARM],
                                                        _obstacles[PART_RIGHT_ARM]);
    joint->setLocalAnchorA(-FOREARM_OFFSET / 2, 0);
    joint->setLocalAnchorB(FOREARM_OFFSET / 2, 0);
    joint->enableLimit(true);
    joint->setUpperAngle(M_PI / 2.0f);
    joint->setLowerAngle(-M_PI / 2.0f);
    _joints.push_back(joint);
    
    // RIGID WRISTS USING WELD JOINTS
    jointP = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_LEFT_FOREARM], _obstacles[PART_LEFT_HAND]);
    jointP->setLocalAnchorA(-HAND_OFFSET / 2, 0);
    jointP->setLocalAnchorB(HAND_OFFSET / 2, 0);
    jointP->enableLimit(true);
    _joints.push_back(jointP);

    jointP = physics2::PrismaticJoint::allocWithObstacles(_obstacles[PART_RIGHT_FOREARM], _obstacles[PART_RIGHT_HAND]);
    jointP->setLocalAnchorA(HAND_OFFSET / 2, 0);
    jointP->setLocalAnchorB(-HAND_OFFSET / 2, 0);
    jointP->enableLimit(true);
    _joints.push_back(jointP);

    return true;
}

#pragma mark -
#pragma mark Scene Graph Management

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
void RagdollModel::setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node){
	_node = node;
	for (int ii = 0; ii <= NUM_OF_PARTS; ii++) {
        std::shared_ptr<Texture> image = _textures[ii];
		std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
		if (ii == PART_RIGHT_ARM || ii == PART_RIGHT_FOREARM) {
			sprite->flipHorizontal(true); // More reliable than rotating 90 degrees.
		}
		_node->addChild(sprite);
	}
}

/**
 * Sets the scene graph node representing this Ragdoll, and adds it to physics.
 *
 * Note that this method also handles creating the nodes for the body parts
 * of this Ragdoll. Since the obstacles are decoupled from the scene graph,
 * initialization (which creates the obstacles) occurs prior to the call to
 * this method. Therefore, to be drawn to the screen, the nodes of the attached
 * bodies must be added here.
 * *
 * By storing a reference to the scene graph node, the model can update
 * the node to be in sync with the physics info. It does this via the
 * {@link Obstacle#update(float)} method.
 *
 * @param scenenode  The scene graph node representing this Ragdoll, which has been added to the world node already.
 */

void RagdollModel::linkPartsToWorld(const std::shared_ptr<cugl::physics2::net::NetWorld>& _world, const std::shared_ptr<cugl::scene2::SceneNode>& _scenenode, float _scale) {
    _node = _scenenode;
    for(int i = 0; i < _obstacles.size(); i++){
        auto obj = _obstacles[i];
        std::shared_ptr<Texture> image = _textures[i];
        std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
        if (i == PART_RIGHT_ARM || i == PART_RIGHT_FOREARM) {
            sprite->flipHorizontal(true); // More reliable than rotating 90 degrees.
        }

        _world->initObstacle(obj);
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
    for(int i = 0; i < _joints.size(); i++){
        auto joint = _joints[i];
        _world->addJoint(joint);
    }
 
}


/**
 * Sets the parent scene graph node for the debug wireframe
 *
 * The given node is the parent coordinate space for drawing physics.
 * All debug nodes for physics objects are drawn within this coordinate
 * space.  Setting the visibility of this node to false will disable
 * any debugging.  Similarly, setting this value to nullptr will
 * disable any debugging.
 *
 * This scene graph node is intended for debugging purposes only.  If
 * you want a physics body to update a proper texture image, you should
 * either use the method {@link update(float)} for subclasses or
 * {@link setListener} for decoupled classes.
 *
 * @param node  he parent scene graph node for the debug wireframe
 */
void RagdollModel::setDebugScene(const std::shared_ptr<scene2::SceneNode>& node) {
    for(auto it = _obstacles.begin(); it != _obstacles.end(); it++) {
        (*it)->setDebugScene(node);
    }
}

/**
 * Sets the color of the debug wireframe.
 *
 * The default color is white, which means that the objects will be shown
 * with a white wireframe.
 *
 * @param color the color of the debug wireframe.
 */
void RagdollModel::setDebugColor(Color4 color) {
    for(auto it = _obstacles.begin(); it!= _obstacles.end(); ++it) {
        (*it)->setDebugColor(color);
    }
}

/**
 * Sets the ratio of the Ragdoll sprite to the physics body
 *
 * The Ragdoll needs this value to convert correctly between the physics
 * coordinates and the drawing screen coordinates.  Otherwise it will
 * interpret one Box2D unit as one pixel.
 *
 * All physics scaling must be uniform.  Rotation does weird things when
 * attempting to scale physics by a non-uniform factor.
 *
 * @param scale The ratio of the Ragdoll sprite to the physics body
 */
void RagdollModel::setDrawScale(float scale) {
    _drawScale = scale;
}

#pragma mark -
#pragma mark Interpolation
/**
 * Updates the scene graph to interpolate the visuals.
 *
 * This method is called AFTER all physics is complete. The value delta
 * is the time remaining that could not be simulated. The objects in the
 * scene graph node are interpolated to match that time.
 *
 * @param delta The time since the last physics simulation call
 */
void RagdollModel::update(float delta) {
    if (_node != nullptr) {
        std::vector<std::shared_ptr<scene2::SceneNode>> children = _node->getChildren();
        int ii = 0;
        
        // Update the nodes of the attached bodies
        for (auto it = children.begin(); it != children.end(); ++it) {
            Vec2 pos = _obstacles[ii]->getPosition();
            pos += _obstacles[ii]->getLinearVelocity()*delta;
            (*it)->setPosition(pos*_drawScale);
            
            float angle = _obstacles[ii]->getAngle();
            angle += _obstacles[ii]->getAngularVelocity()*delta;
            (*it)->setAngle(angle);
            
            ii++;
        }
    }
}
