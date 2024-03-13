//
//  PauseEvent.cpp
//  Prototype1
//
//  Created by Esther Yu on 3/12/24.
//


#include "PauseEvent.h"

/**
 * This method is used by the NetEventController to create a new event of using a
 * reference of the same type.
 *
 * Not that this method is not static, it differs from the static alloc() method
 * and all methods must implement this method.
 */
std::shared_ptr<NetEvent> PauseEvent::newEvent(){
    return std::make_shared<PauseEvent>();
}

std::shared_ptr<NetEvent> PauseEvent::allocPauseEvent(Vec2 pos){
    auto event = std::make_shared<PauseEvent>();
    event->_pos = pos;
    return event;
}

/**
 * Serialize any paramater that the event contains to a vector of bytes.
 */
std::vector<std::byte> PauseEvent::serialize(){
    _serializer.reset();
    _serializer.writeFloat(_pos.x);
    _serializer.writeFloat(_pos.y);
    return _serializer.serialize();
}
/**
 * Deserialize a vector of bytes and set the corresponding parameters.
 *
 * @param data  a byte vector packed by serialize()
 *
 * This function should be the "reverse" of the serialize() function: it
 * should be able to recreate a serialized event entirely, setting all the
 * useful parameters of this class.
 */
void PauseEvent::deserialize(const std::vector<std::byte>& data){
    _deserializer.reset();
    _deserializer.receive(data);
    float x = _deserializer.readFloat();
    float y = _deserializer.readFloat();
    _pos = Vec2(x,y);
}



