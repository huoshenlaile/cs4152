#ifndef __AUDIO_CONTROLLER_H__
#define __AUDIO_CONTROLLER_H__

#include <cugl/cugl.h>
using namespace cugl;

class AudioController {
    // TODO: How to do this?
protected:
    std::shared_ptr<cugl::AssetManager> _assets;
    
public:
    
    AudioController();
    ~AudioController() {dispose();};
    
    void dispose();
    
    bool init(std::shared_ptr<cugl::AssetManager> _assets);
    
    bool play(const std::string key, const std::string soundName,
              bool loop = false, float volume = -1.0f, bool force = false);
};
#endif
