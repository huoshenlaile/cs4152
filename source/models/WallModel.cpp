#include "WallModel.h"

/**
 * Initializes a wall from (not necessarily convex) polygon
 *
 * The anchor point (the rotational center) of the polygon is specified as a
 * ratio of the bounding box.  An anchor point of (0,0) is the bottom left of
 * the bounding box.  An anchor point of (1,1) is the top right of the bounding
 * box.  The anchor point does not need to be contained with the bounding box.
 *
 * @param  poly     The polygon vertices
 * @param  anchor   The rotational center of the polygon
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool WallModel::init(const Poly2& poly, const Vec2 anchor) {
    PolygonObstacle::initWithAnchor(poly,anchor);
    _wallTexture = "";
    return true;
}
