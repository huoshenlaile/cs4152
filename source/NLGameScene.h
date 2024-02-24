//
//  NLGameScene.h
//  Network Lab
//
//  This class provides a simple game.  We just keep track of the connection
//  and trade color values back-and-forth across the network.
//
//  Author: Walker White, Aidan Hobler
//  Version: 2/8/22
//
#ifndef __NL_GAME_SCENE_H__
#define __NL_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <vector>


/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller. Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The network connection (as made by this scene) */
    std::shared_ptr<cugl::net::NetcodeConnection> _network;

    /** The back button for the menu scene */
    std::shared_ptr<cugl::scene2::Button> _backout;
    /** The players label (for updating) */
    std::shared_ptr<cugl::scene2::Label> _player;
    /** The white button */
    std::shared_ptr<cugl::scene2::Button> _white;
    /** The red button */
    std::shared_ptr<cugl::scene2::Button> _red;
    /** The green button */
    std::shared_ptr<cugl::scene2::Button> _green;
    /** The blue button */
    std::shared_ptr<cugl::scene2::Button> _blue;
    /** The yellow button */
    std::shared_ptr<cugl::scene2::Button> _yellow;
    /** The cyan button */
    std::shared_ptr<cugl::scene2::Button> _cyan;
    /** The magenta button */
    std::shared_ptr<cugl::scene2::Button> _magenta;
    /** The black button */
    std::shared_ptr<cugl::scene2::Button> _black;
    /** The grey button */
    std::shared_ptr<cugl::scene2::Button> _grey;

    /** Whether this player is the host */
    bool _ishost;

    /** Whether we quit the game */
    bool _quit;
    
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    GameScene() : cugl::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Initializes the controller contents, and starts the game
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
     
    /**
     * The method called to update the scene.
     *
     * We need to update this method to constantly talk to the server
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;
    
    /**
     * Returns the network connection (as made by this scene)
     *
     * This value will be reset every time the scene is made active.
     *
     * @return the network connection (as made by this scene)
     */
    std::shared_ptr<cugl::net::NetcodeConnection> getConnection() const {
        return _network;
    }
    
    /**
     * Returns the network connection (as made by this scene)
     *
     * This value will be reset every time the scene is made active.
     *
     * @return the network connection (as made by this scene)
     */
    void setConnection(const std::shared_ptr<cugl::net::NetcodeConnection>& network) {
        _network = network;
    }
    
    /**
     * Returns true if the player is host.
     *
     * We may need to have gameplay specific code for host.
     *
     * @return true if the player is host.
     */
    bool isHost() const { return _ishost; }

    /**
     * Sets whether the player is host.
     *
     * We may need to have gameplay specific code for host.
     *
     * @param host  Whether the player is host.
     */
    void setHost(bool host)  { _ishost = host; }

    /**
     * Returns true if the player quits the game.
     *
     * @return true if the player quits the game.
     */
    bool didQuit() const { return _quit; }
 
    /**
     * Disconnects this scene from the network controller.
     *
     * Technically, this method does not actually disconnect the network controller.
     * Since the network controller is a smart pointer, it is only fully disconnected
     * when ALL scenes have been disconnected.
     */
    void disconnect() { _network = nullptr; }

private:
    /**
     * Processes data sent over the network.
     *
     * Once connection is established, all data sent over the network consistes of
     * byte vectors. This function is a call back function to process that data.
     * Note that this function may be called *multiple times* per animation frame,
     * as the messages can come from several sources.
     *
     * Typically this is where players would communicate their names after being
     * connected. In this lab, we only need it to do one thing: communicate that
     * the host has started the game.
     *
     * @param source    The UUID of the sender
     * @param data      The data received
     */
    void processData(const std::string source, const std::vector<std::byte>& data);

    /**
     * Checks that the network connection is still active.
     *
     * Even if you are not sending messages all that often, you need to be calling
     * this method regularly. This method is used to determine the current state
     * of the scene.
     *
     * @return true if the network connection is still active.
     */
    bool checkConnection();

    /**
     * Transmits a color change to all other devices.
     *
     * Because a device does not receive messages from itself, this method should
     * also set the color.
     *
     * @param color The new color
     */
    void transmitColor(cugl::Color4 color);

};

#endif /* __NL_GAME_SCENE_H__ */
