//
//  GravityReversePlatform.h
//  DustyPaints
//
//  Created by Haoxuan Zou on 4/14/24.
//

#ifndef GravityReversePlatform_h
#define GravityReversePlatform_h

#include <stdio.h>

#include <cugl/cugl.h>
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include <box2d/b2_distance.h>
#include "interactable.h"

using namespace cugl;

class GravityReversePlatform : public Interactable {
protected:
    bool timeUpdateEnabled = false;
    bool OnBeginContactEnabled = false;
    bool OnEndContactEnabled = false;
    bool OnPreSolveEnabled = false;
    bool OnPostSolveEnabled = false;
    bool canBeGrabbed = false;

    Rect _bounds;
    Vec2 _scale = Vec2(32, 32);

    std::string _name;

    std::string _texture_name;
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

    PublishedMessage onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false, std::shared_ptr<CharacterController> character = nullptr) override;

    static std::shared_ptr<GravityReversePlatform> alloc(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) {
        std::shared_ptr<GravityReversePlatform> result = std::make_shared<GravityReversePlatform>();
        return (result->init(json, scale, bounds) ? result : nullptr);
    }
    
    bool hasTimeUpdate() { return timeUpdateEnabled; }
    bool hasOnBeginContact() { return OnBeginContactEnabled; }
    bool hasOnEndContact() { return OnEndContactEnabled; }
    bool hasOnPreSolve() { return OnPreSolveEnabled; }
    bool hasOnPostSolve() { return OnPostSolveEnabled; }

    std::string getName() { return _name; }
    const std::map<std::string, std::function<PublishedMessage(ActionParams)>>& getActions() { return actions; }


    bool init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) override;

};


#endif
