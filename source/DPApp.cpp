//
//  DPApp.cpp
//
//  Created by Xilai Dai on 3/8/24.
//

#include "DPApp.h"
#include <stdio.h>

using namespace cugl;

float acutal_input_scaler = 0.0015f;

#pragma mark STARTUP, SHUTDOWN
void DPApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch = SpriteBatch::alloc();

// Start-up basic input
#ifdef CU_TOUCH_SCREEN
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
#endif

    Input::activate<Keyboard>();
    Input::activate<TextInput>();

    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());
    _assets->attach<LevelLoader2>(GenericLoader<LevelLoader2>::alloc()->getHook());

    _loadScene.init(_assets);
    _status = LOAD;

    _loaded = false;
    // Que up the other assets
    AudioEngine::start(24);
    _assets->loadDirectoryAsync("json/assets.json", nullptr);

    _assets2 = _assets; // the assets dedicated for game scene, currently is still the global asset manager

    _levelLoadScene.init(_assets2);
    acutal_input_scaler = getInputScale();
    CULog("time for fixed update: %f", getFixedStep() / 1000000.0f);

    cugl::net::NetworkLayer::start(net::NetworkLayer::Log::INFO);
    setDeterministic(true);

    auto savedir = Application::get()->getSaveDirectory();
    std::shared_ptr<JsonReader> jsonreader = JsonReader::alloc(savedir + "user_progress.json");
    if (jsonreader == nullptr) {
        std::shared_ptr<JsonWriter> jsonwriter = JsonWriter::alloc(savedir + "user_progress.json");
        // std::cout<< "r-20" <<std::endl;
        JsonValue updatedjson;
        // std::cout<< "r-21" <<std::endl;
        std::shared_ptr<JsonValue> res = updatedjson.allocNull();
        if (jsonwriter != nullptr && res != nullptr) {
            jsonwriter->writeJson(res);
            // std::cout<< "r-23" <<std::endl;
        }
        jsonwriter->close();
    } else {
        jsonreader->close();
    }

    Application::onStartup(); // YOU MUST END with call to parent
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void DPApp::onShutdown() {
    _gameScene.dispose();
    _menuScene.dispose();
    //  _hostScene.dispose();
    //  _clientScene.dispose();
    _levelSelectScene.dispose();
    _settingScene.dispose();
    _restorationScene.dispose();
    _loadScene.dispose();
    _pauseScene.dispose();
    _assets = nullptr;
    _batch = nullptr;

// Shutdown input
#ifdef CU_TOUCH_SCREEN
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif

    AudioEngine::stop();
    Application::onShutdown(); // YOU MUST END with call to parent
}

/**
 * The method called when the application is suspended and put in the
 * background.
 *
 * When this method is called, you should store any state that you do not
 * want to be lost.  There is no guarantee that an application will return
 * from the background; it may be terminated instead.
 *
 * If you are using audio, it is critical that you pause it on suspension.
 * Otherwise, the audio thread may persist while the application is in
 * the background.
 */
void DPApp::onSuspend() {
    // TODO: Restoration Scene may come in here
    AudioEngine::get()->pause();
}

/**
 * The method called when the application resumes and put in the foreground.
 *
 * If you saved any state before going into the background, now is the time
 * to restore it. This guarantees that the application looks the same as
 * when it was suspended.
 *
 * If you are using audio, you should use this method to resume any audio
 * paused before app suspension.
 */
void DPApp::onResume() { AudioEngine::get()->resume(); }

#pragma mark -
#pragma mark GENERAL UPDATES

