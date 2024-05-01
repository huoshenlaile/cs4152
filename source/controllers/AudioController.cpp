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

void AudioController::pause(std::string key) {
    AudioEngine::get() -> pause(key);
}

void AudioController::clear(std::string key){
    AudioEngine::get() -> clear(key);
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

bool AudioController::playRandomNote() {
    int choice = std::rand();
    choice = choice % 6;
    std::string name = "touch1";
    switch (choice) {
        case 0:
            break;
        case 1:
            name = "touch2";
            break;
        case 2:
            name = "touch3";
            break;
        case 3:
            name = "touch4";
            break;
        case 4:
            name = "touch5";
            break;
        case 5:
            name = "touch6";
            break;
        default:
            break;
    }
    return this -> play(name, name);
}
