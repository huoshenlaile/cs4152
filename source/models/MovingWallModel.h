//
//  MovingWallModel.h
//  DustyPaints
//
//  Created by Esther Yu on 4/14/24.
//

#ifndef MovingWallModel_h
#define MovingWallModel_h
#include <cugl/cugl.h>
#include <stdio.h>
#include "WallModel.h"

using namespace cugl;

#include <cugl/physics2/CUPolygonObstacle.h>

class MovingWallModel : public WallModel {
private:
    std::vector<Vec2> _path;       // Path along which the wall moves
    float _speed;                  // Speed of the movement
    size_t _currentPathIndex;      // Current index in the path

public:
    MovingWallModel();
    virtual ~MovingWallModel();

    // Initializes the moving wall with a polygon, anchor, path, and speed
    bool init(const Poly2& poly, const Vec2& anchor, const std::vector<Vec2>& path, float speed);

    // Update method to move the wall along the path
    void update(float deltaTime);

    // Static allocator for the moving wall
    static std::shared_ptr<MovingWallModel> alloc(const Poly2& poly, const Vec2& anchor, const std::vector<Vec2>& path, float speed);
};


#endif /* MovingWallModel_h */
