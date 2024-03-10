//
//  DPCharacterController.hpp
//  Prototype1
//
//  Created by Emily on 3/9/24.
//

#ifndef DPCharacterController_hpp
#define DPCharacterController_hpp

#include <stdio.h>
#include "GLInputController.h"
#include "RGRagdollModel.h"

#pragma mark -
#pragma mark Body Part Indices
/** Indices for the body parts in the bodies array */
#define PART_NONE           -1
#define PART_BODY           0
//#define PART_HEAD           1
#define PART_LEFT_ARM       1
#define PART_RIGHT_ARM      2
#define PART_LEFT_FOREARM   3
#define PART_RIGHT_FOREARM  4
#define PART_LEFT_HAND      5
#define PART_RIGHT_HAND     6

class CharacterController {
private:
    std::shared_ptr<InputController> _input;
    float _scale;
    std::unordered_map<std::string,int> _input_to_arm;
    /** Relates arm to input id*/
    std::unordered_map<int, std::string> _arm_to_input;

public:
    std::shared_ptr<RagdollModel> _ragdoll;

    /**
     * Creates a new character controller.
     *
     * This constructor DOES NOT attach any listeners, as we are not
     * ready to do so until the scene is created. You should call
     * the {@link #init} method to initialize the scene.
     */
    CharacterController();

    /**
     * Deletes this character controller, releasing all resources.
     */
    ~CharacterController() { dispose(); }
    
    /**
     * @return true if the initialization was successful
     */
    bool init(const std::shared_ptr<InputController>& input, float scale);
        
    /**
     */
    void dispose();
    
    /**
     * It might seem weird to have this method given that everything
     * is processed with call back functions.  But we need some way
     * to synchronize the input with the animation frame.  Otherwise,
     * how can we know what was the touch location *last frame*?
     * Maybe there has been no callback function executed since the
     * last frame. This method guarantees that everything is properly
     * synchronized.
     */
    void update();
    
    void updatePositionFromInput(const Scene2& scene);
    
    void populate(const std::shared_ptr<cugl::scene2::SceneNode>& _worldnode,
                                       const std::shared_ptr<cugl::physics2::net::NetWorld>& _world, const     std::shared_ptr<cugl::AssetManager>& _assets);


    
};

#endif /* DPCharacterController_hpp */
