//
//  MovingWall.h
//  DustyPaints
//
//  Created by Esther Yu on 4/14/24.
//

#ifndef MovingWall_h
#define MovingWall_h

#include "interactable.h"
#include <cugl/cugl.h>

using namespace cugl;

class MovingWall : public Interactable {
protected:
    std::vector<Vec2> _path;        // Path for movement
    float _speed;                   // Speed of movement along the path
    size_t _currentPathIndex = 0;   // Current index in the path
    bool _isMoving = true;          // Is the wall currently moving?
    float _movementTimer = 0.0;     // Timer to manage movement updates
    float _remainingDuration = 0;  

public:
    MovingWall() : Interactable(), _speed(0) {}
    virtual ~MovingWall() {}

    virtual bool init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) override;
    void update(float dt);

    static std::shared_ptr<MovingWall> alloc(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) {
        std::shared_ptr<MovingWall> result = std::make_shared<MovingWall>();
        return (result->init(json, scale, bounds) ? result : nullptr);
    }
    
    virtual bool linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode> &sceneNode, float scale) override;

    void setPath(const std::vector<Vec2>& path);
    void setSpeed(float speed);
    void startMoving();
    void stopMoving();
};

#endif /* MovingWall_h */
