#include "AudioController.h"


bool AudioController::play(const std::string key, const std::shared_ptr<Sound> &sound, bool loop = false, float volume = 1.0f, bool force = false) {
    if (!AudioEngine::get()->isActive(key)) {
        AudioEngine::get() -> play(key, sound, false, sound->getVolume());
        return true;
    }
    
    return false;
}
