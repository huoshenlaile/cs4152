#pragma once
//
//  RGBubbleGenerator.h
//  RagdollDemo
//
//  This module provides a physics object that can generate bubbles from its
//  current location.  It is a physics object so that we can weld it to another
//  object to control its current position.  However, its main purpose is as a
//  Particle generator and therefore its graphics are slightly different than
//  other physics obstacles. See the comments on setSceneNode below for more
//  information.
//
//  Notices that this class makes use of GreedyFreelist.  That is a free list
//  that aggressively recycles objects, making it ideal for a particle system.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design. This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together. However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden (2007).
//  This file has been refactored to support the physics changes in CUGL 2.5.
//
//  Author: Walker White and Anthony Perello
//  Version:  2/9/24
//
#ifndef __RG_BUBBLE_GENERATOR_H__
#define __RG_BUBBLE_GENERATOR_H__

#include <box2d/b2_circle_shape.h>
#include <cugl/cugl.h>

using namespace cugl;


#pragma mark -
#pragma mark Bubble Defaults
/** The name of the bubble texture */
#define BUBBLE_TEXTURE  "bubble"
/** Maximum number of bubbles at a time */
#define MAX_PARTICLES   6
/** How long we have left to live */
#define DEFAULT_LIFE    500
/** How often we make bubbles */
#define BUBBLE_TIME     200
/** How big the bubbles are */
#define BUBBLE_RADIUS   0.2


#pragma mark -
#pragma mark Bubble Generator
/**
 * Physics object that generates non-physics bubble shapes.
 *
 * It is a physics object so that we can weld it to another object to control its
 * current position. However, its main purpose is as a particular generator, where
 * each particle is generated relative to the current location of this generator.
 *
 * The graphics for this class are slightly different than they are for other
 * physics obstacles.  The debug Node works as normal; it controls a positional
 * wireframe that shows the current location of the generator.  However, the scene
 * node should be a blank node centered at the origin. That way, the the particles
 * can be added as individual children to this node.
 *
 * Notices that this class makes use of GreedyFreelist.  That is a free list that
 * aggressively recycles objects, making it ideal for a particle system.
 */
class BubbleGenerator : public physics2::WheelObstacle {
    
protected:
#pragma mark -
#pragma mark Particle System
	/**
	 * Representation of the bubbles for drawing purposes.
	 *
	 * This is a graphics object, NOT a physics object.
	 */
	class Particle {
	public:
        /** The texture of an individual bubble */
        static std::shared_ptr<Texture> texture;
		/** Position of the bubble in Box2d space */
		Vec2 position;
		/** The number of animation frames left to live */
		int life;
		/** The image for this particle */
		std::shared_ptr<scene2::SceneNode> node;
        
		/** 
         * Creates a new Particle with no lifespan 
         *
         * @param generator A weak reference to the bubbble generator 
         */
		Particle();

		/** Disposes of this particle, permanently deleting all resources */
		~Particle();
        
		/** Resets the particle so it can be reclaimed by the free list */
		void reset() {
			position.set(0, 0);
			node->setPosition(Vec2::ZERO);
			life = -1;
		}

		/**
         * Updates the particle based on drawing scale
		 *
		 * @param  scale    the current drawing scale
		 */
		void update();
	};
    
    
	/** Free list to manage the system of particles. */
	std::shared_ptr<GreedyFreeList<Particle>> _memory;

    /** 
     * Vector of particles added to the game world from the FreeList.
     *
     * These particles are tracked for updating their drawing 
     */
    std::vector<Particle*> _activeParticles;
    
	/** How long bubbles live after creation */
	int _lifespan;
	/** How long until we can make another bubble */
	int _cooldown;
	/** Whether or not we bubbled this animation frame */
	bool _bubbled;
	
	/** The scene graph node for the generator */
	std::shared_ptr<scene2::SceneNode> _generatorNode;
    /** The individual bubble texture */
    std::shared_ptr<Texture> _texture;
	/** The scale between the physics world and the screen (MUST BE UNIFORM) */
	float _drawscale;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new bubble generator at the origin.
     */
    BubbleGenerator(void) : WheelObstacle(),
        _bubbled(false), _cooldown(0), _lifespan(0), _drawscale(0) { }
    
    /**
     * Destroys this bubble generator, releasing all resources.
     */
    virtual ~BubbleGenerator(void);

    /**
     * Initializes a bubble generator at the origin.
     *
     * The generator is scaled so that 1 pixel = 1 Box2d unit
     *
     * The attached scene graph is completely decoupled from the physics
     * system. The node does not have to be the same size as the physics body.
     * We only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override {
        return init(nullptr,Vec2::ZERO,1.0f);
    }
    
    /**
     * Initializes a bubble generator at the origin.
     *
     * The generator is scaled so that 1 pixel = 1 Box2d unit
     *
     * The attached scene graph is completely decoupled from the physics
     * system. The node does not have to be the same size as the physics body.
     * We only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param texture   The bubbble texture
     *
     * @return true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const std::shared_ptr<Texture>& texture) {
        return init(texture,Vec2::ZERO,1.0f);
    }
    
    /**
     * Initializes a bubble generator with the given position.
     *
     * The generator is scaled so that 1 pixel = 1 Box2d unit
     *
     * The attached scene graph is completely decoupled from the physics
     * system. The node does not have to be the same size as the physics body.
     * We only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param texture   The bubbble texture
     * @param pos       Initial position in world coordinates
     *
     * @return true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const std::shared_ptr<Texture>& texture, const Vec2& pos) {
        return init(texture,pos,1.0f);
    }
    
    /**
     * Initializes a bubble generator with the given position and scale
     *
     * The scale is the ratio of drawing coordinates to physics coordinates.
     * This allows us to construct the child objects appropriately.
     *
     * The attached scene graph is completely decoupled from the physics
     * system. The node does not have to be the same size as the physics body.
     * We only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param texture   The bubbble texture
     * @param pos       Initial position in world coordinates
     * @param scale     The drawing scale
     *
     * @return true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const std::shared_ptr<Texture>& texture,
                      const Vec2& pos, float scale);

    
#pragma mark Static Constructors
	/**
	 * Returns a newly allocated bubble generator at the origin.
	 *
     * The generator is scaled so that 1 pixel = 1 Box2d unit
	 *
	 * The attached scene graph is completely decoupled from the physics 
     * system. The node does not have to be the same size as the physics body. 
     * We only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
     *
     * @param texture   The bubbble texture
	 *
	 * @return a newly allocated bubble generator at the origin.
	 */
    static std::shared_ptr<BubbleGenerator> alloc(const std::shared_ptr<Texture>& texture) {
        std::shared_ptr<BubbleGenerator> result = std::make_shared<BubbleGenerator>();
        return (result->init(texture) ? result : nullptr);
    }

