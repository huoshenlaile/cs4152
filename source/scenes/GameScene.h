#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include <cugl/cugl.h>
#include "../controllers/InputController.h"

class GameScene: public cugl::Scene2 {
protected:
    
    std::shared_ptr<cugl::AssetManager> _assets;
    
    std::shared_ptr<InputController> _input;
};


#endif
