//
//  RGBubbleGenerator.cpp
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
#include "RGBubbleGenerator.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/assets/CUAssetManager.h>

#pragma mark Particles

/** The texture of an individual bubble */
std::shared_ptr<Texture> BubbleGenerator::Particle::texture;

/** Creates a new Particle with no lifespan */
BubbleGenerator::Particle::Particle() : life(-1) {
    node = scene2::PolygonNode::allocWithTexture(texture);
}

/** Disposes of this particle, permanently deleting all resources */
BubbleGenerator::Particle::~Particle() {
	node = nullptr;
}

/**
 * Updates the particle based on drawing scale
 *
 * @param  scale    the current drawing scale
 */
void BubbleGenerator::Particle::update() {
	Vec2 pos(position.x, position.y);
	node->setPosition(pos);
}

#pragma mark -
#pragma mark Constructors

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
bool BubbleGenerator::init(const std::shared_ptr<Texture>& texture,
                           const Vec2& pos, float scale) {
	WheelObstacle::init(pos, BUBBLE_RADIUS);
	setDensity(1);
	_lifespan = DEFAULT_LIFE;
	_cooldown = 0;

	_texture = texture;
    Particle::texture = texture;
	_memory  = GreedyFreeList<Particle>::alloc(MAX_PARTICLES);

	return true;
}

/**
 * Destroys this bubble generator, releasing all resources.
 */
BubbleGenerator::~BubbleGenerator() {
	_memory  = nullptr;
	_texture = nullptr;
}


#pragma mark -
#pragma mark Bubble Management

/**
 * Generates a new bubble object and puts it on the screen.
 */
void BubbleGenerator::bubble() {
	Particle* p = _memory->malloc();
	if (p != nullptr) {
		// Convert the position to the position in the world's coordinate system
		p->position.set(getPosition() * _drawscale);
		p->life = _lifespan;
		p->update();
		// Add the bubble to the world node
        // Makes it so position is not affected by the position of the Ragdoll
		_generatorNode->getScene()->getChildByName("world")->addChild(p->node);
		_activeParticles.push_back(p);
	}
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method for cooldowns and bubble movement.  We also override
 * the graphics support in WheelObstacle, as our shape behaves differently.
 *
 * @param  delta    number of seconds since last animation frame
 */
void BubbleGenerator::update(float dt) {
  for (Particle* p : _activeParticles) {
		if (p->life > 0) {
			p->position.y += 0.5f;
			p->life -= 1;
			if (p->life == 0) {
				_memory->free(p);
				// Remove the bubble from the world node
				_generatorNode->getScene()->getChildByName("world")->removeChild(p->node);
			}
			else {
				p->update();
			}
		}
  }
  
  // Remove inactive particles after iterating
  _activeParticles.erase(std::remove_if(_activeParticles.begin(), _activeParticles.end(),
                       [](Particle* p) { return p->life == 0; }), _activeParticles.end());
                       

	if (_cooldown == 0) {
		_bubbled = true;
		bubble();
		_cooldown = BUBBLE_TIME;
	}
	else {
		_bubbled = false;
		_cooldown--;
	}
	updateDebug();
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
void BubbleGenerator::setDrawScale(float scale) {
	_drawscale = scale;
}
