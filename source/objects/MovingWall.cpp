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
            std::string path = movement->getString("xy");
            std::vector<int> coordinates;
            std::stringstream ss(path);
            std::string item;
            while (getline(ss, item, ',')) {
                coordinates.push_back(std::stoi(item));
            }
            Vec2 p = _selfObstacle->getPosition();
            for (size_t i = 0; i < coordinates.size(); i += 2) {
                int xi = coordinates[i];
                int yi = coordinates[i + 1];
                _path.emplace_back(p.x + xi, p.y + yi);
            }
            
            //            for (auto& point : movement->get("path")->children()) {
            //                Vec2 p = _selfObstacle->getPosition();
            //                CULog("HIIIIIILOOOOKKKKKKHEREEEEEEEEE: %f hi %f", p.x, p.y);
            //                _path.emplace_back(p.x+point->getFloat("x"), p.y+point->getFloat("y"));
            //            }
            
            for (auto c : coordinates){
                std::cout<<c << std::endl;
            }
            
            CULog("HIIIIIILOOOOKKKKKKHEREEEEEEEEE: %f", 0.1f);
            _moveOnContact = movement->getBool("MoveOnContact");
        }
    }
    
    _isMoving = !_path.empty() && !_moveOnContact;
    activated = true;
    OnBeginContactEnabled = true;
    timeUpdateEnabled = true;
    return true;
}

PublishedMessage MovingWall::onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact, std::shared_ptr<Interactable> otherInteractable, bool isCharacter, std::shared_ptr<CharacterController> character) {
    if(_moveOnContact && isCharacter){
        _isMoving = true;
    }
    return PublishedMessage();
}



bool MovingWall::linkToWorld(const std::shared_ptr<cugl::physics2::ObstacleWorld> &physicsWorld, const std::shared_ptr<cugl::scene2::SceneNode>& sceneNode, float scale) {
    // Call the base class implementation first
    Interactable::linkToWorld(physicsWorld, sceneNode, scale);

    // If the wall is supposed to move, set up a listener to update the scene node position accordingly
    if (!_path.empty()) {
        scene2::SceneNode* weakTexture = _selfTexture.get(); // Safe as long as `this` is alive
        _selfObstacle->setListener([=](physics2::Obstacle* obs){
            if (!_isMoving || _path.empty()) return;
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
            } else {
                _currentPathIndex = (_currentPathIndex + 1) % _path.size();  // Cycle through the path
            }
        });
    }
    
    return true;
}
