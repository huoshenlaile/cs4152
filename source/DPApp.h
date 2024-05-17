//
//  DPApp.h
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
#include "controllers/PauseEvent.h"


#include "scenes/GameScene.h"
#include "scenes/LevelSelectScene.h"
#include "scenes/LoadScene.h"
#include "scenes/MenuScene.h"
#include "scenes/PauseScene.h"
#include "scenes/RestorationScene.h"
#include "scenes/SettingScene.h"
#include "scenes/LevelLoadScene.h"

//#include "scenes/ClientScene.h"
//#include "scenes/HostScene.h"

#include "helpers/LevelConstants.h"
#include "helpers/LevelLoader2.h"
#include "helpers/GameSceneConstants.h"
#include "helpers/deviceInputScale.h"
using namespace cugl::physics2::net;

/**
 * This class represents the application root for the ship demo.
 */
class DPApp : public cugl::Application {
	enum GameStatus {
		LOAD,
		MENU,
		LEVELSELECT,
		LEVELLOAD,
		GAME,
		SETTING,
		RESTORE,
		PAUSE
	};

protected:
	/** The global sprite batch for drawing (only want one of these) */
	std::shared_ptr<cugl::SpriteBatch> _batch;
	/** The global asset manager */
	std::shared_ptr<cugl::AssetManager> _assets;

	std::shared_ptr<cugl::AssetManager> _assets2; // the assets dedicated for game scene, currently is still the global asset manager
	std::shared_ptr<NetEventController> _network;
    
    std::shared_ptr<JsonReader> _gameProgress;
    std::string _currentLevelKey;

	std::shared_ptr<InputController> _input;
    
    float _music_volume = 1;
    float _sound_volume = 1;
    bool hasPlayedEndingMusic = false;
    /**This boolean exists because CUGL has problems with onResume(). Right now, after tuning CUGL,
     onResume() works (more) correctly but it's called every time we FIRST open an app. That's counter-intuitive.
     To solve that issue, we check if we has suspended at least once before we call anything in onResume().*/
    bool hasSuspendedOnce = false;

	/** Whether or not we have finished loading all assets */
    bool _loaded = false;
    bool _reset = false;

//	HostScene _hostScene;
//	ClientScene _clientScene;
	GameScene _gameScene;
	PauseScene _pauseScene;
	LoadScene _loadScene;
	SettingScene _settingScene;
	MenuScene _menuScene;
	LevelSelectScene _levelSelectScene;
	RestorationScene _restorationScene;
	LevelLoadScene _levelLoadScene;
	GameStatus _status;
    AudioController _audioController;

public:
    bool honors_mode = true;
#pragma mark Constructors
    
	DPApp() : cugl::Application() {}

	~DPApp() {}

#pragma mark Application State

	virtual void onStartup() override;

	virtual void onShutdown() override;

	virtual void onSuspend() override;

	virtual void onResume() override;

#pragma mark Application Loop

	virtual void preUpdate(float timestep) override;

	virtual void postUpdate(float timestep) override;

	virtual void fixedUpdate() override;

	virtual void update(float timestep) override;

	void updateMenu(float timestep);

	void updateHost(float timestep);

	void updateClient(float timestep);

	void updatePause(float timestep);

	//    void updateGame(float timestep);  // NOT required, as Game scene uses
	//    deterministic update

	void updateSettings(float timestep);

	void updateRestoration(float timestep);

	void updateLoad(float timestep);

	void updateLevelSelect(float timestep);

	void updateLevelLoad(float timestep);

	void updateSetting(float timestep);

    /**Updates the level json after completing one level (adds current level and unlock the next level**/
    void updateLevelJson();
    
    /**Takes care of adding and updating one specific level's json fields**/
    void addLevelJson(std::shared_ptr<JsonValue> json, std::string levelkey, bool completedfield, std::string endingtypefield);
	virtual void draw() override;
    
    void playCurrentLevelMusic();
};
#endif /* DPApp_h */
