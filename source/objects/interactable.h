#ifndef __INTERACTABLE_H__
#define __INTERACTABLE_H__

#include <cugl/cugl.h>

// TODO placeholder for unified arguments struct
struct ActionParams {
    int int1;
    int int2;
    int int3;
    float float1;
    float float2;
    float float3;
    std::string str1;
    std::string str2;
    std::string str3;
};

struct ResultParams {
    bool success;
    std::string message;
};


class Interactable {

public:
    std::shared_ptr<cugl::Texture> _texture;
    std::shared_ptr<cugl::physics2::Obstacle> _obstacle;
    std::map<std::string, std::function<ResultParams(ActionParams)>> actions;

    Interactable() {}

};


#endif