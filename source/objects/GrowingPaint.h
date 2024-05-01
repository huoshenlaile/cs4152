#ifndef __GROWING_PAINT_H__
#define __GROWING_PAINT_H__

#include <cugl/cugl.h>
#include "interactable.h"

class GrowingPaint : public Interactable {
protected:
    std::string sub_message_head;
    std::string pub_message_head;
    std::string color;
    bool _initial_splash_done = false;
    bool is_out = false;
    float angle_splash = 0.0;
    
    std::shared_ptr<scene2::SpriteNode> _animation;
    std::string _textureName;
    std::shared_ptr<cugl::scene2::ActionManager> _actions;
    std::shared_ptr<cugl::scene2::Animate> _animate;
    
public:
    GrowingPaint(void) : Interactable() {}
    ~GrowingPaint() {
    }

    virtual bool init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) override;
    
    virtual PublishedMessage timeUpdate(float) override;
    
    virtual bool linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode> &sceneNode, float scale) override;

    virtual PublishedMessage onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false, std::shared_ptr<CharacterController> character = nullptr) override;
    
    static std::shared_ptr<GrowingPaint> alloc(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds) {
        std::shared_ptr<GrowingPaint> result = std::make_shared<GrowingPaint>();
        return (result->init(json, scale, bounds) ? result : nullptr);
    }

};

#endif
