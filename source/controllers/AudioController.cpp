#include "AudioController.h"


AudioController::AudioController() {
    
};


bool AudioController::init(std::shared_ptr<cugl::AssetManager> _assets) {
    this -> _assets = _assets;
    return true;
}


void AudioController::dispose() {
    this -> _assets = nullptr;
}


bool AudioController::play(const std::string key, const std::string soundName, bool loop, float volume, bool force) {
    auto sound = _assets -> get<Sound>(soundName);
    if (volume == -1.0f) {
        volume = sound -> getVolume();
    }
    if (!AudioEngine::get() -> isActive(key)) {
        AudioEngine::get() -> play(key, sound, loop, volume, force);
        return true;
    }
    return false;
}
