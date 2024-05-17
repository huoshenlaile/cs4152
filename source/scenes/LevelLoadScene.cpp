#include "LevelLoadScene.h"

#define SCENE_SIZE 1024

bool LevelLoadScene::init(const std::shared_ptr<cugl::AssetManager> &assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    if (dimen.width > dimen.height) {
        dimen *= SCENE_SIZE / dimen.width;
    } else {
        dimen *= SCENE_SIZE / dimen.height;
    }
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }

    if (assets == nullptr) {
        return false;
    }

    _state = LOADING;

    _assets = assets;

    // currently no progress bar or button simple black screen

    Application::get()->setClearColor(Color4(255, 255, 255));

    return true;
}

bool LevelLoadScene::loadFileAsync(const std::string &file, const std::string &key, const std::string &levelAssets) {
    // TODO:
    _state = LOADING;
    _file = file;
    _key = key;
    _levelAssets = levelAssets;
    CULog("Loading level description file: %s, Loading level assets: %s, Loading level key: %s, old addsets: %s", file.c_str(), levelAssets.c_str(), key.c_str(), _levelAssets_currently_loaded.c_str());
    loadAssets();
    
    return true;
}


bool LevelLoadScene::loadAssets() {
    if (_levelAssets_currently_loaded == _levelAssets) {
        loadLevelDescription();
    }else{
        unloadAssets();
        _assets->loadDirectoryAsync(_levelAssets, [this](const std::string key2, bool success2) {
            if (!success2) {
                CULog("Failed to load level assets");
                return;
            }
            CULog("Successfully loaded level assets: %s", key2.c_str());
            this->_levelAssets_currently_loaded = _levelAssets;
            this->_is_currently_loaded = true;
            this->loadLevelDescription();
        });
    }
    return true;
}

bool LevelLoadScene::unloadAssets() {
    if (_is_currently_loaded) {
        _assets->unloadDirectory(_levelAssets_currently_loaded);
        _is_currently_loaded = false;
        return true;
    }
    return false;
}

bool LevelLoadScene::loadLevelDescription() {
    _assets->loadAsync<LevelLoader2>(_key, _file, [this](const std::string key2, bool success2) {
        if (!success2) {
            CULog("Failed to load level description");
            return;
        }
        CULog("Successfully loaded level description: %s", key2.c_str());
        this->_state = CONSTRUCT;
    });
    return true;
}


void LevelLoadScene::update(float timestep) {
    if (_state == CONSTRUCT) {
//        CULog("constructing level1111");
        auto ll = _assets->get<LevelLoader2>(_key);
        ll->construct(_assets);
        _state = DONE;
    }
}

void LevelLoadScene::dispose() {
    _assets = nullptr;
    _state = LOADING;
}
