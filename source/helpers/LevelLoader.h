#ifndef __LEVEL_SCENE_H__
#define __LEVEL_SCENE_H__

#include <stdio.h>
#include <cugl/assets/CUAsset.h>
#include <cugl/cugl.h>
#include <cugl/io/CUJsonReader.h>
#include "../models/WallModel.h"
#include "../models/ExitModel.h"
#include "../models/SensorModel.h"


using namespace cugl;

/** This class handles loading the entire level. The scene and the physical world
 * objects are all created here. This class parses the json file and creates 
 * correspoding objects. Camera is set in this class as well for the scene. Must 
 * asynchronously load the json file along with attaching hooks to parse
 * the json file an App.cpp.
*/
class LevelLoader : public Asset{
protected:
    /** The root node of this level */
    std::shared_ptr<scene2::SceneNode> _root;
    /** The bounds of this level in physics coordinates */
    Rect _bounds;
    /** The global gravity for this level */
    Vec2 _gravity;
    /** The level drawing scale (difference between physics and drawing coordinates) */
    Vec2 _scale;
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _worldnode;
    /** Reference to the debug root of the scene graph */
    std::shared_ptr<scene2::SceneNode> _debugnode;
    /** The physics world; part of the model (though listeners elsewhere) */
    std::shared_ptr<cugl::physics2::net::NetWorld> _world;
    /** Reference to all the walls */
    std::vector<std::shared_ptr<WallModel>> _walls;
    /** Reference to the end destination of the game*/
    std::shared_ptr<ExitModel> _goalDoor;
    /** Reference to the sensors of the game*/
    std::vector<std::shared_ptr<SensorModel>> _sensors;
    /** The AssetManager for the game mode */
    std::shared_ptr<cugl::AssetManager> _assets;
public:
#pragma mark Physics Attributes
    /**
     * Returns the bounds of this level in physics coordinates
     *
     * @return the bounds of this level in physics coordinates
     */
    const Rect& getBounds() const   { return _bounds; }

    /**
     * Returns the global gravity for this level
     *
     * @return the global gravity for this level
     */
    const Vec2& getGravity() const { return _gravity; }

    /**
    * Obtains a physics position for the game world from a tiled json object.
    *
    * This conversion is necessary since y=0 in tiled is at the top of the screen,
    * whilst y=0 in our game is at the bottom.
    *
    * @param the json file for the tiled object that the position needs to be obtained from
    *
    * @return the resultant tiled position
    *
    */
    Vec2 getObjectPos(const std::shared_ptr<JsonValue>& json);

    /**
     * Returns the exit door in this game level
     *
     * @return the exit door in this game level
     */
    const std::shared_ptr<ExitModel>& getExit() { return _goalDoor; }
    
    /**
     * Returns the sensors in this game level
     *
     * @return the sensors in this game level
     */
    const std::vector<std::shared_ptr<SensorModel>>& getSensors() { return _sensors; }
    /**
    * Determines which object is currently being parsed,
    * and then sends the object over
    * to the respective load function
    *
    * @param reader a JSON reader with cursor ready to read the object
    *
    * @return true if the object was successfully loaded
    */
    bool loadObject(const std::shared_ptr<JsonValue>& json);

    /**
     * Loads the singular exit door
     *
     * The exit door will will be stored in _goalDoor field and retained.
     * If the exit fails to load, then _goalDoor will be nullptr.
     *
     * @param  reader   a JSON reader with cursor ready to read the exit
     *
     * @retain the exit door
     * @return true if the exit door was successfully loaded
     */
    bool loadGoalDoor(const std::shared_ptr<JsonValue>& json);
    
    /**
     * Loads a sensor obstacle
     *
     * The sensor door will will be stored in _sensors field and retained.
     * If the sensor fails to load, then _sensors will be nullptr.
     *
     * @param  reader   a JSON reader with cursor ready to read the exit
     *
     * @retain the sensor
     * @return true if the sensor was successfully loaded
     */
    bool loadSensor(const std::shared_ptr<JsonValue>& json);
    
    /**
     * Creates a new game level with no source file.
     *
     * The source file can be set at any time via the setFile() method. This method
     * does NOT load the asset.  You must call the load() method to do that.
     *
     * @return  an autoreleased level file
     */
    static std::shared_ptr<LevelLoader> alloc() {
        std::shared_ptr<LevelLoader> result = std::make_shared<LevelLoader>();
        return (result->init("") ? result : nullptr);
    }

    /**
     * Creates a new game level with the given source file.
     *
     * This method does NOT load the level. You must call the load() method to do that.
     * This method returns false if file does not exist.
     *
     * @return  an autoreleased level file
     */
    static std::shared_ptr<LevelLoader> alloc(std::string file) {
        std::shared_ptr<LevelLoader> result = std::make_shared<LevelLoader>();
        return (result->init(file) ? result : nullptr);
    }

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
    void setAssets(const std::shared_ptr<AssetManager>& assets){_assets = assets;};

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
     * Returns the physics world in this game level
     *
     * @return the physics world in this game level
     */
    const std::shared_ptr<cugl::physics2::net::NetWorld>& getWorld() { return _world; }
    
    
    void setWorld(std::shared_ptr<cugl::physics2::net::NetWorld> world) {
        this->_world = world;
    }
    
    /** Sets the background image of the level*/
    void setBackgroundScene();

    /**
     * Loads this game level from the source file
     *
     * This load method should NEVER access the AssetManager.  Assets are loaded in
     * parallel, not in sequence.  If an asset (like a game level) has references to
     * other assets, then these should be connected later, during scene initialization.
     *
     * @param file the name of the source file to load from
     *
     * @return true if successfully loaded the asset from a file
     */
    virtual bool preload(const std::string& file);

    /**
     * Loads this game level from a JsonValue containing all data from a source Json file.
     *
     * This load method should NEVER access the AssetManager.  Assets are loaded in
     * parallel, not in sequence.  If an asset (like a game level) has references to
     * other assets, then these should be connected later, during scene initialization.
     *
     * @param json the json loaded from the source file to use when loading this game level
     *
     * @return true if successfully loaded the asset from the input JsonValue
     */
    virtual bool preload(const std::shared_ptr<cugl::JsonValue>& json) override;

    /**
     * Creates a new, empty level.
     */
    LevelLoader(void);

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
    virtual ~LevelLoader(void);

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
    
    
    std::shared_ptr<cugl::physics2::net::NetWorld> getPhysicsWorld(){
        return _world;
    }
};
#endif
