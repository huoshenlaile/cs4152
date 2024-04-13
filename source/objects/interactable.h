#ifndef __INTERACTABLE_H__
#define __INTERACTABLE_H__

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

// TODO placeholder for unified arguments struct
struct ActionParams {
    int int1;
    int int2;
    float float1;
    float float2;
    std::string str1;
    std::string str2;
};

struct PublishedMessage {
    bool enable;
    int int1;
    int int2;
    float float1;
    float float2;
    bool success;
    std::string message;
};



class Interactable {

public:
    std::string _name;

    std::shared_ptr<cugl::scene2::SceneNode> _node;
    std::shared_ptr<cugl::physics2::Obstacle> _obstacle;
    std::map<std::string, std::function<PublishedMessage(ActionParams)>> actions;

    physics2::Obstacle* getObstacleRawPtr() {
        return _obstacle.get();
    }

    bool activated = false;

    virtual bool isActivated() { return activated; }
    virtual void activate() { activated = true; }
    virtual void deactivate() { activated = false; }

    // various way of modifying the object or publish message
    virtual PublishedMessage timeUpdate(float timestep);
    virtual PublishedMessage onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false);
    virtual PublishedMessage onEndContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false);
    virtual PublishedMessage onPreSolve(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, const b2Manifold* oldManifold = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false);
    virtual PublishedMessage onPostSolve(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, const b2ContactImpulse* impulse = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false);

    bool timeUpdateEnabled = false;
    bool OnBeginContactEnabled = false;
    bool OnEndContactEnabled = false;
    bool OnPreSolveEnabled = false;
    bool OnPostSolveEnabled = false;

    bool hasTimeUpdate() { return timeUpdateEnabled; }
    bool hasOnBeginContact() { return OnBeginContactEnabled; }
    bool hasOnEndContact() { return OnEndContactEnabled; }
    bool hasOnPreSolve() { return OnPreSolveEnabled; }
    bool hasOnPostSolve() { return OnPostSolveEnabled; }

    Interactable() {}
    ~Interactable() {}


};


#endif