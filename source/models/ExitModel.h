#ifndef __EXIT_MODEL_H__
#define __EXIT_MODEL_H__

#include <cugl/physics2/CUBoxObstacle.h>

using namespace cugl;


#pragma mark -
#pragma mark Exit Door Model

/**
* An exit door in the rocket lander game.
*
* Note that this class does keeps track of its own assets via keys, so that
* they can be attached later as part of the scene graph.
*/
class ExitModel : public physics2::BoxObstacle {
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CU_DISALLOW_COPY_AND_ASSIGN(ExitModel);

protected:
    /** The texture filmstrip for the exit door */
    std::string _exitTexture;
    
    /** The colors required to exit */
    std::set<std::string> _colorReqs;
    
    /** The colors that have been collected*/
    std::set<std::string> _colorsCollected;
    


#pragma mark -
#pragma mark Static Constructors
public:

    /**
     * Creates a new exit at the origin.
     *
     * The exit is 1 unit by 1 unit in size. The exit is scaled so that
     * 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return A newly allocated exit at the origin
     */
    static std::shared_ptr<ExitModel> alloc() {
        std::shared_ptr<ExitModel> result = std::make_shared<ExitModel>();
        return (result->init() ? result : nullptr);
    }

    /**
     * Creates a new exit with the given position
     *
     * The exit is 1 unit by 1 unit in size. The exit is scaled so that
     * 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos  Initial position in world coordinates
     *
     * @return  A newly allocated Exit, at the given position
     */
    static std::shared_ptr<ExitModel> alloc(const Vec2& pos) {
        std::shared_ptr<ExitModel> result = std::make_shared<ExitModel>();
        return (result->init(pos) ? result : nullptr);
    }

    /**
     * Creates a new exit with the given position and size.
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
     * @return  A newly allocated Exit, at the given position, with the given size
     */
    static std::shared_ptr<ExitModel> alloc(const Vec2& pos, const Size& size) {
        std::shared_ptr<ExitModel> result = std::make_shared<ExitModel>();
        return (result->init(pos, size) ? result : nullptr);
    }


#pragma mark -
#pragma mark Animation
    /**
     * Returns the texture (key) for this exit
     *
     * The value returned is not a Texture2D value.  Instead, it is a key for
     * accessing the texture from the asset manager.
     *
     * @return the texture (key) for this exit
     */
    const std::string& getTextureKey() const { return _exitTexture; }

    /**
     * Returns the texture (key) for this exit
     *
     * The value returned is not a Texture2D value.  Instead, it is a key for
     * accessing the texture from the asset manager.
     *
     * @param  strip    the texture (key) for this exit
     */
    void setTextureKey(std::string strip) { _exitTexture = strip; }

    /**
     * Returns the color requirements for this exit
     *
     * @return the set of required colors
     */
    const std::set<std::string> getColorReqs() const { return _colorReqs; }

    /**
     * Sets the color requirements for this exit
     *
     * @param colors   set of required colors
     */
    void setColorReqs(std::set<std::string> colors) { _colorReqs = colors; }
    
    /**
     * Returns the colors collected for this exit
     *
     * @return the set of collected colors
     */
    const std::set<std::string> getColorsCollected() const { return _colorsCollected; }
    
    /**
     * Adds a new collected color to the exit
     *
     * @param color  a color that was collected
     */
    void addColor(std::string color) { _colorsCollected.insert(color); }
    
    /**
     * Sets the colors collected for this exit
     *
     * @param colors   set of collected colors
     */
    void setColorsCollected(std::set<std::string> colors) { _colorsCollected = colors; }
    
    /**
     * Returns the remaining colors needed
     *
     * @param remaining_colors  set of remaining colors
     */
    const std::set<std::string> getRemainingColors() const {
        std::set<std::string> result;
        std::set_difference(_colorReqs.begin(),_colorReqs.end(), _colorsCollected.begin(), _colorsCollected.end(),
            std::inserter(result, result.end()));
        return result;
    }



#pragma mark -
#pragma mark Initializers
    /**
     * Creates a new exit at the origin.
     */
    ExitModel(void) : BoxObstacle() { }

    /**
     * Destroys this exit, releasing all resources.
     */
    virtual ~ExitModel(void) {}

    /**
     * Initializes a new exit at the origin.
     *
     * The exit is 1 unit by 1 unit in size. The exit is scaled so that
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
     * Initializes a new exit with the given position
     *
     * The exit is 1 unit by 1 unit in size. The exit is scaled so that
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
     * @param  size       The dimensions of the exit door.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2 pos, const Size size) override;

};

#endif 
