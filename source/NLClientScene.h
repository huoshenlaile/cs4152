//
//  NLClientScene.h
//  Network Lab
//
//  This class represents the scene for the client when joining a game. Normally
//  this class would be combined with the class for the client scene (as both
//  initialize the network controller).  But we have separated to make the code
//  a little clearer for this lab.
//
//  Author: Walker White, Aidan Hobler
//  Version: 2/8/22
//
#ifndef __NL_CLIENT_SCENE_H__
#define __NL_CLIENT_SCENE_H__
#include <cugl/cugl.h>
#include <vector>

/**
 * This class provides the interface to join an existing game.
 *
 * Most games have a since "matching" scene whose purpose is to initialize the
 * network controller.  We have separate the host from the client to make the
 * code a little more clear.
 */
class ClientScene : public cugl::Scene2 {
public:
    /**
     * The configuration status
     *
     * This is how the application knows to switch to the next scene.
     */
    enum Status {
        /** Client has not yet entered a room */
        IDLE,
        /** Client is connecting to the host */
        JOIN,
        /** Client is waiting on host to start game */
        WAIT,
        /** Time to start the game */
        START,
        /** Game was aborted; back to main menu */
        ABORT
    };
    
protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** The network connection (as made by this scene) */
    std::shared_ptr<cugl::net::NetcodeConnection> _network;

    /** The menu button for starting a game */
    std::shared_ptr<cugl::scene2::Button> _startgame;
    /** The back button for the menu scene */
    std::shared_ptr<cugl::scene2::Button> _backout;
    /** The game id label (for updating) */
    std::shared_ptr<cugl::scene2::TextField> _gameid;
    /** The players label (for updating) */
    std::shared_ptr<cugl::scene2::Label> _player;
    
    /** The network configuration */
    cugl::net::NetcodeConfig _config;
    
    /** The current status */
    Status _status;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new client scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    ClientScene() : cugl::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~ClientScene() { dispose(); }
    
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
     * Returns the scene status.
     *
     * Any value other than WAIT will transition to a new scene.
     *
     * @return the scene status
     *
     */
    Status getStatus() const { return _status; }
    
    /**
     * The method called to update the scene.
     *
     * We need to update this method to constantly talk to the server
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;

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
     * Updates the text in the given button.
     *
     * Techincally a button does not contain text. A button is simply a scene graph
     * node with one child for the up state and another for the down state. So to
     * change the text in one of our buttons, we have to descend the scene graph.
     * This method simplifies this process for you.
     *
     * @param button    The button to modify
     * @param text      The new text value
     */
    void updateText(const std::shared_ptr<cugl::scene2::Button>& button, const std::string text);

    /**
     * Reconfigures the start button for this scene
     *
     * This is necessary because what the buttons do depends on the state of the
     * networking.
     */
    void configureStartButton();
    
    /**
     * Connects to the game server as specified in the assets file
     *
     * The {@link #init} method set the configuration data. This method simply uses
     * this to create a new {@Link NetworkConnection}. It also immediately calls
     * {@link #checkConnection} to determine the scene state.
     *
     * @param room  The room ID to use
     *
     * @return true if the connection was successful
     */
    bool connect(const std::string room);

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

};

#endif /* __NL_GAME_SCENE_H__ */
