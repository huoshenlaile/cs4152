//
//  NLHostScene.cp
//  Network Lab
//
//  This class represents the scene for the host when creating a game. Normally
//  this class would be combined with the class for the client scene (as both
//  initialize the network controller).  But we have separated to make the code
//  a little clearer for this lab.
//
//  Author: Walker White, Aidan Hobler
//  Version: 2/8/22
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "NLHostScene.h"

using namespace cugl;
using namespace cugl::net;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

/**
 * Converts a hexadecimal string to a decimal string
 *
 * This function assumes that the string is 4 hexadecimal characters
 * or less, and therefore it converts to a decimal string of five
 * characters or less (as is the case with the lobby server). We
 * pad the decimal string with leading 0s to bring it to 5 characters
 * exactly.
 *
 * @param hex the hexadecimal string to convert
 *
 * @return the decimal equivalent to hex
 */
static std::string hex2dec(const std::string hex) {
    Uint32 value = strtool::stou32(hex,0,16);
    std::string result = strtool::to_string(value);
    if (result.size() < 5) {
        size_t diff = 5-result.size();
        std::string alt(5,'0');
        for(size_t ii = 0; ii < result.size(); ii++) {
            alt[diff+ii] = result[ii];
        }
        result = alt;
    }
    return result;
}

#pragma mark -
#pragma mark Provided Methods
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
bool HostScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("host");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD

    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("host_center_start"));
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("host_back"));
    _gameid = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("host_center_game_field_text"));
    _player = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("host_center_players_field_text"));
    _status = Status::WAIT;
    
    // Program the buttons
    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            disconnect();
//            _firstconnected = true;
	    	_status = Status::ABORT;
        }
    });

    _startgame->addListener([this](const std::string& name, bool down) {
        if (down) {
            startGame();
//            _firstconnected = true;
        }
    });
    
    // Create the server configuration
    auto json = _assets->get<JsonValue>("server");
    _config.set(json);
    
    addChild(scene);
    setActive(false);
    _firstconnected = true;
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void HostScene::dispose() {
    if (_active) {
        removeAllChildren();
        _network = nullptr;
        _active = false;
        _firstconnected = true;
    }
}

/**
 * Sets whether the scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void HostScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _status = WAIT;
            configureStartButton();
            _backout->activate();
            _firstconnected = true;
            connect();
        } else {
            _startgame->deactivate();
            _backout->deactivate();
            // If any were pressed, reset them
            _startgame->setDown(false);
            _backout->setDown(false);
        }
    }
}


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
void HostScene::updateText(const std::shared_ptr<scene2::Button>& button, const std::string text) {
    auto label = std::dynamic_pointer_cast<scene2::Label>(button->getChildByName("up")->getChildByName("label"));
    label->setText(text);

}

#pragma mark -
#pragma mark Student Methods
/**
 * The method called to update the scene.
 *
 * We need to update this method to constantly talk to the server
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void HostScene::update(float timestep) {
    // We have written this for you this time
    if (_network) {
        _network->receive([this](const std::string source,
                                 const std::vector<std::byte>& data) {
            processData(source,data);
        });
        checkConnection();
        // Do this last for button safety
        configureStartButton();
    }
}

/**
 * Processes data sent over the network.
 *
 * Once connection is established, all data sent over the network consistes of
 * byte vectors. This function is a call back function to process that data.
 * Note that this function may be called *multiple times* per animation frame,
 * as the messages can come from several sources.
 *
 * In this lab, this method does not do all that much. Typically this is where
 * players would communicate their names after being connected.
 *
 * @param source    The UUID of the sender
 * @param data      The data received
 */
void HostScene::processData(const std::string source,
                            const std::vector<std::byte>& data) {
    // No real data is handled in this scene
}


/**
 * Connects to the game server as specified in the assets file
 *
 * The {@link #init} method set the configuration data. This method simply uses
 * this to create a new {@Link NetworkConnection}. It also immediately calls
 * {@link #checkConnection} to determine the scene state.
 *
 * @return true if the connection was successful
 */
bool HostScene::connect() {
    // IMPLEMENT ME
    _network = cugl::net::NetcodeConnection::alloc(_config);
    if (!_network) return false;
    _network->open();
    return checkConnection();
//    return true;
}

/**
 * Checks that the network connection is still active.
 *
 * Even if you are not sending messages all that often, you need to be calling
 * this method regularly. This method is used to determine the current state
 * of the scene.
 *
 * @return true if the network connection is still active.
 */
bool HostScene::checkConnection() {
    auto net_state = _network->getState();
    switch (net_state){
        case NetcodeConnection::State::NEGOTIATING:
            _status = WAIT;
            break;
        case NetcodeConnection::State::CONNECTED:{
            if (_firstconnected){
                _firstconnected = false;
                _status = IDLE;
                
            }
            //            else{
            //                _status = START;
            //            }
            
            auto roomidraw = _network->getRoom();
//            CULog("get room id: %s\n", roomidraw.c_str());
            auto roomidhex = hex2dec(roomidraw);
            _gameid->setText(roomidhex);
            _player->setText(to_string(_network->getNumPlayers()));
            
            break;
        }
            
            
        case NetcodeConnection::State::DENIED:
        case NetcodeConnection::State::MISMATCHED:
        case NetcodeConnection::State::INVALID:
        case NetcodeConnection::State::FAILED:
        case NetcodeConnection::State::DISCONNECTED:
            _status = WAIT;
            return false;
        default:
            break;
    }
    
    
    
    return true;
}

/**
 * Reconfigures the start button for this scene
 *
 * This is necessary because what the buttons do depends on the state of the
 * networking.
 */
void HostScene::configureStartButton() {
    // THIS IS WRONG. FIX ME.
    
    if ((_status == IDLE) or (_status == START)){
        updateText(_startgame, "Start Game");
        _startgame->activate();
    }else{
        updateText(_startgame,"Waiting");
        _startgame->deactivate();
    }
}


/**
 * Starts the game.
 *
 * This method is called once the requisite number of players have connected.
 * It locks down the room and sends a "start game" message to all other
 * players.
 */
void HostScene::startGame() {
    _network->broadcast({std::byte{255}});
    _status = Status::START;
    // ADD YOUR CODE HERE
}


