//
//  DPCharacterController.cpp
//  Prototype1
//
//  Created by Emily on 3/9/24.
//

#include <cugl/cugl.h>
#include "DPCharacterController.h"
 
using namespace cugl;
float DOLL_POS[] = { 16, 10 };

CharacterController::CharacterController() {
}

void CharacterController::populate(const std::shared_ptr<cugl::scene2::SceneNode>& _worldnode,
    const std::shared_ptr<cugl::physics2::net::NetWorld>& _world,
const std::shared_ptr<cugl::AssetManager>& _assets){
    // Allocate the ragdoll and set its (empty) node. Its model handles creation of parts
    // (both obstacles and nodes to be drawn) upon alllocation and setting the scene node.

    _ragdoll = RagdollModel::alloc(DOLL_POS, _scale);
    _ragdoll->buildParts(_assets);
    _ragdoll->createJoints();
    
    auto ragdollNode = scene2::SceneNode::alloc();
    // Add the ragdollNode to the world before calling setSceneNode,
    // as noted in the documentation for the Ragdoll's method.
    _worldnode->addChild(ragdollNode);
    _ragdoll->setDrawScale(_scale);
    
    _ragdoll->linkPartsToWorld(_world, ragdollNode, _scale);
}

bool CharacterController::init(const std::shared_ptr<InputController>& input, float scale) {
    _input = input;
    _scale = scale;
    return true;
}

void CharacterController::updatePositionFromInput(const Scene2& scene){
    int knob_radius = 200;
    std::shared_ptr<cugl::physics2::Obstacle> leftArm = _ragdoll->getPartObstacle(PART_LEFT_HAND);
    std::shared_ptr<cugl::physics2::Obstacle> rightArm = _ragdoll->getPartObstacle(PART_RIGHT_HAND);
    std::unordered_map<std::string,cugl::Vec2> inputs = _input->getPosition();
    for (const auto & [ key, value ] : inputs) {
        if (_input_to_arm.count(key) == 0){ // Add touchpoint
            cugl::Vec2 pos2d = ((cugl::Vec2)scene.screenToWorldCoords(value));
            if(_arm_to_input.count(PART_LEFT_HAND) == 0 && (leftArm->getPosition() * _scale).distance(pos2d) < knob_radius){
                _input_to_arm[key] = PART_LEFT_HAND;
                _arm_to_input[PART_LEFT_HAND] = key;
                
            }
            else if(_arm_to_input.count(PART_RIGHT_HAND) == 0 && (rightArm->getPosition() * _scale).distance(pos2d) < knob_radius){
                _input_to_arm[key] = PART_RIGHT_HAND;
                _arm_to_input[PART_RIGHT_HAND] = key;
            }
        }
        else{ // Do stuff with existing touchpoint
            cugl::Vec2 pos_now =  ((cugl::Vec2)scene.screenToWorldCoords(value));
            std::shared_ptr<cugl::physics2::Obstacle> arm = _ragdoll->getPartObstacle(_input_to_arm[key]);
            arm->setPosition(pos_now/_scale);
        }
    }
    
    auto it = _arm_to_input.begin();
    while (it != _arm_to_input.end()){ // Remove old touchpoints
        auto id = it->second;
        if (inputs.count(id)==0){
            it = _arm_to_input.erase(it);
            _input_to_arm.erase(id);
        }
        else{
            ++it;
        }
    }
    
}

void CharacterController::dispose(){
    _input = nullptr;
    _ragdoll =nullptr;
}
