#include "ExitModel.h"

using namespace cugl;

#pragma mark -
#pragma mark Initializers
/**
 * Initializes a new exit with the given position and size.
 *
 * The exit size is specified in world coordinates.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 * @param  size   	The dimensions of the exit door.
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool ExitModel::init(const Vec2 pos, const Size size) {
	BoxObstacle::init(pos,size);
	std::string name("goal");
	setName(name);
	setSensor(true);
	_exitTexture = "";
    _colorReqs = std::set<std::string>();
    _colorsCollected = std::set<std::string>();
	return true;
}

