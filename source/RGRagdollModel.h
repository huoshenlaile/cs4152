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
#ifndef _RG_RAGDOLL_MODEL_H
#define _RG_RAGDOLL_MODEL_H
#include <cugl/cugl.h>
#include <vector>
#include "RGBubbleGenerator.h"

#pragma mark -
#pragma mark Body Part Indices
/** Indices for the body parts in the bodies array */
#define PART_NONE           -1
#define PART_BODY           0
#define PART_LEFT_E1        1
#define PART_RIGHT_E1       2
#define PART_LEFT_E2        3
#define PART_RIGHT_E2       4
#define PART_LEFT_E3        5
#define PART_RIGHT_E3       6
#define PART_LEFT_E4        7
#define PART_RIGHT_E4       8
#define PART_LEFT_E5        9
#define PART_RIGHT_E5       10
#define PART_LEFT_HAND      11
#define PART_RIGHT_HAND     12

#pragma mark -
#pragma mark ATTENTION! PART CHANGE HERE


#pragma mark -
#pragma mark Body Part Textures
/** Textures for the body parts in the asset manager */
#define BODY_TEXTURE        "body"
#define E1_TEXTURE          "arm"
#define E2_TEXTURE          "miarm"
#define E3_TEXTURE          "miarm"
#define E4_TEXTURE          "miarm"
#define E5_TEXTURE          "miarm"
#define HAND_TEXTURE          "forearm"


#pragma mark -
#pragma mark Body Part Offsets
// Layout of ragdoll
//
// o = joint
//                   ___
//                  |   |
//                  |_ _|
//   ______ ______ ___o___ ______ ______
//  |______o______o       o______o______|
//                |       |
//                |       |
//                |_______|
//                | o | o |
//                |   |   |
//                |___|___|
//                | o | o |
//                |   |   |
//                |   |   |
//                |___|___|
//
/** Y-distance between torso center and arm center */
#define E1_YOFFSET      0.75f
/** X-distance between torso center and arm center */
#define E1_XOFFSET      3.15f
/** Distance between center of arm and center of forearm */
#define E2_OFFSET  2.1f
#define E3_OFFSET  2.1f
#define E4_OFFSET  0.0f
#define E5_OFFSET  2.1f
#define HAND_OFFSET  2.1f


#pragma mark -
#pragma mark Physics Constants
/** The density for each body part */
#define DEFAULT_DENSITY  1.0f
/** The density for the center of mass */
#define CENTROID_DENSITY 0.1f
/** The radius for the center of mass */
#define CENTROID_RADIUS  0.1f


#pragma mark -
#pragma mark Ragdoll
/**
 * A ragdoll whose body parts are boxes connected by joints
 *
 * Note that this module handles its own scene graph management. Since a
 * ComplexObstacle owns all of its child obstacles, it is natural for it to
 * own the corresponding as well scene graph. 
 *
 * For the construction, see the ragdoll diagram above, with the position 
 * offsets.
 */
class RagdollModel {
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CU_DISALLOW_COPY_AND_ASSIGN(RagdollModel);

protected:
    /** The object name */
    std::string _name;
	/** Bubble generator to glue to snorkler. */
	std::shared_ptr<BubbleGenerator> _bubbler;
	/** The textures for the individual body parts */
    std::vector<std::shared_ptr<cugl::Texture>> _textures;
    /** The collection of obstacles in this model */
    std::vector<std::shared_ptr<cugl::physics2::Obstacle>> _obstacles;
    /** The collection of joints in this model */
    std::vector<std::shared_ptr<cugl::physics2::Joint>> _joints;
    /** The offset to apply to all the obstacles */
    Vec2 _offset;

    /**
     * The scene graph node for the Ragdoll.
     *
     * This is empty, but attaches parts to it.
     */
    std::shared_ptr<cugl::scene2::SceneNode> _node;

    /**
     * The debug scene graph node for the Ragdoll.
     *
     * This is empty, but attaches parts to it.
     */
    std::shared_ptr<cugl::scene2::SceneNode> _debug;
    
    /** Cache object for transforming the force according the object angle */
    cugl::Mat4 _affine;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;
    
    /**
     * Returns the texture key for the given body part.
     *
     * As some body parts are symmetrical, we reuse textures.
     *
     * @return the texture key for the given body part
     */
    std::string getPartName(int part);
    
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
     * @param  scale    The draw scale to convert from world to screen coordinates
     *
     * @return the created body part
     */
    std::shared_ptr<cugl::physics2::BoxObstacle> makePart(int part, int connect, const cugl::Vec2& pos);
    
public:
#pragma mark -
#pragma mark Constructors
	/**
	 * Creates a new Ragdoll at the origin.
	 *
	 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate a model on
	 * the heap, use one of the static constructors instead.
	 */
	RagdollModel(void) : _drawScale(0) { }

    /**
     * Destroys this Ragdoll, releasing all resources.
     */
	virtual ~RagdollModel(void) { dispose(); }
    
    /**
     * Disposes all resources and assets of this Ragdoll
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a Ragdoll may not be used until it is initialized again.
     */
    void dispose();
    
