#ifndef __INTERACTABLEUI_H__
#define __INTERACTABLEUI_H__

#include <cugl/cugl.h>
#include "interactable.h"

class InteractableUI : public Interactable {
protected:
    std::string show_subscription_head;
    std::string hide_subscription_head;
    std::string toggle_subscription_head;
    float _show_cd;
    float _hide_cd;
    float _cd;


    bool _showing = false;
    bool _animated = false;
    bool _repeat = true;
    std::string _textureName;
    std::string _aniname;
    int _cells;
    int _rows;
    int _cols;
    float _scale;
    bool _shown_already=false;
    
    bool _showCDTimerTriggered = false;
    std::shared_ptr<cugl::scene2::ActionManager> _actions;
    std::shared_ptr<cugl::scene2::Animate> _animate;
    std::shared_ptr<scene2::SpriteNode> _animation;


public:
    InteractableUI(void) : Interactable() {}
    ~InteractableUI() {
    }

    virtual bool init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) override;

    static std::shared_ptr<InteractableUI> alloc(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds) {
        std::shared_ptr<InteractableUI> result = std::make_shared<InteractableUI>();
        return (result->init(json, scale, bounds) ? result : nullptr);
    }
    
    bool linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode>&, float scale) override;
    
    virtual PublishedMessage timeUpdate(float) override;

    
};

#endif
