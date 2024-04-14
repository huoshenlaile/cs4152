#ifndef __INTERACTION_CONTROLLER_2_H__
#define __INTERACTION_CONTROLLER_2_H__

#include <cugl/cugl.h>
#include <stdio.h>
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include <box2d/b2_distance.h>

#include "../objects/objectImport.h"
#include "../helpers/LevelLoader2.h"
#include "../controllers/CharacterController.h"

using namespace cugl;

class InteractionController2 {

protected:

    std::shared_ptr<LevelLoader2> _level;
    std::shared_ptr<CharacterController> _character;

    //all the interactables in the level (including the wall, which is "static" interactable)
    std::vector<std::shared_ptr<Interactable>> _interactables;

    // 5 containers for different types of interactables
    std::vector<std::shared_ptr<Interactable>> _timeUpdateInteractables;
    std::unordered_map<cugl::physics2::Obstacle*, std::shared_ptr<Interactable>> _BeginContactInteractable;
    std::unordered_map<cugl::physics2::Obstacle*, std::shared_ptr<Interactable>> _EndContactInteractable;
    std::unordered_map<cugl::physics2::Obstacle*, std::shared_ptr<Interactable>> _PreSolveInteractable;
    std::unordered_map<cugl::physics2::Obstacle*, std::shared_ptr<Interactable>> _PostSolveInteractable;

    // message to subscriber map
    std::unordered_map<std::string, std::vector<std::shared_ptr<Interactable>>> _HeadToInteractable;


    // physics world and render world for convenience
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    std::shared_ptr<cugl::scene2::SceneNode> _worldnode;
    
    physics2::Obstacle* characterLHRawPtr;
    physics2::Obstacle* characterRHRawPtr;
    physics2::Obstacle* characterBODYRawPtr;
    std::shared_ptr<cugl::physics2::Obstacle> characterLH;
    std::shared_ptr<cugl::physics2::Obstacle> characterRH;
    std::shared_ptr<cugl::physics2::Obstacle> characterBODY;

    // map from raw obstacle pointer to interactable
    std::unordered_map<cugl::physics2::Obstacle*, std::shared_ptr<Interactable>> _obstacleToInteractable;


    // message queue
    std::queue<PublishedMessage> _messageQueue;

    bool _levelComplete = false;
    void runMessageQueue();
    bool isCharacterObs(cugl::physics2::Obstacle* obs) {
        return obs == characterLHRawPtr || obs == characterRHRawPtr || obs == characterBODYRawPtr;
    }
    std::shared_ptr<cugl::physics2::Obstacle> characterRawPtrToObstacle(physics2::Obstacle* rawPtr) {
        if (rawPtr == characterLHRawPtr) {
            return characterLH;
        }
        else if (rawPtr == characterRHRawPtr) {
            return characterRH;
        }
        else if (rawPtr == characterBODYRawPtr) {
            return characterBODY;
        }
        return nullptr;
    }
public:
    InteractionController2() {}
    ~InteractionController2() {
        // set all pointers to nullptr
        _level = nullptr;
        _character = nullptr;
        _interactables.clear();
        _timeUpdateInteractables.clear();
        _BeginContactInteractable.clear();
        _EndContactInteractable.clear();
        _PreSolveInteractable.clear();
        _PostSolveInteractable.clear();
        _HeadToInteractable.clear();
        _world = nullptr;
        _worldnode = nullptr;
    }

    bool init(std::shared_ptr<LevelLoader2> level);
    void activateController();

    void preUpdate(float timestep);
    void postUpdate(float timestep);

    void isLevelComplete() { return _levelComplete; }

    // 4 callback functions to add to the world
    void beginContact(b2Contact* contact);
    void endContact(b2Contact* contact);
    void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);
    void afterSolve(b2Contact* contact, const b2ContactImpulse* impulse);

    static std::shared_ptr<InteractionController2> alloc(std::shared_ptr<LevelLoader2> level) {
        std::shared_ptr<InteractionController2> result = std::make_shared<InteractionController2>();
        return (result->init(level) ? result : nullptr);
    }

};

#endif