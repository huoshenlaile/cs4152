#ifndef __EXITDOOR_H__
#define __EXITDOOR_H__

#include <cugl/cugl.h>
#include "interactable.h"

class Exitdoor : public Interactable {

    Exitdoor() : Interactable() {}
    ~Exitdoor() {}

    virtual bool init(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale = Vec2(32, 32));

    virtual PublishedMessage onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false);
    
    static std::shared_ptr<Exitdoor> alloc(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale = Vec2(32, 32)) {
        std::shared_ptr<Exitdoor> result = std::make_shared<Exitdoor>();
        return (result->init(json, scale) ? result : nullptr);
    }
};

#endif