    /**
     * Initializes a new Ragdoll at the origin.
     *
     * The Ragdoll is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    bool init() {
        return init(cugl::Vec2::ZERO, 1.0f);
    }
    
    /**
     * Initializes a new Ragdoll with the given position
     *
     * The Ragdoll is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    bool init(const cugl::Vec2& pos) {
        return init(pos,1.0f);
    }
    
    /**
     * Initializes a new Ragdoll with the given position and scale
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param scale The drawing scale to convert world to screen coordinates
	 *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
	bool init(const cugl::Vec2& pos, float scale);

    
#pragma mark -
#pragma mark Static Constructors
    /**
     * Returns a newly allocated Ragdoll at the origin.
     *
     * The Ragdoll is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return a newly allocated Ragdoll at the origin.
     */
    static std::shared_ptr<RagdollModel> alloc() {
        std::shared_ptr<RagdollModel> result = std::make_shared<RagdollModel>();
        return (result->init() ? result : nullptr);
    }
    
    /**
     * Returns a newly allocated Ragdoll with the given position
     *
     * The Ragdoll is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     *
     * @return a newly allocated Ragdoll with the given position
     */
    static std::shared_ptr<RagdollModel> alloc(const cugl::Vec2& pos) {
        std::shared_ptr<RagdollModel> result = std::make_shared<RagdollModel>();
        return (result->init(pos, 1.0f) ? result : nullptr);
    }

	/**
	 * Returns a newly allocated Ragdoll with the given position and scale
	 *
	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
	 * @param pos   Initial position in world coordinates
     * @param scale The drawing scale to convert world to screen coordinates
	 *
	 * @return a newly allocated Ragdoll with the given position
	 */
    static std::shared_ptr<RagdollModel> alloc(const cugl::Vec2& pos, float scale) {
		std::shared_ptr<RagdollModel> result = std::make_shared<RagdollModel>();
		return (result->init(pos, scale) ? result : nullptr);
	}
    

#pragma mark Physics Methods
    /**
     * Activates all of the obstacles in this model.
     *
     * This method invokes {@link Obstacle#activatePhysics} for the each
     * of the obstacles in this model.
     *
     * @param world    The associated obstacle world
     */
    void activate(const std::shared_ptr<physics2::ObstacleWorld>& world);
    
    /**
     * Destroys the obstacles in this model, if appropriate,
     * removing them from the world.
     *
     * @param world    The associated obstacle world
     */
    void deactivate(const std::shared_ptr<physics2::ObstacleWorld>& world);

    /**
     * Creates the joints for this object.
     *
     * This method defines the joints but does not activate them. That is the
     * purpose of {@link #activate}.
     *
     * @return true if object allocation succeeded
     */
    bool createJoints();

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
    bool buildParts(const std::shared_ptr<AssetManager>& assets);
    
    
#pragma mark -
#pragma mark Attribute Accessors
    /**
     * Returns the texture for the given body part.
     *
     * As some body parts are symmetrical, we may reuse textures.
     *
     * @return the texture for the given body part
     */
    const std::shared_ptr<Texture> getPart(int part) const {
        return _textures[part];
    }

    /**
     * Sets the texture for the given body part.
     *
     * As some body parts are symmetrical, we may reuse textures.
     *
     * @param part      The part identifier
     * @param texture   The texture for the given body part
     */
    void setPart(int part, const std::shared_ptr<Texture>& texture);

	/**
	 * Returns the bubble generator for this ragdoll
	 *
	 * The bubble generator will be offset at the snorkel on the head.
	 *
	 * @return the bubble generator for this ragdoll
	 */
	const std::shared_ptr<BubbleGenerator> getBubbleGenerator() const {
        return _bubbler;
    }

    /**
     * Creates the bubble generator for this ragdoll
     *
     * The bubble generator will be offset at the snorkel on the head.
     *
     * @param texture   The texture for an individual bubble
     */
    void makeBubbleGenerator(const std::shared_ptr<Texture>& bubble);

    
#pragma mark -
#pragma mark Animation
	/**
	 * Returns the scene graph node representing this Ragdoll.
	 *
	 * By storing a reference to the scene graph node, the model can update
	 * the node to be in sync with the physics info. It does this via the
	 * {@link Obstacle#update(float)} method.
	 *
	 * @return the scene graph node representing this Ragdoll.
	 */
	const std::shared_ptr<cugl::scene2::SceneNode>& getNode() const { return _node; }

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
	 * @param node  The scene graph node representing this Ragdoll
	 */
	void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node);

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
     * @param node  the parent scene graph node for the debug wireframe
     */
    void setDebugScene(const std::shared_ptr<scene2::SceneNode>& node);
    
    /**
     * Sets the color of the debug wireframe.
     *
     * The default color is white, which means that the objects will be shown
     * with a white wireframe.
     *
     * @param color the color of the debug wireframe.
     */
    void setDebugColor(Color4 color);
    
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
	void setDrawScale(float scale);
  
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
    void update(float delta);
};

#endif /* _RG_RAGDOLL_MODEL_H */
