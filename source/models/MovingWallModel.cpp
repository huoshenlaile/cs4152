//
//  MovingWallModel.cpp
//  DustyPaints
//
//  Created by Esther Yu on 4/14/24.
//

#include <stdio.h>

#include "MovingWallModel.h"

MovingWallModel::MovingWallModel() : WallModel(), _speed(0), _currentPathIndex(0) {}

MovingWallModel::~MovingWallModel() {}

bool MovingWallModel::init(const Poly2& poly, const Vec2& anchor, const std::vector<Vec2>& path, float speed) {
    if (!WallModel::init(poly, anchor)) {
        return false;
    }
    _path = path;
    _speed = speed;
    return true;
}

void MovingWallModel::update(float deltaTime) {
    if (_path.size() > 1 && _speed != 0) {
        Vec2 currentPos = getPosition();
        Vec2 targetPos = _path[_currentPathIndex];

        Vec2 direction = targetPos - currentPos;
        float distanceToTravel = _speed * deltaTime;
        float distanceToTarget = direction.length();

        if (distanceToTravel >= distanceToTarget) {
            setPosition(targetPos);
            _currentPathIndex = (_currentPathIndex + 1) % _path.size(); // Loop to the first index after the last
        } else {
            direction.normalize();
            setPosition(currentPos + direction * distanceToTravel);
        }
    }
}

std::shared_ptr<MovingWallModel> MovingWallModel::alloc(const Poly2& poly, const Vec2& anchor, const std::vector<Vec2>& path, float speed) {
    std::shared_ptr<MovingWallModel> result = std::make_shared<MovingWallModel>();
    if (result->init(poly, anchor, path, speed)) {
        return result;
    }
    return nullptr;
}

