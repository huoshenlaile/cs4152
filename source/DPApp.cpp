//
//  DPApp.cpp
//  Prototype1
//
//  Created by Xilai Dai on 3/8/24.
//

#include <stdio.h>
#include "DPApp.h"

using namespace cugl;

void DPApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    
    // Start-up basic input
#ifdef CU_TOUCH_SCREEN
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
#endif
    
    Input::activate<Keyboard>();
    Input::activate<TextInput>();
    
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());

    _loading.init(_assets);
    _status = LOAD;
    
    // Que up the other assets
    AudioEngine::start(24);
    _assets->loadDirectoryAsync("json/assets.json",nullptr);
    
    cugl::net::NetworkLayer::start(net::NetworkLayer::Log::INFO);
    
    Application::onStartup(); // YOU MUST END with call to parent
    setDeterministic(true);
}