void DPApp::preUpdate(float timestep) {
    if (_status == LOAD) {
        // TODO: 0.01f? Why? Because Walker used this...
        updateLoad(0.01f);
    } else if (_status == MENU) {
        updateMenu(timestep);
    } else if (_status == SETTING) {
        updateSetting(timestep);
    } else if (_status == LEVELSELECT) {
        updateLevelSelect(timestep);
    } else if (_status == LEVELLOAD) {
        updateLevelLoad(timestep);
    } else if (_status == RESTORE) {
        updateRestoration(timestep);
    } else if (_status == GAME) {
        if (_gameScene.isComplete()) {
            if (_gameScene.state == GameScene::UPDATING) {
                updateLevelJson();
                _gameScene.state = GameScene::NETERROR; // using dummy state so it runs once
                // std::cout<<"updated the json and now moving on" <<std::endl;
            }
            if (_gameScene.state == GameScene::QUIT) {
                std::cout << "quitting game back to level select" << std::endl;
                _gameScene.reset();
                _levelSelectScene.setActive(true);
                _audioController.play("menu", "menu");
                _status = LEVELSELECT;
            } else if (_gameScene.state == GameScene::RESET) {
                _gameScene.reset();
                _levelLoadScene.loadFileAsync(_levelSelectScene.getSelectedLevelFile(), _levelSelectScene.getSelectedLevelKey());
                _gameScene.setCameraSkip(true);
                _status = LEVELLOAD;
            } else if (_gameScene.state == GameScene::NEXTLEVEL) {
                // TODO: next level
                _gameScene.reset();
                std::string nextLevel;
                if (_currentLevelKey == "tutorial") {
                    nextLevel = "level1";
                } else if (_currentLevelKey == "level15") {
                    // TODO: what to do with the final level?
                    nextLevel = "level1";
                } else {
                    char nextLevelNum = _currentLevelKey.at(_currentLevelKey.length() - 1);
                    nextLevel = "level";
                    if (nextLevelNum == '9') {
                        nextLevel.push_back(_currentLevelKey.at(_currentLevelKey.length() - 2) + 1);
                        nextLevel.push_back(nextLevelNum + 1);
                    } else {
                        nextLevel.push_back(nextLevelNum + 1);
                    }
                }
                std::cout << "next level number is: " << nextLevel << std::endl;
                _levelSelectScene.selectedLevelKey = nextLevel;
                _levelSelectScene.selectedLevelFile = "json/" + nextLevel + ".json";
                _levelLoadScene.loadFileAsync(_levelSelectScene.getSelectedLevelFile(), _levelSelectScene.getSelectedLevelKey());
                _currentLevelKey = _levelSelectScene.getSelectedLevelKey();
                _gameScene.setCameraSkip(false);
                _status = LEVELLOAD;
            }
        } else if (_gameScene.isPaused()) {
            _status = PAUSE;
            _gameScene.setActive(false);
            _pauseScene.setActive(true);
        } else if (_gameScene.state == GameScene::RESET) {
            _gameScene.setActive(false);
            _gameScene.reset();
            _levelLoadScene.loadFileAsync(_levelSelectScene.getSelectedLevelFile(), _levelSelectScene.getSelectedLevelKey());
            _status = LEVELLOAD;
        } else {
            _gameScene.preUpdate(timestep);
        }
    } else if (_status == PAUSE) {
        switch (_pauseScene.state) {
        case PauseScene::BACK:
            _pauseScene.setActive(false);
            _gameScene.setActive(true);
            _status = GAME;
            break;
        case PauseScene::RESET:
            _pauseScene.setActive(false);
            _gameScene.reset();
            _levelLoadScene.loadFileAsync(_levelSelectScene.getSelectedLevelFile(), _levelSelectScene.getSelectedLevelKey());
            _status = LEVELLOAD;
            break;
        case PauseScene::MENU:
            _pauseScene.setActive(false);
            _gameScene.reset();
            _levelSelectScene.setActive(true);
            _audioController.play("menu", "menu", true);
            _status = LEVELSELECT;
            break;
        default:
            break;
        }
    }
}

void DPApp::postUpdate(float timestep) {
    if (_status == GAME) {
        if (!_gameScene.isComplete()) {
            _gameScene.postUpdate(timestep);
        }
    }
}

void DPApp::fixedUpdate() {
    if (_status == GAME) {
        if (!_gameScene.isComplete()) {
            float time = getFixedStep() / 1000000.0f;
            _gameScene.fixedUpdate(time);
        }
    } else if (_status == PAUSE) {
        _pauseScene.fixedUpdate();
    }
}

#pragma mark SCENE-SPECIFIC UPDATES
/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom
 * implementation. This method should contain any code that is not an OpenGL
 * call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void DPApp::update(float timestep) {
    // deprecated
}

void DPApp::updateMenu(float timestep) {
    _menuScene.update(timestep);
    switch (_menuScene.status) {
    case MenuScene::NEWGAME:
        _levelSelectScene.selectedLevelKey = "tutorial";
        _levelSelectScene.selectedLevelFile = "json/tutorial.json";
        _levelLoadScene.loadFileAsync(_levelSelectScene.getSelectedLevelFile(), _levelSelectScene.getSelectedLevelKey());
        _currentLevelKey = _levelSelectScene.getSelectedLevelKey();
        _menuScene.setActive(false);
        _status = LEVELLOAD;
        break;
    case MenuScene::LEVEL:
        _menuScene.setActive(false);
        _levelSelectScene.setActive(true);
        _status = LEVELSELECT;
        break;
    case MenuScene::NONE:
        // DO NOTHING
        break;
    case MenuScene::SETTING:
        _menuScene.setActive(false);
        _settingScene.setActive(true);
        _status = SETTING;
        break;
    case MenuScene::QUIT:
        onShutdown();
        break;
    }
}

