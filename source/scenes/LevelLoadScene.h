#ifndef __LEVEL_LOAD_SCENE_H__
#define __LEVEL_LOAD_SCENE_H__

#include <cugl/cugl.h>
#include "LevelLoader2.h"
class LevelLoadScene : public cugl::Scene2 {
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;

    bool _constructWorld();

public:
    void dispose() override;

    LevelLoadScene() : cugl::Scene2(), _assets(nullptr) {}

    ~LevelLoadScene() { dispose(); }

    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    bool loadFileAsync(const std::string& file, const std::string& key);

    void update(float timestep);

    enum LevelLoadSceneState {
        LOADING,
        DONE
    };

    LevelLoadSceneState _state;
    
};

#endif
