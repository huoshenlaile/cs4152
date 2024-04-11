#ifndef __LEVEL_LOAD_SCENE_H__
#define __LEVEL_LOAD_SCENE_H__

#include <cugl/cugl.h>

class LevelLoadScene : public cugl::Scene2 {
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;

public:
    void dispose() override;

    LevelLoadScene() : cugl::Scene2(), _assets(nullptr) {}

    ~LevelLoadScene() { dispose(); }

    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    bool loadFile(const std::string& file);

    enum LevelLoadState {
        LOADING,
        DONE
    };

    LevelLoadState _state;
    
};

#endif
