#ifndef __INTERACTABLE_H__
#define __INTERACTABLE_H__

#include <cugl/cugl.h>
#include <stdio.h>
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include <box2d/b2_distance.h>
#include <cugl/io/CUJsonReader.h>
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

using namespace cugl;

class Interactable {
protected:
    bool timeUpdateEnabled = false;
    bool OnBeginContactEnabled = false;
    bool OnEndContactEnabled = false;
    bool OnPreSolveEnabled = false;
    bool OnPostSolveEnabled = false;
    std::string _name;

    std::string _texture_name;
    bool activated = false;

    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    std::shared_ptr<cugl::scene2::SceneNode> _scene;

    std::shared_ptr<cugl::scene2::SceneNode> _selfTexture;
    std::shared_ptr<cugl::physics2::Obstacle> _selfObstacle;

    std::shared_ptr<cugl::AssetManager> _assets;
    std::map<std::string, std::function<PublishedMessage(ActionParams)>> actions;

public:

    cugl::physics2::Obstacle* getObstacleRawPtr() {
        return _selfObstacle.get();
    }


    virtual bool isActivated() { return activated; }
    virtual void activate() { activated = true; }
    virtual void deactivate() { activated = false; }

    // various way of modifying the object or publish message
    virtual PublishedMessage timeUpdate(float timestep);
    virtual PublishedMessage onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false);
    virtual PublishedMessage onEndContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false);
    virtual PublishedMessage onPreSolve(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, const b2Manifold* oldManifold = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false);
    virtual PublishedMessage onPostSolve(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, const b2ContactImpulse* impulse = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false);

    bool hasTimeUpdate() { return timeUpdateEnabled; }
    bool hasOnBeginContact() { return OnBeginContactEnabled; }
    bool hasOnEndContact() { return OnEndContactEnabled; }
    bool hasOnPreSolve() { return OnPreSolveEnabled; }
    bool hasOnPostSolve() { return OnPostSolveEnabled; }

    std::string getName() { return _name; }
    std::map<std::string, std::function<PublishedMessage(ActionParams)>> getActions() { return actions; }

    bool bindAssets(const std::shared_ptr<cugl::AssetManager>& assets);
    bool linkToWorld(const std::shared_ptr<cugl::physics2::net::NetWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode> &sceneNode, float scale);

    // initialize the interactable object
    Interactable();
    virtual ~Interactable();

    bool init(const std::shared_ptr<JsonValue>& json, Vec2 scale = Vec2(32,32));

    // static allocator
    static std::shared_ptr<Interactable> alloc(const std::shared_ptr<JsonValue>& json, Vec2 scale = Vec2(32,32)) {
        std::shared_ptr<Interactable> result = std::make_shared<Interactable>();
        return (result->init(json, scale) ? result : nullptr);
    }

};


#endif
