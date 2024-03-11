//
//  DPApp.cpp
//  Prototype1
//
//  Created by Xilai Dai on 3/8/24.
//

#include <stdio.h>
#include "DPApp.h"

using namespace cugl;

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

    _loadScene.init(_assets);
    _status = LOAD;
    
    // Que up the other assets
    AudioEngine::start(24);
    _assets->loadDirectoryAsync("json/assets.json",nullptr);
    
    cugl::net::NetworkLayer::start(net::NetworkLayer::Log::INFO);
    
    Application::onStartup(); // YOU MUST END with call to parent
    setDeterministic(true);
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
#pragma mark Application Loop

void DPApp::preUpdate(float timestep){
    if (_status == LOAD && _loadScene.isActive()) {
        _loadScene.update(0.01f);
    }
    else if (_status == LOAD) {
        _network = NetEventController::alloc(_assets);
        _loadScene.dispose(); // Disables the input listeners in this mode
        _menuScene.init(_assets);
        _menuScene.setActive(true);
        _hostScene.init(_assets,_network);
        _clientScene.init(_assets,_network);
        //_gameScene.init(_assets);
        _status = MENU;
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
    else if (_status == GAME){
        if(_gameScene.isComplete()){
            _gameScene.reset();
            _status = MENU;
            _menuScene.setActive(true);
        }
        _gameScene.preUpdate(timestep);
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
    }
    if(_network){
        _network->updateNet();
    }
}

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


/**
 * Inidividualized update method for the menu scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the menu scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void DPApp::updateMenu(float timestep) {
    _menuScene.update(timestep);
    switch (_menuScene.getChoice()) {
        case MenuScene::Choice::HOST:
            _menuScene.setActive(false);
            _hostScene.setActive(true);
            _status = HOST;
            break;
        case MenuScene::Choice::JOIN:
            _menuScene.setActive(false);
            _clientScene.setActive(true);
            _status = CLIENT;
            break;
        case MenuScene::Choice::NONE:
            // DO NOTHING
            break;
    }
}

/**
 * Inidividualized update method for the host scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the host scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void DPApp::updateHost(float timestep) {
    _hostScene.update(timestep);
    if(_hostScene.getBackClicked()){
        _status = MENU;
        _hostScene.setActive(false);
        _menuScene.setActive(true);
    }
    else if (_network->getStatus() == NetEventController::Status::HANDSHAKE && _network->getShortUID()) {
        _gameScene.init(_assets, _network, true);
        _network->markReady();
    }
    else if (_network->getStatus() == NetEventController::Status::INGAME) {
        _hostScene.setActive(false);
        _gameScene.setActive(true);
        _status = GAME;
    }
    else if (_network->getStatus() == NetEventController::Status::NETERROR) {
        _network->disconnect();
        _hostScene.setActive(false);
        _menuScene.setActive(true);
        _gameScene.dispose();
        _status = MENU;
    }
}

/**
 * Inidividualized update method for the client scene.
 *
 * This method keeps the primary {\link #update} from being a mess of switch
 * statements. It also handles the transition logic from the client scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void DPApp::updateClient(float timestep) {
    //TODO: Write transition logic for client scene
#pragma mark SOLUTION
    _clientScene.update(timestep);
    if(_clientScene.getBackClicked()){
        _status = MENU;
        _clientScene.setActive(false);
        _menuScene.setActive(true);
    }
    else if (_network->getStatus() == NetEventController::Status::HANDSHAKE && _network->getShortUID()) {
        _gameScene.init(_assets, _network, false);
        _network->markReady();
    }
    else if (_network->getStatus() == NetEventController::Status::INGAME) {
        _clientScene.setActive(false);
        _gameScene.setActive(true);
        _status = GAME;
    }
    else if (_network->getStatus() == NetEventController::Status::NETERROR) {
        _network->disconnect();
        _clientScene.setActive(false);
        _menuScene.setActive(true);
        _gameScene.dispose();
        _status = MENU;
    }
#pragma mark END SOLUTION
}

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
        default:
            break;
    }
}
