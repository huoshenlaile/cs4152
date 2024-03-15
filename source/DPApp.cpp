//
//  DPApp.cpp
//
//  Created by Xilai Dai on 3/8/24.
//

#include <stdio.h>
#include "DPApp.h"

using namespace cugl;


#pragma mark STARTUP, SHUTDOWN
void DPApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    
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
    _assets->attach<LevelLoader>(GenericLoader<LevelLoader>::alloc()->getHook());
    
    _loadScene.init(_assets);
    _status = LOAD;
    
    _loaded = false;
    // Que up the other assets
    AudioEngine::start(24);
    _assets->loadDirectoryAsync("json/assets.json",nullptr);
    _assets->loadAsync<LevelLoader>(LEVEL_ONE_KEY,LEVEL_ONE_FILE,nullptr);
    
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
    _hostScene.dispose();
    _clientScene.dispose();
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
    Application::onShutdown();  // YOU MUST END with call to parent
}


/**
 * The method called when the application is suspended and put in the background.
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
void DPApp::onResume() {
    AudioEngine::get()->resume();
}


#pragma mark -
#pragma mark GENERAL UPDATES

void DPApp::preUpdate(float timestep){
    if (_status == LOAD) {
        // TODO: 0.01f? Why? Because Walker used this...
        updateLoad(0.01f);
    }
    else if (_status == MENU) {
        updateMenu(timestep);
    }
    else if (_status == HOST){
        updateHost(timestep);
    }
    else if (_status == CLIENT){
        updateClient(timestep);
    }
    else if (_status == SETTING) {
        updateSetting(timestep);
    }
    else if (_status == LEVELSELECT) {
        updateLevelSelect(timestep);
    }
    else if (_status == RESTORE) {
        updateRestoration(timestep);
    }
    else if (_status == GAME){
        if(_gameScene.isComplete()){
            _gameScene.reset();
            _status = MENU;
            _menuScene.setActive(true);
        } else if(_gameScene.isPaused()){
            CULog("We pauseddddd");
            _status = PAUSE;
            _gameScene.setActive(false);
            _pauseScene.setActive(true);
        }
        _gameScene.preUpdate(timestep);
    } else if(_status == PAUSE){
        updatePause(timestep);
    }
}


void DPApp::postUpdate(float timestep) {
    if (_status == GAME) {
        _gameScene.postUpdate(timestep);
    }
}


void DPApp::fixedUpdate() {
    if (_status == GAME) {
        _gameScene.fixedUpdate();
    } else if(_status == PAUSE){
        _pauseScene.fixedUpdate();
    }
    if(_network){
        _network->updateNet();
    }
}


#pragma mark SCENE-SPECIFIC UPDATES
/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void DPApp::update(float timestep) {
    //deprecated
}


void DPApp::updateMenu(float timestep) {
    _menuScene.update(timestep);
    switch (_menuScene.status) {
        case MenuScene::HOST:
            _menuScene.setActive(false);
            _hostScene.setActive(true);
            _status = HOST;
            break;
        case MenuScene::JOIN:
            _menuScene.setActive(false);
            _clientScene.setActive(true);
            _status = CLIENT;
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
        default:
            break;
    }
    
}

void DPApp::updateHost(float timestep) {
    _hostScene.update(timestep);
    switch (_hostScene.state) {
        case HostScene::BACK:
            _status = MENU;
            _hostScene.setActive(false);
            _menuScene.setActive(true);
            break;
        case HostScene::HANDSHAKE:
            if (!_loaded){
                _gameScene.init(_assets, _network, true);
                _loaded = true;
            }
            
            _network->markReady();
            break;
        case HostScene::STARTGAME:
            _hostScene.setActive(false);
            _gameScene.setActive(true);
            _status = GAME;
            break;
        case HostScene::NETERROR:
            _hostScene.setActive(false);
            _menuScene.setActive(true);
            _gameScene.dispose();
            _status = MENU;
            break;
        case HostScene::INSCENE:
            //Do nothing
            break;
    }
}


void DPApp::updateClient(float timestep) {
    _clientScene.update(timestep);
    switch (_clientScene.state) {
        case ClientScene::BACK:
            _status = MENU;
            _clientScene.setActive(false);
            _menuScene.setActive(true);
            break;
        case ClientScene::HANDSHAKE:
            if(!_loaded){
                _gameScene.init(_assets, _network, false);
                _loaded = true;
            }
            _network->markReady();
            break;
        case ClientScene::STARTGAME:
            _clientScene.setActive(false);
            _gameScene.setActive(true);
            _status = GAME;
            break;
        case ClientScene::NETERROR:
            _network->disconnect();
            _clientScene.setActive(false);
            _menuScene.setActive(true);
            _gameScene.dispose();
            _status = MENU;
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
            _hostScene.init(_assets,_network);
            _clientScene.init(_assets,_network);
            _pauseScene.init(_assets, _network);
            _settingScene.init(_assets);
            //_pauseScene.init(_assets);
            //_gameScene.init(_assets);
            _status = MENU;
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
    // TODO: !
}


void DPApp::updateRestoration(float timestep) {
    _restorationScene.update(timestep);
    // TODO: !
}


#pragma mark DRAW
/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
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
        case HOST:
            _hostScene.render(_batch);
            break;
        case CLIENT:
            _clientScene.render(_batch);
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
    }
}
