#ifndef __CHARACTER_CONTROLLER_H__
#define __CHARACTER_CONTROLLER_H__

#include <cugl/cugl.h>
#include <vector>
#include "../models/CharacterModel.h"




/*
 
 O--o--oo--o--o()o--o--oo--o--O
 
 
 
 */

#define PART_NONE   -1
#define PART_BODY   0
#define PART_JR1    1
#define PART_JR2    2
#define PART_JR3    3
#define PART_JR4    4
#define PART_JR5    5
#define PART_RH     6
#define PART_LR1    7
#define PART_LR2    8
#define PART_LR3    9
#define PART_LR4    10
#define PART_LR5    11
#define PART_LH     12

#define BODY_TEXTURE "body1"
#define CJOINT_TEXTURE "j1"
#define HAND_TEXTURE "hand1"

#define CJOINT_OFFSET 1.0f


#pragma mark -
#pragma mark Physics Constants
/** The density for each body part */
#define DEFAULT_DENSITY  1.0f
/** The density for the center of mass */
#define CENTROID_DENSITY 0.1f
/** The radius for the center of mass */
#define CENTROID_RADIUS  0.1f


using namespace cugl;

class CharacterController {
protected:
    std::string _name;
    std::vector<std::shared_ptr<cugl::Texture>> _textures;
    std::vector<std::shared_ptr<cugl::physics2::Obstacle>> _obstacles;
    std::vector<std::shared_ptr<cugl::physics2::Joint>> _joints;
    
    cugl::Vec2 _offset;
    std::shared_ptr<cugl::scene2::SceneNode> _node;
    cugl::Mat4 _affine;
    float _drawScale;
    
    CharacterController(void) : _drawScale(0) { }
    virtual ~CharacterController(void) { dispose(); }
    void dispose();
    
    std::string getPartName(int part);
    
    std::shared_ptr<cugl::physics2::BoxObstacle> makePart(int part, int connect, const cugl::Vec2& pos);
    
    bool init() {
        return init(cugl::Vec2::ZERO, 1.0f);
    }
    
    bool init(const cugl::Vec2& pos) {
        return init(pos,1.0f);
    }
    
    bool init(const cugl::Vec2& pos, float scale);
    
#pragma mark -
#pragma mark Static Constructors
    
    static std::shared_ptr<CharacterController> alloc() {
        std::shared_ptr<CharacterController> result = std::make_shared<CharacterController>();
        return (result->init() ? result : nullptr);
    }
    static std::shared_ptr<CharacterController> alloc(const cugl::Vec2& pos) {
        std::shared_ptr<CharacterController> result = std::make_shared<CharacterController>();
        return (result->init(pos, 1.0f) ? result : nullptr);
    }
    static std::shared_ptr<CharacterController> alloc(const cugl::Vec2& pos, float scale) {
        std::shared_ptr<CharacterController> result = std::make_shared<CharacterController>();
        return (result->init(pos, scale) ? result : nullptr);
    }
    
    void activate(const std::shared_ptr<cugl::physics2::net::NetWorld>& world);
    void deactivate(const std::shared_ptr<cugl::physics2::net::NetWorld>& world);
    
    bool createJoints();
    bool buildParts(const std::shared_ptr<cugl::AssetManager>& assets);
    
    void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node);
    void setDrawScale(float scale);
    
    bool moveRightHand(cugl::Vec2 offset);
    bool moveLeftHand(cugl::Vec2 offset);
};

#endif /* CharacterController_h */
