//
//  PLWallModel.hpp
//  Prototype1
//
//  Created by Jinseok Oh on 3/2/24.
//

#ifndef PLWallModel_hpp
#define PLWallModel_hpp
#include <cugl/cugl.h>
#include <stdio.h>

using namespace cugl;

#include <cugl/physics2/CUPolygonObstacle.h>


class WallModel :public physics2::PolygonObstacle{
protected:
    /** The texture filmstrip for the wall */
    std::string _wallTexture;
    
public:
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
    static std::shared_ptr<WallModel> alloc(const Poly2& poly) {
        std::shared_ptr<WallModel> result = std::make_shared<WallModel>();
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
    static std::shared_ptr<WallModel> alloc(const Poly2& poly, const Vec2& anchor) {
        std::shared_ptr<WallModel> result = std::make_shared<WallModel>();
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
#endif /* PLWallModel_hpp */
