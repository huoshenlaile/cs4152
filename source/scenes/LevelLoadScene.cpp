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

    // copied from LoadScene.cpp
    _assets->loadDirectory("json/level_loading.json");
    layerA = assets->get<scene2::SceneNode>("levelloadA");
    layerA->setContentSize(dimen);
    layerA->doLayout();
    barA = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("levelloadA_bar"));
    
    layerB = assets->get<scene2::SceneNode>("levelloadB");
    layerB->setContentSize(dimen);
    layerB->doLayout();
    barB = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("levelloadB_bar"));

    layerC = assets->get<scene2::SceneNode>("levelloadC");
    layerC->setContentSize(dimen);
    layerC->doLayout();
    barC = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("levelloadC_bar"));

    layerD = assets->get<scene2::SceneNode>("levelloadD");
    layerD->setContentSize(dimen);
    layerD->doLayout();
    barD = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("levelloadD_bar"));

    layerE = assets->get<scene2::SceneNode>("levelloadE");
    layerE->setContentSize(dimen);
    layerE->doLayout();
    barE = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("levelloadE_bar"));
    

    Application::get()->setClearColor(Color4(255, 255, 255));

    return true;
}

// helper funcion to get level number from "level1" "level4" like strings
int getLevelNumber(const std::string& levelStr) {
    // Check if the string starts with "level"
    if (levelStr.compare(0, 5, "level") != 0) {
        return 1;
    }
    
    // Extract the substring after "level"
    std::string numberStr = levelStr.substr(5);

    // Convert the substring to an integer
    int number;
    std::stringstream ss(numberStr);
    if (!(ss >> number)) {
        return 1; // Return 0 if conversion fails
    }

    return number;
}

bool LevelLoadScene::loadFileAsync(const std::string &file, const std::string &key, const std::string &levelAssets) {
    // TODO:
    _state = LOADING;
    _file = file;
    _key = key;
    _levelAssets = levelAssets;
    CULog("Loading level description file: %s, Loading level assets: %s, Loading level key: %s, old addsets: %s", file.c_str(), levelAssets.c_str(), key.c_str(), _levelAssets_currently_loaded.c_str());

    // set cover based on level number
    int levelNumber = getLevelNumber(key);
    removeAllChildren();
    if (levelNumber == 1 || levelNumber == 2 || levelNumber == 3){
        addChild(layerA);
        _bar = barA;
    }else if (levelNumber == 4 || levelNumber == 5 || levelNumber == 6){
        addChild(layerB);
        _bar = barB;
    }else if (levelNumber == 7 || levelNumber == 8 || levelNumber == 9){
        addChild(layerC);
        _bar = barC;
    }else if (levelNumber == 10 || levelNumber == 11 || levelNumber == 12){
        addChild(layerD);
        _bar = barD;
    }else if (levelNumber == 13 || levelNumber == 14 || levelNumber == 15){
        addChild(layerE);
        _bar = barE;
    }else{
        CULog("Level number not found: %s", key.c_str());
        addChild(layerA);
        _bar = barA;
    }

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
    }else{
        auto progress = _assets->progress();
        _bar->setProgress(progress);
        CULog("get progress: %f",_bar->getProgress());
        CULog("Loading progress: %f", progress);
    }
}

void LevelLoadScene::dispose() {
    _assets = nullptr;
    _state = LOADING;
}
