//
//  MovingWall.cpp
//  DustyPaints
//
//  Created by Esther Yu on 4/14/24.
//

#include <stdio.h>
#include "MovingWall.h"

bool MovingWall::init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) {
    Interactable::init(json, scale, bounds);
    
    std::shared_ptr<JsonValue> properties = json -> get("properties");
    // find the Publication property
    for (int i = 0; i < properties->size(); i++){
        if(properties->get(i)->getString("name") == "Movements"){
            auto movement = properties->get(i)->get("value");
            _speed = movement->getFloat("speed", 0);
            for (auto& point : movement->get("path")->children()) {
                Vec2 p = _selfObstacle->getPosition();
                CULog("HIIIIIILOOOOKKKKKKHEREEEEEEEEE: %f hi %f", p.x, p.y);
                _path.emplace_back(p.x+point->getFloat("x"), p.y+point->getFloat("y"));
            }
            CULog("HIIIIIILOOOOKKKKKKHEREEEEEEEEE: %f", 0.1f);
        }
    }
    
    _isMoving = !_path.empty();
    activated = true;
    return true;
}

void MovingWall::update(float dt) {
    if (!_isMoving || _path.empty()) return;
    
    // Calculate the next position along the path
    Vec2 currentTarget = _path[_currentPathIndex];
    Vec2 currentPosition = _selfObstacle->getPosition();
    Vec2 direction = (currentTarget - currentPosition);
    float distance = direction.length();
    direction.normalize();

    float step = _speed * dt;
    if (step >= distance) {
        currentPosition = currentTarget;
        _currentPathIndex = (_currentPathIndex + 1) % _path.size();  // Loop the path
    } else {
        currentPosition += direction * step;
    }

    _selfObstacle->setPosition(currentPosition);
}



bool MovingWall::linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode>& sceneNode, float scale) {
    // Call the base class implementation first
    Interactable::linkToWorld(physicsWorld, sceneNode, scale);

    // If the wall is supposed to move, set up a listener to update the scene node position accordingly
    if (!_path.empty()) {
        scene2::SceneNode* weakTexture = _selfTexture.get(); // Safe as long as `this` is alive
        _selfObstacle->setListener([=](physics2::Obstacle* obs){
            Vec2 currentPosition = obs->getPosition();
            Vec2 currentTarget = _path[_currentPathIndex];
            
            Vec2 direction = currentTarget - currentPosition;
            float distanceToTarget = direction.length();

            if (distanceToTarget > 0) {
                direction.normalize();
                float step = _speed / 60.0f; // Assuming 60 updates per second
                if (step > distanceToTarget) {
                    currentPosition = currentTarget;
                    _currentPathIndex = (_currentPathIndex + 1) % _path.size();  // Cycle through the path
                } else {
                    currentPosition += direction * step;
                }
                obs->setPosition(currentPosition);
                weakTexture->setPosition(currentPosition * scale);
                weakTexture->setAngle(obs->getAngle());
            }
        });
    }
    
    return true;
}
