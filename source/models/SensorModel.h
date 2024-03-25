#ifndef __SENSOR_MODEL_H__
#define __SENSOR_MODEL_H__

#include <cugl/physics2/CUPolygonObstacle.h>

using namespace cugl;


#pragma mark -
#pragma mark Sensor Model

/**
* A sensor for detecting the player.
*
* Note that this class does keeps track of its own assets via keys, so that
* they can be attached later as part of the scene graph.
*/
class SensorModel : public physics2::PolygonObstacle {
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
     * Creates a new sensor with the given polygon.
     *
     * The sensor size is specified in world coordinates.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  poly    The polygon of the sensor
     *
     * @return  A newly allocated Sensor allocated via the given polygon
     */
    static std::shared_ptr<SensorModel> alloc(const Poly2& poly) {
        std::shared_ptr<SensorModel> result = std::make_shared<SensorModel>();
        return (result->init(poly) ? result : nullptr);
    }

    /**
     * Creates a new sensor with the given polygon and anchor.
     *
     * The sensor size is specified in world coordinates.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  poly    The polygon of the sensor
     * @param  anchor   The rotational center of the polygon
     *
     * @return  A newly allocated Sensor allocated via the given polygon and anchor
     */
    static std::shared_ptr<SensorModel> alloc(const Poly2& poly, const Vec2& anchor) {
        std::shared_ptr<SensorModel> result = std::make_shared<SensorModel>();
        return (result->init(poly, anchor) ? result : nullptr);
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
    SensorModel(void) : PolygonObstacle() { }

    /**
     * Destroys this sensor, releasing all resources.
     */
    virtual ~SensorModel(void) {}

    /**
     * Initializes a new sensor with the given polygon
     *
     * The sensor size is specified in world coordinates.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  poly    The polygon of the sensor
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Poly2& poly) override { return init(poly,Vec2(0.5,0.5)); }

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
     * @param  poly    The polygon of the sensor
     * @param  anchor   The rotational center of the polygon
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Poly2& poly, const Vec2& anchor);

};

#endif
