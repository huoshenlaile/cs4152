#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include <cugl/cugl.h>
#include <vector>

class MenuScene : public cugl::Scene2 {

protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The menu button for hosting a game */
    std::shared_ptr<cugl::scene2::Button> _startbutton;
//    /** The menu button for joining a game */
//    std::shared_ptr<cugl::scene2::Button> _joinbutton;
    /** The menu button for setting */
    std::shared_ptr<cugl::scene2::Button> _settingbutton;
//    /** The menu button for quiting a game */
//    std::shared_ptr<cugl::scene2::Button> _quitbutton;

    
public:
    /**
     * The menu SceneStatus.
     *
     * This state allows the top level application to know what the user
     * chose.
     */
    enum SceneStatus {
        /** User has not yet made a SceneStatus */
        NONE,
        /** User wants to host a game */
        START,
        /** User wants to join a game */
//        JOIN,
        SETTING,
        QUIT
    };
    /** The player menu SceneStatus */
    SceneStatus status;
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new  menu scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    MenuScene() : cugl::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~MenuScene() { dispose(); }
    
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
