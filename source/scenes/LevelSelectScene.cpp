#include "LevelSelectScene.h"
#include <sstream>
#include <stdio.h>
#include <iostream>


using namespace cugl;
using namespace cugl::net;
using namespace std;

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

void LevelSelectScene::addButtons(std::shared_ptr<cugl::AssetManager> _assets){
    // Program the buttons
    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            this -> state = BACK;
        }
    });
    
    _level1->addListener([this](const std::string& name, bool down) {
    if (down) {
        selectedLevelFile = "json/level1.json";
        selectedLevelKey = "level1";
        selectedLevelAssets = "json/level1_assets.json";
        startGame();
    }
    });

    _level2->addListener([this](const std::string& name, bool down) {
        if (down) {
            selectedLevelFile = "json/level2.json";
            selectedLevelKey = "level2";
            selectedLevelAssets = "json/level2_assets.json";
            startGame();
        }
    });

    _level3->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 3 selected");
            selectedLevelFile = "json/level3.json";
            selectedLevelKey = "level3";
            selectedLevelAssets = "json/level3_assets.json";
            startGame();
        }
    });
    _level4->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 4 selected");
            selectedLevelFile = "json/level4.json";
            selectedLevelKey = "level4";
            selectedLevelAssets = "json/level4_assets.json";
            startGame();
        }
    });
    _level5->addListener([this](const std::string& name, bool down) {
    if (down) {
        CULog("Level 5 selected");
        selectedLevelFile = "json/level5.json";
        selectedLevelKey = "level5";
        selectedLevelAssets = "json/level5_assets.json";
        startGame();
    }
    });
    _level6->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 6 selected");
            selectedLevelFile = "json/level6.json";
            selectedLevelKey = "level6";
            selectedLevelAssets = "json/level6_assets.json";
            startGame();
        }
    });
    _level7->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 7 selected");
            selectedLevelFile = "json/level7.json";
            selectedLevelKey = "level7";
            selectedLevelAssets = "json/level7_assets.json";
            startGame();
        }
    });
    _level8->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 8 selected");
            selectedLevelFile = "json/level8.json"; // Ensure this file exists in your assets
            selectedLevelKey = "level8";
            selectedLevelAssets = "json/level8_assets.json";
            startGame();
        }
    });
    _level9->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 9 selected");
            selectedLevelFile = "json/level9.json"; // Ensure this file exists in your assets
            selectedLevelKey = "level9";
            selectedLevelAssets = "json/level9_assets.json";
            startGame();
        }
    });
    _level10->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 10 selected");
            selectedLevelFile = "json/level10.json"; // Ensure this file exists in your assets
            selectedLevelKey = "level10";
            selectedLevelAssets = "json/level10_assets.json";
            startGame();
        }
    });
    
    _level11->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 14 selected");
            selectedLevelFile = "json/level11.json"; // Ensure this file exists in your assets
            selectedLevelKey = "level11";
            selectedLevelAssets = "json/level11_assets.json";
            startGame();
        }
    });
    
    _level12->addListener([this](const std::string& name, bool down) {
    if (down) {
        CULog("Level 12 selected");
        selectedLevelFile = "json/level12.json"; // Ensure this file exists in your assets
        selectedLevelKey = "level12";
        selectedLevelAssets = "json/level12_assets.json";
        startGame();
    }
    });
    _level13->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 13 selected");
            selectedLevelFile = "json/level13.json"; // Ensure this file exists in your assets
            selectedLevelKey = "level13";
            selectedLevelAssets = "json/level13_assets.json";
            startGame();
        }
    });
    _level14->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 14 selected");
            selectedLevelFile = "json/level14.json"; // Ensure this file exists in your assets
            selectedLevelKey = "level14";
            selectedLevelAssets = "json/level14_assets.json";
            startGame();
        }
    });
    _level15->addListener([this](const std::string& name, bool down) {
        if (down) {
            CULog("Level 14 selected");
            selectedLevelFile = "json/level15.json"; // Ensure this file exists in your assets
            selectedLevelKey = "level15";
            selectedLevelAssets = "json/level15_assets.json";
            startGame();
        }
    });
}

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
        _level6 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level6"));
        _level7 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level7"));
        _level8 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level8"));
        _level9 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level9"));
        _level10 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level10"));
        _level11 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level11"));
        _level12 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level12"));
        _level13 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level13"));
        _level14 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level14"));
        _level15 = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_levels_level15"));
        _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("level_home"));
    addButtons(_assets);
    addChild(scene);
    
    setActive(false);
    
    return true;
}

