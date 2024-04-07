#ifndef __INTERACTION_CONTROLLER_H__
#define __INTERACTION_CONTROLLER_H__

#include <cugl/cugl.h>
#include <stdio.h>
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include <box2d/b2_distance.h>
#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include "NetworkController.h"
#include "../helpers/LevelLoader.h" //Need to include in dependency graph
#include "../models/WallModel.h"
#include "../models/PlatformModel.h"
#include "../models/ButtonModel.h"
#include "../models/ExitModel.h"
#include "CharacterController.h"


using namespace cugl::physics2::net;

#define PLAYER_ONE 1
#define PLAYER_TWO 2
#define NOT_PLAYER 0

class InteractionController {
protected:
    /** Network Controller? */
    std::shared_ptr<NetEventController> _netController;
    // TODO: Interaction Controller SHOULD have access to models. Models should be passed in as parameters in initializers.
    std::shared_ptr<CharacterController> _characterControllerA;
    std::shared_ptr<CharacterController> _characterControllerB;
    
    std::vector<std::shared_ptr<PlatformModel>> _platforms;
    std::vector<std::shared_ptr<ButtonModel>> _buttons;
    std::vector<std::shared_ptr<WallModel>> _walls;
    std::shared_ptr<LevelLoader> _level;
    std::shared_ptr<cugl::JsonValue> _levelJson;
    std::shared_ptr<cugl::physics2::net::NetWorld> _world;
    
    /**
     This vector stores TWO obstacles to create joint in between.
     The reason why this is NOT a smart pointer vector is because this vector
     will be emptied very frequently. For unknown reason, every time I empty it, it tries to
     destroy the smart pointer, hence destroying the obstacle. I don't want that.
     */
    std::vector<physics2::Obstacle*> _obstaclesForJoint;

    
    struct PlayerCounter {
        int bodyOne = NOT_PLAYER;
        int bodyTwo = NOT_PLAYER;
        bool bodyOneIsHand = false;
        bool bodyTwoIsHand = false;
    };
    
    PlayerCounter checkContactForPlayer(b2Body* body1, b2Body* body2);
    
public:
    struct PublisherMessage {
        /**
            the Publisher Message matches with the Subscriber Message via the pub id and the message (which is the listeningFor field in Subscriber Message).
         */
          std::string pub_id;
          std::string trigger;
          std::string message;
          std::unordered_map<std::string, std::string> body;
    };
    
    struct SubscriberMessage {
          std::string pub_id;
          std::string listening_for;
          std::unordered_map<std::string, std::string> actions;
    };
    
    std::queue<PublisherMessage> messageQueue;
    //subscriptions[pub_id][listening_for]
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<SubscriberMessage>>> subscriptions;
    
    //publications[trigger][pub_id]
    std::unordered_map<std::string, std::unordered_map<std::string, PublisherMessage>> publications;

    /** the TENTATIVE initializer for this (feel free to change), according to our arch spec
     */
    bool init(std::vector<std::shared_ptr<PlatformModel>> platforms,                
              std::shared_ptr<CharacterController> characterA,
              std::shared_ptr<CharacterController> characterB,
              std::vector<std::shared_ptr<ButtonModel>> buttons,
              std::vector<std::shared_ptr<WallModel>> walls,
              std::shared_ptr<LevelLoader> level,
              const std::shared_ptr<cugl::JsonValue>& json);
    
    
    //TODO: Make this method protected, but public right now for testing
    void publishMessage(PublisherMessage& message);
/** 
 * Creates a new subscription
 *
 * @param message  The subscriber message
 *
 * @return true if the publisher exists and the subscriber was added, false otherwise
 */
    bool addSubscription(SubscriberMessage& message);
    
/**
 * Creates a new publication
 *
 * @param message  The publisher message
 *
 * @return true if the publisher was added, false otherwise
 */
    bool addPublisher(PublisherMessage& message);

    
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
    
    void endContact(b2Contact* contact);

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
    
    void detectPolyContact(const std::shared_ptr<scene2::PolygonNode>& poly2, float scale);
    
    /**
     * Handles message subscription processing
     * @param  dt  timestep
     */
    void preUpdate(float dt);
    
    /**
     * This function creates a revolute joint between two obstacles in the _obstaclesForJoint_ vector.
     * It clears the vector everytime it creates a joint.
     */
    void connectGrabJoint();
};

#endif /* InteractionController_h */
