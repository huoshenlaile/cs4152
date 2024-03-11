#ifndef __RESTORE_SCENE_H__
#define __RESTORE_SCENE_H__

#include <cugl/cugl.h>
using namespace cugl::physics2::net;

class RestorationScene: public cugl::Scene2 {
    
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The NetEventController by this scene) */
    std::shared_ptr<NetEventController> _network;
    /** The network configuration */
    cugl::net::NetcodeConfig _config;
    
public:
    /**
     * Creates a new Restoration scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    RestorationScene() : cugl::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~RestorationScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Initializes the controller contents.
     *
     * In previous labs, this method "started" the scene.  But in this
     * case, we only use to initialize the scene user interface.  We
     * do not activate the user interface yet, as an active user
     * interface will still receive input EVEN WHEN IT IS HIDDEN.
     *
     * That is why we have the method {@link #setActive}.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    /**
     * Sets whether the scene is currently active
     *
     * This method should be used to toggle all the UI elements.  Buttons
     * should be activated when it is made active and deactivated when
     * it is not.
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
};

#endif
