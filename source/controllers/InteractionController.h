#ifndef __INTERACTION_CONTROLLER_H__
#define __INTERACTION_CONTROLLER_H__

#include <cugl/cugl.h>
#include <stdio.h>
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include "NetworkController.h"
#include "../models/WallModel.h"
#include "../models/PlatformModel.h"
#include "../models/ButtonModel.h"
#include "../models/CharacterModel.h"


using namespace cugl::physics2::net;

class InteractionController {
protected:
    /** Network Controller? */
    NetEventController _netController;
    // TODO: Interaction Controller SHOULD have access to models. Models should be passed in as parameters in initializers.
    
public:

    /** the TENTATIVE initializer for this (feel free to change), according to our arch spec
     */
    bool init(std::vector<std::shared_ptr<PlatformModel>> platforms,                
              std::shared_ptr<CharacterModel> character,
              std::vector<std::shared_ptr<ButtonModel>> buttons,
              std::vector<std::shared_ptr<WallModel>> walls);
    
    
    
#pragma mark Collision Handling
    /**
     * Processes the start of a collision
     *
     * This method is called when we first get a collision between two objects.
     * We use this method to test if it is the "right" kind of collision.  In
     * particular, we use it to test if we make it to the win door.
     *
     * @param  contact  The two bodies that collided
     */
    void beginContact(b2Contact* contact);

    /**
     * Handles any modifications necessary before collision resolution
     *
     * This method is called just before Box2D resolves a collision.  We use
     * this method to implement sound on contact, using the algorithms outlined
     * in Ian Parberry's "Introduction to Game Physics with Box2D".
     *
     * @param  contact  The two bodies that collided
     * @param  contact  The collision manifold before contact
     */
    void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);
};

#endif /* InteractionController_h */
