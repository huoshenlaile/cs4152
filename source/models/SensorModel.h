#ifndef __SENSOR_MODEL_H__
#define __SENSOR_MODEL_H__

#include <cugl/physics2/CUBoxObstacle.h>

using namespace cugl;


#pragma mark -
#pragma mark Sensor Model

/**
* A sensor for detecting the player.
*
* Note that this class does keeps track of its own assets via keys, so that
* they can be attached later as part of the scene graph.
*/
class SensorModel : public physics2::BoxObstacle {
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CU_DISALLOW_COPY_AND_ASSIGN(SensorModel);

protected:
    /** The texture filmstrip for the sensor door */
    std::string _sensorTexture;
    
#pragma mark -
#pragma mark Static Constructors
public:
    /**
     * Creates a new sensor at the origin.
     *
     * The sensor is 1 unit by 1 unit in size. The sensor is scaled so that
     * 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return A newly allocated sensor at the origin
     */
    static std::shared_ptr<SensorModel> alloc() {
        std::shared_ptr<SensorModel> result = std::make_shared<SensorModel>();
        return (result->init() ? result : nullptr);
    }

    /**
     * Creates a new sensor with the given position
     *
     * The sensor is 1 unit by 1 unit in size. The sensor is scaled so that
     * 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos  Initial position in world coordinates
     *
     * @return  A newly allocated Sensor, at the given position
     */
    static std::shared_ptr<SensorModel> alloc(const Vec2& pos) {
        std::shared_ptr<SensorModel> result = std::make_shared<SensorModel>();
        return (result->init(pos) ? result : nullptr);
    }

    /**
     * Creates a new sensor with the given position and size.
     *
     * The sensor size is specified in world coordinates.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos      Initial position in world coordinates
     * @param  size       The dimensions of the sensor door.
     *
     * @return  A newly allocated Sensor, at the given position, with the given size
     */
    static std::shared_ptr<SensorModel> alloc(const Vec2& pos, const Size& size) {
        std::shared_ptr<SensorModel> result = std::make_shared<SensorModel>();
        return (result->init(pos, size) ? result : nullptr);
    }

#pragma mark -
#pragma mark Animation
    /**
     * Returns the texture (key) for this sensor
     *
     * The value returned is not a Texture2D value.  Instead, it is a key for
     * accessing the texture from the asset manager.
     *
     * @return the texture (key) for this sensor
     */
    const std::string& getTextureKey() const { return _sensorTexture; }

    /**
     * Returns the texture (key) for this sensor
     *
     * The value returned is not a Texture2D value.  Instead, it is a key for
     * accessing the texture from the asset manager.
     *
     * @param  strip    the texture (key) for this sensor
     */
    void setTextureKey(std::string strip) { _sensorTexture = strip; }
    
#pragma mark -
#pragma mark Initializers
    /**
     * Creates a new sensor at the origin.
     */
    SensorModel(void) : BoxObstacle() { }

    /**
     * Destroys this sensor, releasing all resources.
     */
    virtual ~SensorModel(void) {}

    /**
     * Initializes a new sensor at the origin.
     *
     * The sensor is 1 unit by 1 unit in size. The sensor is scaled so that
     * 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override { return init(Vec2::ZERO,Size::ZERO); }

    /**
     * Initializes a new sensor with the given position
     *
     * The sensor is 1 unit by 1 unit in size. The sensor is scaled so that
     * 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos  Initial position in world coordinates
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2 pos) override { return init(pos,Size(1,1)); }

    /**
     * Initializes a new sensor with the given position and size.
     *
     * The sensor size is specified in world coordinates.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos      Initial position in world coordinates
     * @param  size       The dimensions of the sensor door.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2 pos, const Size size) override;

};

#endif