void DPApp::updatePause(float timestep) {
    _pauseScene.update(timestep);
    switch (_pauseScene.state) {
    case PauseScene::BACK:
        _status = GAME;
        _pauseScene.setActive(false);
        _gameScene.setActive(true);
        break;
    case PauseScene::RESET:
        _status = GAME;
        _pauseScene.setActive(false);
        _gameScene.reset();
        _gameScene.setActive(true);
        break;
    case PauseScene::MENU:
        _status = LEVELSELECT;
        _pauseScene.setActive(false);
        _gameScene.reset();
        break;
    default:
        break;
    }
}

void DPApp::updateLoad(float timestep) {
    switch (_loadScene.state) {
    case LoadScene::LOADING:
        _loadScene.update(0.01f);
        break;
    case LoadScene::LOADED:
        _network = NetEventController::alloc(_assets);
        _loadScene.dispose(); // Disables the input listeners in this mode
        _menuScene.init(_assets);
        _menuScene.setActive(true);
        _pauseScene.init(_assets, _network);
        _settingScene.init(_assets);
        _levelSelectScene.init(_assets);

        _audioController.init(_assets);
        _audioController.play("menu", "menu", true);
        _status = MENU;
        //    _hostScene.init(_assets, _network);
        //    _clientScene.init(_assets, _network);
        break;
    }
}

void DPApp::updateSetting(float timestep) {
    _settingScene.update(timestep);
    switch (_settingScene.state) {
    case SettingScene::BACK:
        _status = MENU;
        _settingScene.setActive(false);
        _menuScene.setActive(true);
        break;
    default:
        break;
    }
}

void DPApp::updateLevelSelect(float timestep) {
    _levelSelectScene.update(timestep);
    switch (_levelSelectScene.state) {
    case LevelSelectScene::BACK:
        _status = MENU;
        _levelSelectScene.setActive(false);
        _menuScene.setActive(true);
        break;
    case LevelSelectScene::STARTGAME:
        _gameScene.setCameraSkip(false);
        _levelSelectScene.setActive(false);
        _levelLoadScene.loadFileAsync(_levelSelectScene.getSelectedLevelFile(), _levelSelectScene.getSelectedLevelKey());
        _currentLevelKey = _levelSelectScene.getSelectedLevelKey();
        _status = LEVELLOAD;
        break;
    case LevelSelectScene::INSCENE:
        // Do nothing
        break;
    }
}

void DPApp::updateLevelLoad(float timestep) {
    switch (_levelLoadScene._state) {
    case LevelLoadScene::LOADING:
    case LevelLoadScene::CONSTRUCT:
        _levelLoadScene.update(timestep);
        break;
    case LevelLoadScene::DONE:
        _gameScene.init(_assets2, _levelSelectScene.getSelectedLevelKey());
        _gameScene.setActive(true);
        _gameScene.setComplete(false);
        _gameScene.setCameraSkip(false);

        _audioController.clear("menu");
        _status = GAME;
        break;
    }
}

void DPApp::updateRestoration(float timestep) { _restorationScene.update(timestep); }

void DPApp::updateHost(float timestep) {}

void DPApp::updateClient(float timestep) {}

#pragma mark DRAW
/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom
 * implementation. This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void DPApp::draw() {
    switch (_status) {
    case LOAD:
        _loadScene.render(_batch);
        break;
    case MENU:
        _menuScene.render(_batch);
        break;
    case GAME:
        _gameScene.render(_batch);
        break;
    case SETTING:
        _settingScene.render(_batch);
        break;
    case RESTORE:
        _restorationScene.render(_batch);
        break;
    case LEVELSELECT:
        _levelSelectScene.render(_batch);
        break;
    case PAUSE:
        _pauseScene.render(_batch);
        break;
    case LEVELLOAD:
        _levelLoadScene.render(_batch);
        break;
    }
}

