//
//  SensorModel.cpp
//  DustyPaints
//
//  Created by Kidus Zegeye on 3/23/24.
//

#include "SensorModel.h"

using namespace cugl;

#pragma mark -
#pragma mark Initializers
/**
 * Initializes a new sensor with the given position and size.
 *
 * The exit size is specified in world coordinates.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  poly    The polygon of the sensor
 * @param  anchor   The rotational center of the polygon
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool SensorModel::init(const Poly2& poly, const Vec2& anchor) {
    PolygonObstacle::initWithAnchor(poly, anchor);
    setSensor(true);
    return true;
}

