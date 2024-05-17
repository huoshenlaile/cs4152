#ifndef __LEVEL_LOAD_SCENE_H__
#define __LEVEL_LOAD_SCENE_H__

#include "../helpers/LevelLoader2.h"
#include <cugl/cugl.h>
class LevelLoadScene : public cugl::Scene2 {
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;
    std::string _file;
    std::string _key;
    std::string _levelAssets;
    std::string _levelAssets_currently_loaded;

    bool _is_currently_loaded = false;
    bool _constructWorld();

    bool loadAssets();
    bool loadLevelDescription();
    bool unloadAssets();
public:
    void dispose() override;

    LevelLoadScene() : cugl::Scene2(), _assets(nullptr) {}

    ~LevelLoadScene() { dispose(); }

    bool init(const std::shared_ptr<cugl::AssetManager> &assets);

    bool loadFileAsync(const std::string &file, const std::string &key, const std::string &levelAssets);

    void update(float timestep) override;

    enum LevelLoadSceneState { LOADING, CONSTRUCT, DONE };

    LevelLoadSceneState _state;
};

#endif
