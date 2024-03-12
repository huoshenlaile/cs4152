#ifndef __AUDIO_CONTROLLER_H__
#define __AUDIO_CONTROLLER_H__

#include <cugl/cugl.h>
using namespace cugl;

class AudioController {
    // TODO: How to do this?
    
    
public:
    bool play(const std::string key, const std::shared_ptr<Sound> &sound,
              bool loop, float volume, bool force);
};
#endif
