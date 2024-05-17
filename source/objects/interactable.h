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
#include "../helpers/GameSceneConstants.h"
#include "../helpers/LevelConstants.h"
#include "../controllers/CharacterController.h"
#include "message.h"

using namespace cugl;

class Interactable {
protected:
    bool timeUpdateEnabled = false;
    bool OnBeginContactEnabled = false;
    bool OnEndContactEnabled = false;
    bool OnPreSolveEnabled = false;
    bool OnPostSolveEnabled = false;
    bool canBeGrabbed = false;
    std::shared_ptr<cugl::scene2::ActionManager> _grab_actions_manager;
    std::shared_ptr<cugl::scene2::Animate> _grab_animate;
    std::shared_ptr<scene2::SpriteNode> _grab_animation;

    bool loadOnce = false;
    
    Rect _bounds;
    Vec2 _scale = Vec2(32, 32);
    float _scale_float;

    std::string _name = "";

    std::string _texture_name = "";
    bool activated = false;

    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    std::shared_ptr<cugl::scene2::SceneNode> _scene;

    std::shared_ptr<cugl::scene2::SceneNode> _selfTexture;
    std::shared_ptr<cugl::physics2::PolygonObstacle> _selfObstacle;

    std::shared_ptr<cugl::AssetManager> _assets;
    std::map<std::string, std::function<PublishedMessage(ActionParams)>> actions;

    std::vector<float> getVertices(const std::shared_ptr<JsonValue>& json);
    Vec2 getObjectPos(const std::shared_ptr<JsonValue>& json);
public:


    virtual std::shared_ptr<cugl::physics2::Obstacle> getObstacle() {
        return _selfObstacle;
    }

    virtual cugl::physics2::Obstacle* getObstacleRawPtr() {
        return _selfObstacle.get();
    }
    //Don't want to break existing behavior
    virtual std::shared_ptr<cugl::physics2::PolygonObstacle> getPolygonObstacle() {
        return _selfObstacle;
    }

    virtual bool isActivated() { return activated; }
    virtual void activate() { activated = true; }
    virtual void deactivate() { activated = false; }

    // various way of modifying the object or publish message
    virtual PublishedMessage timeUpdate(float timestep) { return PublishedMessage(); }
    virtual PublishedMessage onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false, std::shared_ptr<CharacterController> character = nullptr) { return PublishedMessage(); }
    virtual PublishedMessage onEndContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false, std::shared_ptr<CharacterController> character = nullptr) { return PublishedMessage(); }
    virtual PublishedMessage onPreSolve(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, const b2Manifold* oldManifold = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false, std::shared_ptr<CharacterController> character = nullptr) { return PublishedMessage(); }
    virtual PublishedMessage onPostSolve(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, const b2ContactImpulse* impulse = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false, std::shared_ptr<CharacterController> character = nullptr) { return PublishedMessage(); }

    bool hasTimeUpdate() { return timeUpdateEnabled; }
    bool hasOnBeginContact() { return OnBeginContactEnabled; }
    bool hasOnEndContact() { return OnEndContactEnabled; }
    bool hasOnPreSolve() { return OnPreSolveEnabled; }
    bool hasOnPostSolve() { return OnPostSolveEnabled; }
    bool loadOnlyOnInitial() { return loadOnce; }

    std::string getName() { return _name; }
    const std::map<std::string, std::function<PublishedMessage(ActionParams)>>& getActions() { return actions; }

    virtual bool init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds);
    virtual bool bindAssets(const std::shared_ptr<cugl::AssetManager>& assets, Vec2 scale2d);
    virtual bool linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode> &sceneNode, float scale);
    virtual void setup() {}


    // initialize the interactable object
    Interactable(void) {}
    virtual ~Interactable() {
        // set everything to nullptr
//        _world = nullptr; //this crashed app when exiting from level so commented out for now...
        _scene = nullptr;
        _assets = nullptr;
        _selfTexture = nullptr;
        _selfObstacle = nullptr;
    }

 

    // static allocator
    static std::shared_ptr<Interactable> alloc(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) {
        std::shared_ptr<Interactable> result = std::make_shared<Interactable>();
        return (result->init(json, scale, bounds) ? result : nullptr);
    }

    // static converter
    static ActionParams ConvertToActionParams(const PublishedMessage& message) {
    ActionParams params;
    params.enable = message.enable;
    params.int1 = message.int1;
    params.int2 = message.int2;
    params.float1 = message.float1;
    params.float2 = message.float2;
    params.id = message.id;
    params.Head = message.Head;
    params.Body = message.Body;
    return params;
}
    
    virtual bool isGrabbable() {
        return canBeGrabbed;
    }
    
    virtual std::shared_ptr<cugl::scene2::ActionManager> getGrabActionsManager(){
        return _grab_actions_manager;
    }
    
    virtual std::shared_ptr<cugl::scene2::Animate> getGrabAnimate(){
        return _grab_animate;
    }
    
    virtual std::shared_ptr<scene2::SpriteNode> getGrabAnimation(){
        return _grab_animation;
    }
    virtual float getScale(){
        return _scale_float;
    }

};


#endif
