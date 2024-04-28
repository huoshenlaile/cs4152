//
//  DrippingPaint.hpp
//  DustyPaints
//
//  Created by Emily on 4/20/24.
//

#ifndef DrippingPaint_hpp
#define DrippingPaint_hpp


#include <cugl/cugl.h>
#include "interactable.h"

class DrippingPaint : public Interactable {
protected:
    std::string sub_message_head;
    std::string pub_message_head;
    std::string color;
    bool is_out = false;
    float _dripSpeed = 1;
    
    std::shared_ptr<scene2::SpriteNode> _animation;
    std::string _textureName;
    std::shared_ptr<cugl::scene2::ActionManager> _actions;
    std::shared_ptr<cugl::scene2::Animate> _animate;
        
public:
    DrippingPaint(void) : Interactable() {}
    ~DrippingPaint() {
    }

    virtual bool init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) override;
    
    virtual PublishedMessage timeUpdate(float) override;
    
    virtual bool linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode> &sceneNode, float scale) override;
    
    virtual bool bindAssets(const std::shared_ptr<cugl::AssetManager>& assets, Vec2 scale2d) override;
    
 

    virtual PublishedMessage onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false, std::shared_ptr<CharacterController> character = nullptr) override;
    
    static std::shared_ptr<DrippingPaint> alloc(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds) {
        std::shared_ptr<DrippingPaint> result = std::make_shared<DrippingPaint>();
        return (result->init(json, scale, bounds) ? result : nullptr);
    }

};


#endif /* DrippingPaint_hpp */
