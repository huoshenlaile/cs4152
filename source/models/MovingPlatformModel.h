//
//  MovingPlatformModel.h
//  DustyPaints
//
//  Created by Haoxuan Zou on 4/10/24.
//

#ifndef MovingPlatformModel_h
#define MovingPlatformModel_h

#include <cugl/cugl.h>
#include <stdio.h>

using namespace cugl;

#include <cugl/physics2/CUPolygonObstacle.h>

/** This class loads a moving platform object*/
class MovingPlatformModel :public physics2::PolygonObstacle {
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CU_DISALLOW_COPY_AND_ASSIGN(MovingPlatformModel);
    
protected:
    /** The texture filmstrip for the wall */
    std::string _wallTexture;
    
    struct DirectMove {
        float dx;
        float dy;
        float speed;
        float wait_time;
    };

    std::vector<DirectMove> _moves;
    
public:

#pragma mark -
#pragma mark Initializers
    /**
     * Creates a new wall at the origin.
     */
    MovingPlatformModel(void) : PolygonObstacle() { }

    /**
     * Destroys this wall, releasing all resources.
     */
    virtual ~MovingPlatformModel(void) {}

    /**
     * Initializes a wall from (not necessarily convex) polygon
     *
     * The anchor point (the rotational center) of the polygon is at the
     * center of the polygons bounding box.
     *
     * @param poly   The polygon vertices
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Poly2& poly) override {
        return init(poly,Vec2(0.5,0.5));
    }

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
    virtual bool init(const Poly2& poly, const Vec2 anchor) override {
        return init(poly, Vec2(0.5, 0.5), {}, {}, {}, {});
    };
    
    
    bool init(const Poly2& poly, const Vec2 anchor, std::vector<float> dxs, std::vector<float> dys, std::vector<float> speeds, std::vector<float> wait_times);

    /**
     * Creates a wall from (not necessarily convex) polygon
     *
     * The anchor point (the rotational center) of the polygon is at the
     * center of the polygons bounding box.
     *
     * @param poly   The polygon vertices
     *
     * @return  A newly allocated Wall, with the given shape
     */
    static std::shared_ptr<MovingPlatformModel> alloc(const Poly2& poly) {
        std::shared_ptr<MovingPlatformModel> result = std::make_shared<MovingPlatformModel>();
        return (result->init(poly) ? result : nullptr);
    }

    /**
     * Creates a wall from (not necessarily convex) polygon
     *
     * The anchor point (the rotational center) of the polygon is specified as a
     * ratio of the bounding box.  An anchor point of (0,0) is the bottom left of
     * the bounding box.  An anchor point of (1,1) is the top right of the bounding
     * box.  The anchor point does not need to be contained with the bounding box.
     *
     * @param  poly     The polygon vertices
     * @param  anchor   The rotational center of the polygon
     *
     * @return  A newly allocated Wall, with the given shape and anchor
     */
    static std::shared_ptr<MovingPlatformModel> alloc(const Poly2& poly, const Vec2& anchor) {
        std::shared_ptr<MovingPlatformModel> result = std::make_shared<MovingPlatformModel>();
        return (result->init(poly, anchor) ? result : nullptr);
    }

    /**
     * Returns the texture (key) for this wall
     *
     * The value returned is not a Texture2D value.  Instead, it is a key for
     * accessing the texture from the asset manager.
     *
     * @return the texture (key) for this wall
     */
    const std::string& getTextureKey() const { return _wallTexture; }

    /**
     * Returns the texture (key) for this wall
     *
     * The value returned is not a Texture2D value.  Instead, it is a key for
     * accessing the texture from the asset manager.
     *
     * @param  strip    the texture (key) for this wall
     */
    void setTextureKey(std::string strip) { _wallTexture = strip; }
};

#endif /* MovingPlatformModel_h */