	/**
	 * Returns a newly allocated bubble generator with the given position.
	 *
	 * The generator is scaled so that 1 pixel = 1 Box2d unit
	 *
     * The attached scene graph is completely decoupled from the physics
     * system. The node does not have to be the same size as the physics body.
     * We only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
	 *
     * @param texture   The bubbble texture
	 * @param pos       Initial position in world coordinates
	 *
	 * @return a newly allocated bubble generator with the given position.
	 */
	static std::shared_ptr<BubbleGenerator> alloc(const std::shared_ptr<Texture>& texture,
                                                    const Vec2& pos) {
        std::shared_ptr<BubbleGenerator> result = std::make_shared<BubbleGenerator>();
        return (result->init(texture,pos) ? result : nullptr);
    }

	/**
	 * Returns a newly allocated generator with the given position and scale
	 *
	 * The scale is the ratio of drawing coordinates to physics coordinates.
	 * This allows us to construct the child objects appropriately.
 	 *
     * The attached scene graph is completely decoupled from the physics
     * system. The node does not have to be the same size as the physics body.
     * We only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
	 *
     * @param texture   The bubbble texture
     * @param pos       Initial position in world coordinates
     * @param scale     The drawing scale
	 *
	 * @return a newly allocated generator with the given position and scale
	 */
    static std::shared_ptr<BubbleGenerator> alloc(const std::shared_ptr<Texture>& texture,
                                                    const Vec2& pos, float scale) {
        std::shared_ptr<BubbleGenerator> result = std::make_shared<BubbleGenerator>();
        return (result->init(texture,pos,scale) ? result : nullptr);
    }


#pragma mark Bubble Management
	/**
	 * Returns the lifespan of a generated bubble.
	 *
	 * @return the lifespan of a generated bubble.
	 */
	int getLifeSpan() const { return _lifespan; }

	/**
	 * Sets the lifespan of a generated bubble.
	 *
	 * Changing this does not effect bubbles already generated.
	 *
	 * @param value the lifespan of a generated bubble.
	 */
	void setLifeSpan(int value) { _lifespan = value; }

    /**
   	 * Generates a new bubble object and puts it on the screen.
	 */
	void bubble();

	/**
	 * Returns true if we generated a bubble this animation frame.
	 *
	 * @return true if we generated a bubble this animation frame.
	 */
	bool didBubble() const { return _bubbled; }


#pragma mark Animation Support
    /**
	 * Updates the object's physics state (NOT GAME LOGIC).
	 *
	 * We use this method for cooldowns and bubble movement.  We also override
	 * the graphics support in WheelObstacle, as our shape behaves differently.
	 *
	 * @param  delta    number of seconds since last animation frame
	 */
	virtual void update(float dt) override;

    /**
	 * Sets the scene graph node representing this bubble generator.
	 *
	 * By storing a reference to the scene graph node, the model can update
	 * the node to be in sync with the physics info. It does this via the
	 * {@link Obstacle#update(float)} method.
	 *
	 * @param node  The scene graph node representing this bubble generator.
	 */
	void setGeneratorNode(const std::shared_ptr<cugl::scene2::SceneNode>& node) { _generatorNode = node; }

    /**
     * Returns the scene graph node representing this bubble generator.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this bubble generator.
     */
    std::shared_ptr<cugl::scene2::SceneNode> getGeneratorNode() const {
        return _generatorNode;
    }
    
    /**
     * Returns the texture for an individual bubble
     *
     * @return the texture for an individual bubble
     */
    std::shared_ptr<Texture> getTexture() const {
        return _texture;
    }

    /**
	 * Sets the ratio for drawing bubbles;
	 *
	 * The generator needs this value to convert correctly between the physics
	 * coordinates and the drawing screen coordinates.  Otherwise it will
	 * interpret one Box2D unit as one pixel.
 	 *
	 * All physics scaling must be uniform.  Rotation does weird things when
	 * attempting to scale physics by a non-uniform factor.
	 *
	 * @param scale The ratio for drawing bubbles in screen coordinates.
	 */
	void setDrawScale(float scale);
    
    /**
     * Returns the ratio for drawing bubbles
     *
     * The generator needs this value to convert correctly between the physics
     * coordinates and the drawing screen coordinates.  Otherwise it will
     * interpret one Box2D unit as one pixel.
     *
     * All physics scaling must be uniform.  Rotation does weird things when
     * attempting to scale physics by a non-uniform factor.
     *
     * @return the ratio for drawing bubbles
     */
    float getDrawScale() { return _drawscale; }
    
};

#endif /* __RG_BUBBLE_GENERATOR_H__ */