void DPApp::addLevelJson(std::shared_ptr<JsonValue> json, std::string levelkey, bool completedfield, std::string endingtypefield) {
    std::cout << levelkey << std::endl;
    std::shared_ptr<JsonValue> level = json->get(levelkey);

    if (level != nullptr) {
        std::cout << "exists" << std::endl;
        std::shared_ptr<JsonValue> completed = level->get("completed");
        std::shared_ptr<JsonValue> endingType = level->get("endingType");

        if (completed != nullptr) {
            completed->set(completedfield);
        } else {
            level->appendValue("completed", completedfield);
        }

        if (endingType != nullptr) {
            std::cout << level->getString("endingType") << std::endl;
            if (level->getString("endingType").compare("0") != 0) {
                // don't update if already good ending for this level
                endingType->set(endingtypefield);
            }
        } else {
            level->appendValue("endingType", endingtypefield);
        }
        std::cout << level->get("completed")->toString() << std::endl;
    } else {
        std::cout << "doesn't exist" << std::endl;
        if (!json->isObject()) {
            // json file might be empty
            std::cout << "making init object" << std::endl;
            json->initObject();
        }
        // finished a new level
        std::cout << " appending object" << std::endl;
        json->appendObject(levelkey);
        std::shared_ptr<JsonValue> levelfields = json->get(levelkey);
        levelfields->appendValue("completed", completedfield);
        levelfields->appendValue("endingType", endingtypefield);
    }
    std::cout << json->toString() << std::endl;
    std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl;
    return;
}

void DPApp::updateLevelJson() {
    // std::cout<< "r-1" <<std::endl;
    auto savedir = Application::get()->getSaveDirectory();
    // std::cout<< "r-2" <<std::endl;
    std::shared_ptr<JsonReader> jsonreader = JsonReader::alloc(savedir + "user_progress.json");

    //    std::cout << jsonreader->readAll() <<std::endl;
    //    jsonreader->reset();

    // std::cout<< "r-3" <<std::endl;
    if (jsonreader != nullptr && jsonreader->ready()) {
        // std::cout<< "r-4" <<std::endl;
        std::shared_ptr<JsonValue> json;

        // std::cout<< "r-5" <<std::endl;
        if (jsonreader->readLine()[0] != '{') {
            // std::cout<< "r-6" <<std::endl;
            std::shared_ptr<JsonValue> myjson = JsonValue::allocObject();
            // std::cout<< "r-7" <<std::endl;
            json = myjson;
            // std::cout<< "r-8" <<std::endl;
        } else {
            jsonreader->reset();
            // std::cout<< "r-9" <<std::endl;
            json = jsonreader->readJson();
            // std::cout<< "r-10" <<std::endl;
            jsonreader->close();
            // std::cout<< "r-11" <<std::endl;
        }
        if (json != nullptr) {
            addLevelJson(json, _currentLevelKey, true, std::to_string(_gameScene.getEndingType()));
            // std::cout<< "r-12" <<std::endl;
            // now unlock new level
            if (_currentLevelKey == "tutorial") {
                addLevelJson(json, "level1", false, "null");
                // std::cout<< "r-13" <<std::endl;
            } else {
                size_t start = _currentLevelKey.find("level") + 5;
                // std::cout<< "r-14" <<std::endl;
                size_t end = _currentLevelKey.size();
                // std::cout<< "r-15" <<std::endl;
                int num = std::stoi(_currentLevelKey.substr(start, end - start)) + 1;
                // std::cout<< "r-16" <<std::endl;
                std::string next_level = "level" + std::to_string(num);
                // std::cout<< "r-17" <<std::endl;
                if (json->get(next_level) == nullptr) {
                    addLevelJson(json, next_level, false, "null");
                }

                // std::cout<< "r-18" <<std::endl;
            }
        } else {
            CULog("There is a parsing error in the json file");
        }

        // std::cout<< "r-19" <<std::endl;

        std::shared_ptr<JsonWriter> jsonwriter = JsonWriter::alloc(savedir + "user_progress.json");
        // std::cout<< "r-20" <<std::endl;
        JsonValue updatedjson;
        // std::cout<< "r-21" <<std::endl;
        std::shared_ptr<JsonValue> res = updatedjson.allocWithJson(json->toString());
        // std::cout<< "r-22" <<std::endl;
        if (jsonwriter != nullptr && res != nullptr) {
            jsonwriter->writeJson(res);
            // std::cout<< "r-23" <<std::endl;
        }
        jsonwriter->close();
        // std::cout<< "r-25" <<std::endl;

        std::cout << "wrote to json file" << std::endl;
    } else {
        if (jsonreader != nullptr) {
            jsonreader->close();
        }

        // std::cout<< "r-26" <<std::endl;
    }

    // std::cout<< "r-27" <<std::endl;

    return;
}