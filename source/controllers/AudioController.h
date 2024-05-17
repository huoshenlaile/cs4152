#ifndef __AUDIO_CONTROLLER_H__
#define __AUDIO_CONTROLLER_H__

#include <cugl/cugl.h>
using namespace cugl;

class AudioController {
    // TODO: How to do this?
protected:
    std::shared_ptr<cugl::AssetManager> _assets;
    
public:
    float music_volume = 1;
    float sound_volume = 1;
    
    AudioController();
    ~AudioController() {dispose();};
    
    void dispose();
    
    bool init(std::shared_ptr<cugl::AssetManager> _assets);

/**
 * Plays the given sound, and associates it with the specified key.
 *
 * Sounds are associated with a reference key. This allows the application
 * to easily reference the sound state without having to internally manage
 * pointers to the audio channel.
 *
 * If the key is already associated with an active sound effect, this
 * method will stop the existing sound and replace it with this one. It
 * is the responsibility of the application layer to manage key usage.
 *
 * There are a limited number of slots available for sounds. If you go
 * over the number available, the sound will not play unless `force` is
 * true. In that case, it will grab the channel from the longest playing
 * sound effect.
 *
 * @param  key      The reference key for the sound effect
 * @param  sound    The sound effect to play
 * @param  loop     Whether to loop the sound effect continuously
 * @param  volume   The music volume (relative to the default asset volume)
 * @param  force    Whether to force another sound to stop.
 *
 * @return true if there was an available channel for the sound
 */
    bool play(const std::string key, const std::string soundName,
              bool loop = false, float volume = -1.0f, bool force = false);
    
    bool playRandomNote();
    
    void pause(std::string key);
    
    void clear(std::string key);
    
    void setMusicVolume(float volume);
    
    void setSoundVolume(float volume);
};
#endif
