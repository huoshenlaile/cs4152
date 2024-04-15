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
#include "interactable.h"

using namespace cugl;

class GravityReversePlatform : public Interactable {
protected:
    bool OnBeginContactEnabled = true;

    std::string _name;
    bool activated = true;
public:
    
    GravityReversePlatform(void): Interactable() {}

    virtual PublishedMessage onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false, std::shared_ptr<CharacterController> character = nullptr) override;

    static std::shared_ptr<GravityReversePlatform> alloc(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) {
        std::shared_ptr<GravityReversePlatform> result = std::make_shared<GravityReversePlatform>();
        return (result->init(json, scale, bounds) ? result : nullptr);
    }

    bool hasOnBeginContact() { return true; }
    void setOnBeginContact() { this -> OnBeginContactEnabled = true; }
    
    virtual bool init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) override;

};


#endif
