#include <stdio.h>

#include <iostream>

#include "LevelSelectScene.h"

#include <iostream>
#include <sstream>

using namespace cugl;
using namespace cugl::net;
using namespace std;

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

#pragma mark Helpers
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


#pragma mark Class Methods
    bool LevelSelectScene::init(const std::shared_ptr<cugl::AssetManager> &assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }

    //    _network = network;

    // Start up the input handler
    _assets = assets;

    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("level");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD

    _level1 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level1"));
    _level2 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level2"));
    _level3 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level3"));
    _level4 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level4"));
    _level5 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level5"));

    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_home"));

    // Program the buttons
    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            this -> state = BACK;
        }
    });
    _level1->addListener([this](const std::string& name, bool down) {
    if (down) {
        CULog("Level 1 selected");
        selectedLevelFile = "json/level1.json";
        selectedLevelKey = "level1";
        startGame();
    }
    });

    _level2->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 2 selected");
            this->selectedLevelFile = "json/doodlejump.json";
            this->selectedLevelKey = "doodlejump";
            startGame();
        }
    });

    _level3->addListener([this](const std::string& name, bool down) {
    if (down) {
        CULog("Level 3 selected");
        selectedLevelFile = "json/falldown.json";
        selectedLevelKey = "falldown";
        startGame();
    }
    });

    _level4->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 4 selected");
            selectedLevelFile = "json/tubelevel.json";
            selectedLevelKey = "tube";
            startGame();
        }
    });

    _level5->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 5 selected");
            // startGame();
        }
    });


    addChild(scene);
    setActive(false);
    return true;
    }

bool LevelSelectScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetEventController> network) {
    // no, we don't use this. we are single-player.
    return false;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LevelSelectScene::dispose() {
    if (_active) {
        removeAllChildren();
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
void LevelSelectScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _level1->activate();
            _level2->activate();
            _level3->activate();
            _level4->activate();
            _level5->activate();
            _backout->activate();

            state = INSCENE;
        } else {
            _level1->deactivate();
            _level2->deactivate();
            _level3->deactivate();
            _level4->deactivate();
            _level5->deactivate();
            _backout->deactivate();

            _level1->setDown(false);
            _level2->setDown(false);
            _level3->setDown(false);
            _level4->setDown(false);
            _level5->setDown(false);
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
void LevelSelectScene::updateText(const std::shared_ptr<scene2::Button>& button, const std::string text) {
//    auto label = std::dynamic_pointer_cast<scene2::Label>(button->getChildByName("up")->getChildByName("label"));
//    label->setText(text);

}


/**
 * The method called to update the scene.
 *
 * We need to update this method to constantly talk to the server
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void LevelSelectScene::update(float timestep) {
//    if(_network->getStatus() == NetEventController::Status::CONNECTED){
//        if (this -> state != STARTGAME) {
//            updateText(_startgame, "Start Game");
//            _startgame->activate();
//        }
//        else {
//            updateText(_startgame, "Starting");
//            _startgame->deactivate();
//        }
//        _gameid->setText(hex2dec(_network->getRoomID()));
//        _player->setText(std::to_string(_network->getNumPlayers()));
//    } else if (_network->getStatus() == NetEventController::Status::HANDSHAKE && _network->getShortUID()) {
//        state = HANDSHAKE;
//    } else if (_network->getStatus() == NetEventController::Status::INGAME) {
//        state = STARTGAME;
//    } else if (_network->getStatus() == NetEventController::Status::NETERROR) {
//        state = NETERROR;
//    }
}

/**
 * This method prompts the network controller to start the game.
 */
void LevelSelectScene::startGame(){
//    _network->startGame();
    // this -> state = STARTGAME;
    this -> state = STARTGAME;
}
