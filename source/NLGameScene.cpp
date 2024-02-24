//
//  NLGameScene.cpp
//  Network Lab
//
//  This class provides a simple game.  We just keep track of the connection
//  and trade color values back-and-forth across the network.
//
//  Author: Walker White, Aidan Hobler
//  Version: 2/8/22
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>
#include <cstddef>
#include <vector>

#include "NLGameScene.h"

using namespace cugl;
using namespace cugl::net;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720


#pragma mark -
#pragma mark Provided Methods
/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
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
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("game");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game_back"));
    _player = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("game_players_field_text"));
    _white = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game_buttons_white"));
    _red = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game_buttons_red"));
    _green = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game_buttons_green"));
    _blue = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game_buttons_blue"));
    _yellow = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game_buttons_yellow"));
    _cyan = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game_buttons_cyan"));
    _magenta = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game_buttons_magenta"));
    _black = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game_buttons_black"));
    _grey = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game_buttons_grey"));
    _quit = false;
    
    _backout->addListener([=](const std::string& name, bool down) {
        if (down) {
            _quit = true;
            Application::get()->setClearColor(Color4("#c0c0c0"));
        }
    });

    // Activate all the buttons
    _white->addListener([this](const std::string& name, bool down) {
        if (down) {
            // Ideally we would get the button color, but down buttons are DARKENED
            // This lab was finished too last minute to fix this
            transmitColor(Color4::WHITE);
        }
    });

    _red->addListener([this](const std::string& name, bool down) {
        if (down) {
            transmitColor(Color4::RED);
        }
    });

    _green->addListener([this](const std::string& name, bool down) {
        if (down) {
            transmitColor(Color4::GREEN);
        }
    });
    
    _blue->addListener([this](const std::string& name, bool down) {
        if (down) {
            transmitColor(Color4::BLUE);
        }
    });

    _yellow->addListener([this](const std::string& name, bool down) {
        if (down) {
            transmitColor(Color4::YELLOW);
        }
    });

    _cyan->addListener([this](const std::string& name, bool down) {
        if (down) {
            transmitColor(Color4::CYAN);
        }
    });

    _magenta->addListener([this](const std::string& name, bool down) {
        if (down) {
            transmitColor(Color4::MAGENTA);
        }
    });

    _black->addListener([this](const std::string& name, bool down) {
        if (down) {
            transmitColor(Color4::BLACK);
        }
    });

    _grey->addListener([this](const std::string& name, bool down) {
        if (down) {
            transmitColor(Color4("#888888"));
        }
    });

    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _network = nullptr;
        _backout = nullptr;
        _player = nullptr;
        _white = nullptr;
        _red = nullptr;
        _green = nullptr;
        _blue = nullptr;
        _yellow = nullptr;
        _cyan = nullptr;
        _magenta = nullptr;
        _black = nullptr;
        _grey = nullptr;
        _active = false;
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
void GameScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _quit = false;
            _backout->activate();
            _white->activate();
            _red->activate();
            _green->activate();
            _blue->activate();
            _yellow->activate();
            _cyan->activate();
            _magenta->activate();
            _black->activate();
            _grey->activate();
        } else {
            _backout->deactivate();
            _white->deactivate();
            _red->deactivate();
            _green->deactivate();
            _blue->deactivate();
            _yellow->deactivate();
            _cyan->deactivate();
            _magenta->deactivate();
            _black->deactivate();
            _grey->deactivate();
            // If any were pressed, reset them
            _backout->setDown(false);
            _white->setDown(false);
            _red->setDown(false);
            _green->setDown(false);
            _blue->setDown(false);
            _yellow->setDown(false);
            _cyan->setDown(false);
            _magenta->setDown(false);
            _black->setDown(false);
            _grey->setDown(false);
        }
    }
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
void GameScene::update(float timestep) {
    if (_network) {
        _network->receive([this](const std::string source,
                                 const std::vector<std::byte>& data) {
            processData(source,data);
        });
        bool connection = checkConnection();
        if (!connection){
            _network->close();
            Application::get()->setClearColor(Color4(192,192,192,255));
            _quit = true;
        }
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
 * This is where we handle the gameplay. All connected devices should immediately
 * change their color when directed by the following method. Changing the color
 * means changing the clear color of the entire {@link Application}.
 *
 * @param source    The UUID of the sender
 * @param data  The data received
 */
void GameScene::processData(const std::string source,
                            const std::vector<std::byte>& data) {
    if(data.size() > 0){
        Color4 color = Color4((uint8)data[0],(uint8)data[1],(uint8)data[2],(uint8)data[3]);
        Application::get()->setClearColor(color);
    }
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
bool GameScene::checkConnection() {
    // IMPLEMENT ME
    auto net_state = _network->getState();
    switch (net_state){
        case NetcodeConnection::State::CONNECTED:{
            _player->setText(to_string(_network->getNumPlayers()));
            break;
        }
        case NetcodeConnection::State::FAILED:
        case NetcodeConnection::State::DISCONNECTED:
            return false;
        default:
            break;
    }
    return true;
}

/**
 * Transmits a color change to all other devices.
 *
 * Because a device does not receive messages from itself, this method should
 * also set the color (the clear color of the {@link Application} that is).
 *
 * @param color The new color
 */
void GameScene::transmitColor(Color4 color) {
    Application::get()->setClearColor(color);
    std::vector<std::byte> color_data = {std::byte{color.r},std::byte{color.g},std::byte{color.b},std::byte{color.a}};
    _network->broadcast(color_data);
}
