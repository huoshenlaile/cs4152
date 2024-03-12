//
//  DPApp.h
//  Prototype1
//
//  Created by Xilai Dai on 3/8/24.
//

#ifndef __DP_APP_H__
#define __DP_APP_H__

#include <cugl/cugl.h>
#include "controllers/AudioController.h"
#include "controllers/CharacterController.h"
#include "controllers/InputController.h"
#include "controllers/InteractionController.h"
#include "controllers/NetworkController.h"

#include "scenes/ClientScene.h"
#include "scenes/HostScene.h"
#include "scenes/GameScene.h"
#include "scenes/LoadScene.h"
#include "scenes/MenuScene.h"
#include "scenes/SettingScene.h"
#include "scenes/RestorationScene.h"

#include "helpers/LevelLoader.h"
using namespace cugl::physics2::net;

/**
 * This class represents the application root for the ship demo.
 */
class DPApp : public cugl::Application {
    
enum GameStatus {
    LOAD,
    MENU,
    HOST,
    CLIENT,
    LEVEL,
    GAME,
    SETTING,
    RESTORE
};

protected:
    /** The global sprite batch for drawing (only want one of these) */
    std::shared_ptr<cugl::SpriteBatch> _batch;
    /** The global asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;

    std::shared_ptr<NetEventController> _network;
    
    std::shared_ptr<InputController> _input;
    
    HostScene       _hostScene;
    ClientScene     _clientScene;
    GameScene       _gameScene;
    LoadScene       _loadScene;
    SettingScene    _settingScene;
    MenuScene       _menuScene;
    
    GameStatus _status;
    bool loaded = false;
    
public:
#pragma mark Constructors

    DPApp() : cugl::Application() {}
    
    ~DPApp() { }
    
#pragma mark Application State

    virtual void onStartup() override;

    virtual void onShutdown() override;
    
    virtual void onSuspend() override;
    
    virtual void onResume()  override;
    
#pragma mark Application Loop
    
    virtual void preUpdate(float timestep) override;

    virtual void postUpdate(float timestep) override;

    virtual void fixedUpdate() override;
    
    virtual void update(float timestep) override;
    
    void updateMenu(float timestep);
    
    void updateHost(float timestep);
    
    void updateClient(float timestep);
    
//    void updateGame(float timestep);  // NOT required, as Game scene use deterministic update
    
    void updateLevel(float timestep);
    
    void updateSettings(float timestep);
    
    void updateRestoration(float timestep);
    
    virtual void draw() override;
};
#endif /* DPApp_h */
