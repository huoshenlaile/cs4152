#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <cugl/cugl.h>
// TODO placeholder for unified arguments struct
struct ActionParams {
    bool enable;
    int int1;
    int int2;
    float float1;
    float float2;
    int id;
    std::string Head;
    std::string Body;
};

struct PublishedMessage {
    bool enable;
    int int1;
    int int2;
    float float1;
    float float2;
    int id;
    std::string Head;
    std::string Body;
};

#endif