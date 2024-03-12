//  NetworkController.h
//  Dusty Paints
//
//  This class represents an event of player grabing an object
//
//  Created by Esther Yu  on 03/12/24.
//

#ifndef __NETWORK_CONTROLLER_H__
#define __NETWORK_CONTROLLER_H__
#include <cugl/cugl.h>

using namespace cugl::physics2::net;
using namespace cugl;

class GrabEvent : public NetEvent {
protected:
    LWSerializer _serializer;
    LWDeserializer _deserializer;
    Vec2 _pos;
    
public:
    /**
     * This method is used by the NetEventController to create a new event of using a
     * reference of the same type.
     *
     * Not that this method is not static, it differs from the static alloc() method
     * and all methods must implement this method.
     */
    std::shared_ptr<NetEvent> newEvent() override;
    
    static std::shared_ptr<NetEvent> allocGrabEvent(Vec2 pos);
    
    /**
     * Serialize any paramater that the event contains to a vector of bytes.
     */
    std::vector<std::byte> serialize() override;
    /**
     * Deserialize a vector of bytes and set the corresponding parameters.
     *
     * @param data  a byte vector packed by serialize()
     *
     * This function should be the "reverse" of the serialize() function: it
     * should be able to recreate a serialized event entirely, setting all the
     * useful parameters of this class.
     */
    void deserialize(const std::vector<std::byte>& data) override;
    
    /** Gets the position of the event. */
    Vec2 getPos() { return _pos; }
};


#endif /* NetworkController_h */
