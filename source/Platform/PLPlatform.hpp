//
//  PLPlatform.hpp
//  Prototype1
//
//  Created by Jinseok Oh on 3/2/24.
//

#ifndef PLPlatform_hpp
#define PLPlatform_hpp

#include <stdio.h>
#include <cugl/assets/CUAsset.h>
#include <cugl/cugl.h>
#include <cugl/io/CUJsonReader.h>
#include "./PLWallModel.hpp"

using namespace cugl;
//class WallModel;
class PlatformModel : public Asset{
protected:
    /** Reference to all the walls */
    std::vector<std::shared_ptr<WallModel>> _walls;
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _worldnode;
    /** The root node of this level */
    std::shared_ptr<scene2::SceneNode> _root;
    /** The AssetManager for the game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The physics word; part of the model (though listeners elsewhere) */
    std::shared_ptr<cugl::physics2::net::NetWorld> _world;
    /** The level drawing scale (difference between physics and drawing coordinates) */
    Vec2 _scale;
    /** The bounds of this level in physics coordinates */
    Rect _bounds;
public:
    /**
    * Obtains a vector array of floats representing the vertices of a polygon
    * given a tiled polygon object
    *
    *
    * @return a vector array of floats representing the vertices of the polygon
    */
    std::vector<float> getVertices(const std::shared_ptr<JsonValue>& json);
    /**
     * Loads a single wall object
     *
     * The wall will be retained and stored in the vector _walls.  If the
     * wall fails to load, then it will not be added to _walls.
     *
     * @param  reader   a JSON reader with cursor ready to read the wall
     *
     * @retain the wall
     * @return true if the wall was successfully loaded
     */
    bool loadWall(const std::shared_ptr<JsonValue>& json);
    /**
     * Sets the loaded assets for this game level
     *
     * @param assets the loaded assets for this game level
     */
    void setAssets(const std::shared_ptr<AssetManager>& assets) { _assets = assets;  }
    /**
     * Sets the scene graph node for drawing purposes.
     *
     * The scene graph is completely decoupled from the physics system.  The node
     * does not have to be the same size as the physics body. We only guarantee
     * that the node is positioned correctly according to the drawing scale.
     *
     * @param value  the scene graph node for drawing purposes.
     *
     * @retain  a reference to this scene graph node
     * @release the previous scene graph node used by this object
     */
    void setRootNode(const std::shared_ptr<scene2::SceneNode>& root);
    
    /**
     * Clears the root scene graph node for this level
     */
    void clearRootNode();
    
    /**
     * Toggles whether to show the debug layer of this game world.
     *
     * The debug layer displays wireframe outlines of the physics fixtures.
     *
     * @param  flag whether to show the debug layer of this game world
     */
    void showDebug(bool flag);
    
    /**
     * Returns the Obstacle world in this game level
     *
     * @return the obstacle world in this game level
     */
    const std::shared_ptr<cugl::physics2::net::NetWorld>& getWorld() { return _world; }

//#pragma mark -
//#pragma mark Initializers
    /**
     * Creates a new, empty level.
     */
    PlatformModel(void);

    /**
     * Converts the string to a color
     *
     * Right now we only support the following colors: yellow, red, blur, green,
     * black, and grey.
     *
     * @param  name the color name
     *
     * @return the color for the string
     */
    Color4 parseColor(std::string name);
    
    /**
     * Destroys this level, releasing all resources.
     */
    virtual ~PlatformModel(void);

    /**
     * Adds the physics object to the physics world and loosely couples it to the scene graph
     *
     * There are two ways to link a physics object to a scene graph node on the
     * screen.  One way is to make a subclass of a physics object, like we did
     * with rocket.  The other is to use callback functions to loosely couple
     * the two.  This function is an example of the latter.
     *
     * param obj    The physics object to add
     * param node   The scene graph node to attach it to
     */
    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                     const std::shared_ptr<cugl::scene2::SceneNode>& node);

};
#endif /* PLPlatform_hpp */
