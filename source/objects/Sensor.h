#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <cugl/cugl.h>
#include "interactable.h"

class Sensor : public Interactable {
protected:
    std::string message_head;

public:
    Sensor(void) : Interactable() {}
    ~Sensor() {
        // Call the parent class destructor
        Interactable::~Interactable();
    }

    virtual bool init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds);

    virtual PublishedMessage onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false, std::shared_ptr<CharacterController> character = nullptr) override;

    static std::shared_ptr<Sensor> alloc(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds) {
        std::shared_ptr<Sensor> result = std::make_shared<Sensor>();
        return (result->init(json, scale, bounds) ? result : nullptr);
    }

    
};

#endif
