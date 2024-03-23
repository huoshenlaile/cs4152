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
 * @param  pos      Initial position in world coordinates
 * @param  size       The dimensions of the exit door.
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool SensorModel::init(const Vec2 pos, const Size size) {
    BoxObstacle::init(pos,size);
    std::string name("sensor");
    setName(name);
    setSensor(true);
    return true;
}