void LevelSelectScene::newGame(){
    // update level description file
    selectedLevelFile = "json/tutorial.json";
    selectedLevelKey = "tutorial";
    selectedLevelAssets = "json/tutorial_assets.json";

}


void LevelSelectScene::go_to_next_level() {
    // update level description file
    if (selectedLevelFile == "json/tutorial.json"){
        selectedLevelFile = "json/level1.json";
    }else if (selectedLevelFile == "json/level1.json"){
        selectedLevelFile = "json/level2.json";
    }else if (selectedLevelFile == "json/level2.json"){
        selectedLevelFile = "json/level3.json";
    }else if (selectedLevelFile == "json/level3.json"){
        selectedLevelFile = "json/level4.json";
    }else if (selectedLevelFile == "json/level4.json"){
        selectedLevelFile = "json/level5.json";
    }else if (selectedLevelFile == "json/level5.json"){
        selectedLevelFile = "json/level6.json";
    }else if (selectedLevelFile == "json/level6.json"){
        selectedLevelFile = "json/level7.json";
    }else if (selectedLevelFile == "json/level7.json"){
        selectedLevelFile = "json/level8.json";
    }else if (selectedLevelFile == "json/level8.json"){
        selectedLevelFile = "json/level9.json";
    }else if (selectedLevelFile == "json/level9.json"){
        selectedLevelFile = "json/level10.json";
    }else if (selectedLevelFile == "json/level10.json"){
        selectedLevelFile = "json/level11.json";
    }else if (selectedLevelFile == "json/level11.json"){
        selectedLevelFile = "json/level12.json";
    }else if (selectedLevelFile == "json/level12.json"){
        selectedLevelFile = "json/level13.json";
    }else if (selectedLevelFile == "json/level13.json"){
        selectedLevelFile = "json/level14.json";
    }else if (selectedLevelFile == "json/level14.json"){
        selectedLevelFile = "json/level15.json";
    }else if (selectedLevelFile == "json/level15.json"){
        CULog("No more levels to go to!");
        selectedLevelFile = "json/level15.json";
    }else{
        CULog("Error in level selection, defaulting to level 1.");
        selectedLevelFile = "json/level1.json";
    }


    // update level key
    if (selectedLevelKey == "tutorial"){
        selectedLevelKey = "level1";
    }else if (selectedLevelKey == "level1"){
        selectedLevelKey = "level2";
    }else if (selectedLevelKey == "level2"){
        selectedLevelKey = "level3";
    }else if (selectedLevelKey == "level3"){
        selectedLevelKey = "level4";
    }else if (selectedLevelKey == "level4"){
        selectedLevelKey = "level5";
    }else if (selectedLevelKey == "level5"){
        selectedLevelKey = "level6";
    }else if (selectedLevelKey == "level6"){
        selectedLevelKey = "level7";
    }else if (selectedLevelKey == "level7"){
        selectedLevelKey = "level8";
    }else if (selectedLevelKey == "level8"){
        selectedLevelKey = "level9";
    }else if (selectedLevelKey == "level9"){
        selectedLevelKey = "level10";
    }else if (selectedLevelKey == "level10"){
        selectedLevelKey = "level11";
    }else if (selectedLevelKey == "level11"){
        selectedLevelKey = "level12";
    }else if (selectedLevelKey == "level12"){
        selectedLevelKey = "level13";
    }else if (selectedLevelKey == "level13"){
        selectedLevelKey = "level14";
    }else if (selectedLevelKey == "level14"){
        selectedLevelKey = "level15";
    }else if (selectedLevelKey == "level15"){
        CULog("No more levels to go to!");
        selectedLevelKey = "level15";
    }else{
        CULog("Error in level selection, defaulting to level 1.");
        selectedLevelKey = "level1";
    }

    // update level assets
    if (selectedLevelAssets == "json/tutorial_assets.json"){
        selectedLevelAssets = "json/level1_assets.json";
    }else if (selectedLevelAssets == "json/level1_assets.json"){
        selectedLevelAssets = "json/level2_assets.json";
    }else if (selectedLevelAssets == "json/level2_assets.json"){
        selectedLevelAssets = "json/level3_assets.json";
    }else if (selectedLevelAssets == "json/level3_assets.json"){
        selectedLevelAssets = "json/level4_assets.json";
    }else if (selectedLevelAssets == "json/level4_assets.json"){
        selectedLevelAssets = "json/level5_assets.json";
    }else if (selectedLevelAssets == "json/level5_assets.json"){
        selectedLevelAssets = "json/level6_assets.json";
    }else if (selectedLevelAssets == "json/level6_assets.json"){
        selectedLevelAssets = "json/level7_assets.json";
    }else if (selectedLevelAssets == "json/level7_assets.json"){
        selectedLevelAssets = "json/level8_assets.json";
    }else if (selectedLevelAssets == "json/level8_assets.json"){
        selectedLevelAssets = "json/level9_assets.json";
    }else if (selectedLevelAssets == "json/level9_assets.json"){
        selectedLevelAssets = "json/level10_assets.json";
    }else if (selectedLevelAssets == "json/level10_assets.json"){
        selectedLevelAssets = "json/level11_assets.json";
    }else if (selectedLevelAssets == "json/level11_assets.json"){
        selectedLevelAssets = "json/level12_assets.json";
    }else if (selectedLevelAssets == "json/level12_assets.json"){
        selectedLevelAssets = "json/level13_assets.json";
    }else if (selectedLevelAssets == "json/level13_assets.json"){
        selectedLevelAssets = "json/level14_assets.json";
    }else if (selectedLevelAssets == "json/level14_assets.json"){
        selectedLevelAssets = "json/level15_assets.json";
    }else if (selectedLevelAssets == "json/level15_assets.json"){
        CULog("No more levels to go to!");
        selectedLevelAssets = "json/level15_assets.json";
    }else{
        CULog("Error in level selection, defaulting to level 1.");
        selectedLevelAssets = "json/level1_assets.json";
    }
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
            _level6->activate();
            _level7->activate();
            _level8->activate();
            _level9->activate();
            _level10->activate();
            _level11->activate();
            _level12->activate();
            _level13->activate();
            _level14->activate();
            _level15->activate();
//            unlockAllLevels();
            updateLevelStatus();
            _backout->activate();
            
            addButtons(_assets);
            state = INSCENE;
        } else {
            _level1->deactivate();
            _level2->deactivate();
            _level3->deactivate();
            _level4->deactivate();
            _level5->deactivate();
            _level6->deactivate();
            _level7->deactivate();
            _level8->deactivate();
            _level9->deactivate();
            _level10->deactivate();
            _level11->deactivate();
            _level12->deactivate();
            _level13->deactivate();
            _level14->deactivate();
            _level15->deactivate();
            _backout->deactivate();

            _level1->setDown(false);
            _level2->setDown(false);
            _level3->setDown(false);
            _level4->setDown(false);
            _level5->setDown(false);
            _level6->setDown(false);
            _level7->setDown(false);
            _level8->setDown(false);
            _level9->setDown(false);
            _level10->setDown(false);
            _level11->setDown(false);
            _level12->setDown(false);
            _level13->setDown(false);
            _level14->setDown(false);
            _level15->setDown(false);
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

void LevelSelectScene::lockOrUnlockLevelButton(int levelNumber, bool isLock) {
    //for activating and deactivating correct button
    std::string levelString = "level_levels_level";
    std::string levelNum = std::to_string(levelNumber);
    std::string buttonNodeString = levelString + levelNum;
    
    levelString = levelString + levelNum + "_up";
    std::string buttonString = "button_level" + levelNum;
    
    auto _levelButtonImageNode = std::dynamic_pointer_cast<scene2::PolygonNode>(_assets->get<scene2::SceneNode>(levelString)-> getChildByName(buttonString));
    
    //for setting the correct texture of the level select button
    std::string newTextureString = buttonString + (isLock ? "_locked" : "");
    
    auto savedir = Application::get()->getSaveDirectory();
    std::shared_ptr<JsonReader> jsonreader = JsonReader::alloc(savedir + "user_progress.json");
    
    //delete to unlock all
    if (jsonreader != nullptr && jsonreader->ready() && !isLock && jsonreader->readLine()[0] == '{'){
        jsonreader->reset();
        std::shared_ptr<JsonValue> jsonobj = jsonreader->readJson();
        if (jsonobj->get("level"+std::to_string(levelNumber)) != nullptr){
            if (jsonobj->get("level"+std::to_string(levelNumber))->getString("endingType") == "0"){
                newTextureString = "button_level"+std::to_string(levelNumber)+"_good";
            } else if (jsonobj->get("level"+std::to_string(levelNumber))->getString("endingType") == "1"){
                newTextureString = "button_level"+std::to_string(levelNumber)+"_bad";
            }
        }
    }
    //delete to unlock all
    

    auto newTexture = _assets->get<Texture>(newTextureString);
    _levelButtonImageNode->setTexture(newTexture);
    Size contentDimen = _assets->get<scene2::SceneNode>(levelString)-> getChildByName(buttonString)->getContentSize();
    _levelButtonImageNode->setContentSize(contentDimen.width, contentDimen.height);
    if (isLock) {
        std::dynamic_pointer_cast<scene2::Button>(_assets -> get<scene2::SceneNode>(buttonNodeString)) -> setDown(false);
        std::dynamic_pointer_cast<scene2::Button>(_assets -> get<scene2::SceneNode>(buttonNodeString)) -> deactivate();
    } else {
        std::dynamic_pointer_cast<scene2::Button>(_assets -> get<scene2::SceneNode>(buttonNodeString)) -> setDown(false);
        std::dynamic_pointer_cast<scene2::Button>(_assets -> get<scene2::SceneNode>(buttonNodeString)) -> activate();
    }
    
//    std::cout << "locking or unlocking button " << levelNumber << "!" << std::endl;
}

void LevelSelectScene::lockAllLevels() {
    for (int i = 1; i <= 15; i ++) {
        lockOrUnlockLevelButton(i, true);
    }
}

void LevelSelectScene::unlockAllLevels() {
    for (int i = 1; i <= 15; i ++) {
        lockOrUnlockLevelButton(i, false);
    }
}

void LevelSelectScene::updateLevelStatus() {
    std::string root = cugl::Application::get()->getSaveDirectory();
    std::shared_ptr<JsonReader> _gameProgress = JsonReader::alloc(root+"user_progress.json");
    std::set<int> unlocked_levels;
    if (_gameProgress != nullptr && _gameProgress->ready() && _gameProgress->readLine()[0] == '{'){
        _gameProgress->reset();
        std::shared_ptr<JsonValue> jsonobj = _gameProgress->readJson();
        for (auto child : jsonobj->children()){
            if (child->key() == "tutorial"){
                continue;
            }
            std::string levelNumber = child->key().substr(5,child->key().length()-5);
            int num = std::stoi(levelNumber);
            std::cout << "(DEBUG: Level Number:) "<< num << std::endl;
            unlocked_levels.emplace(num);
        }
    } else {
        CULog("Error in json file");
    }
    
    if (_gameProgress != nullptr){
        _gameProgress->close();
    }
    
    
    //debugging purposes
//    for (auto i = unlocked_levels.begin(); i != unlocked_levels.end(); i++){
//        std::cout<< (*i) << std::endl;
//    }
    
    for (int i = 1; i <= 15; i ++) {
        lockOrUnlockLevelButton(i, false);
        
        //delete to unlock all
        if (unlocked_levels.find(i) != unlocked_levels.end()){
            lockOrUnlockLevelButton(i, false);
        } else {
            lockOrUnlockLevelButton(i, true);
        }
        //delete to unlock all
        
    }
}
