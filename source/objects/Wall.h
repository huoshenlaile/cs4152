#ifndef __WALL_H__
#define __WALL_H__

#include <cugl/cugl.h>
#include "interactable.h"

class Wall : public Interactable {
protected:
    std::string message_head;
    std::string show_subscription_head;
    std::string hide_subscription_head;
    
public:
    Wall(void) : Interactable() {}
    virtual ~Wall() {}

    virtual bool init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds);

    static std::shared_ptr<Wall> alloc(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds) {
        std::shared_ptr<Wall> result = std::make_shared<Wall>();
        return (result->init(json, scale, bounds) ? result : nullptr);
    }

    
};

#endif
