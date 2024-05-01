//
//  DPApp.cpp
//
//  Created by Xilai Dai on 3/8/24.
//

#include "DPApp.h"
#include <stdio.h>

using namespace cugl;

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
    
    auto savedir = Application::get()->getSaveDirectory();
    CULog("save directory is: %s", savedir.c_str());
    auto saveReader = JsonReader::alloc(savedir + "/save.json");
    if (saveReader != nullptr) {
        std::cout << "one line from save.json" << saveReader->readLine() << std::endl;
    }

    _assets2 = _assets; // the assets dedicated for game scene, currently is still the global asset manager
    
    std::cout << "The Application's Save Directory is: " << Application::getSaveDirectory() << std::endl;
    std::string root = cugl::Application::get()->getSaveDirectory();
    std::string path = cugl::filetool::join_path({root,"save.json"});
    std::cout << path << std::endl;
    _gameProgress = JsonReader::alloc(path);
    if (_gameProgress != nullptr) {
        std::cout << "game progress: " << _gameProgress -> readLine() << std::endl;
        std::cout << "From DpApp::updateLoad() - game progress json is loaded!" << std::endl;
    }
    


    _levelLoadScene.init(_assets2);
    // _assets->loadAsync<LevelLoader>(ALPHA_RELEASE_KEY, ALPHA_RELEASE_FILE,
    //                                 [](const std::string key, bool success) {
    //                                     if (success) {
    //                                         // Handle successful loading
    //                                         std::cout << "Successfully loaded resource with key: " << key << std::endl;
    //                                     } else {
    //                                         // Handle failure
    //                                         std::cout << "Failed to load resource with key: " << key << std::endl;
    //                                     }
    //                                 });
    
    
    cugl::net::NetworkLayer::start(net::NetworkLayer::Log::INFO);
    setDeterministic(true);
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
        // update json file

//        std::string root = cugl::Application::get()->getSaveDirectory();
//        std::string path = cugl::filetool::join_path({root,"GameProgress.json"});
//        std::shared_ptr<JsonValue> jv = JsonValue::allocObject();
//        jv -> appendValue(this -> _currentLevelKey, _gameScene.defaultGoodOrBad == 0? "good" : _gameScene.defaultGoodOrBad == 1? "bad" : "default");
//        _progressWriter = JsonWriter::alloc(path);
        auto savedir = Application::get()->getSaveDirectory();
        auto saveWriter = JsonWriter::alloc(savedir + "/save.json");
        if (saveWriter != nullptr) {
            saveWriter->writeLine(_currentLevelKey);
        }
        saveWriter->flush();
        saveWriter->close();
        std::cout << "preUpdate writing line to save.json: " << _currentLevelKey << std::endl;
        if (_gameScene.state == GameScene::QUIT) {
            _gameScene.reset();
            _levelSelectScene.setActive(true);
            _audioController.play("menu", "menu");
            _status = LEVELSELECT;
        } else if (_gameScene.state == GameScene::RESET) {
            _gameScene.reset();
            _levelLoadScene.loadFileAsync(_levelSelectScene.getSelectedLevelFile(), _levelSelectScene.getSelectedLevelKey());
            _status = LEVELLOAD;
        } else if (_gameScene.state == GameScene::NEXTLEVEL) {
            // TODO: next level
            _gameScene.reset();
            if (_currentLevelKey == "tutorial") {
                // TODO: handle tutorial case
            } else {
                char nextLevelNum = _currentLevelKey.back() + 1;
                std::string nextLevel = "level";
                nextLevel.push_back(nextLevelNum);
                std::cout << "next level number is: " << nextLevel << std::endl;
                _levelSelectScene.selectedLevelKey = nextLevel;
                _levelSelectScene.selectedLevelFile = "json/" + nextLevel + ".json";
                _levelLoadScene.loadFileAsync(_levelSelectScene.getSelectedLevelFile(), _levelSelectScene.getSelectedLevelKey());
                _currentLevelKey = _levelSelectScene.getSelectedLevelKey();
                _gameScene.setCameraSkip(false);
                _status = LEVELLOAD;
            }
        }
    } else if (_gameScene.isPaused()) {
        _status = PAUSE;
        _gameScene.setActive(false);
        _pauseScene.setActive(true);
    } else if (_gameScene.state == GameScene::RESET){
        _gameScene.setActive(false);
        _gameScene.reset();
        _levelLoadScene.loadFileAsync(_levelSelectScene.getSelectedLevelFile(), _levelSelectScene.getSelectedLevelKey());
        _status = LEVELLOAD;
    }
    else {
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
        if (!_gameScene.isComplete()){
            _gameScene.postUpdate(timestep);
        }
    }
}

void DPApp::fixedUpdate() {
      if (_status == GAME) {
          if (!_gameScene.isComplete()){
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
            // TODO: start tutorial!
//            _assets->loadAsync<LevelLoader2>("tutorial", "json/tutorial.json", [&](const std::string key2, bool success2) {
//                if (!success2) {
//                    CULog("Failed to tutorial");
//                }
//                CULog("Loaded tutorial");
//                _menuScene.startTutorial();
//            });
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

void DPApp::updateLevelLoad(float timestep){
    switch (_levelLoadScene._state) {
        case LevelLoadScene::LOADING:
        case LevelLoadScene::CONSTRUCT:
            _levelLoadScene.update(timestep);
            break;
        case LevelLoadScene::DONE:
            _gameScene.init(_assets2, _levelSelectScene.getSelectedLevelKey());
            _gameScene.setActive(true);
            _gameScene.setComplete(false);
            _gameScene.setCameraSkip(true);
            _audioController.clear("menu");
            _status = GAME;
            break;
    }
}


void DPApp::updateRestoration(float timestep) {
    _restorationScene.update(timestep);
}


void DPApp::updateHost(float timestep) {

}

void DPApp::updateClient(float timestep) {

}

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
