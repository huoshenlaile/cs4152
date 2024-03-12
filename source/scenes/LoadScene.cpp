#include "LoadScene.h"
using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

#pragma mark Constructors


bool LoadScene::init(const std::shared_ptr<AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    if (dimen.width > dimen.height) {
        dimen *= SCENE_SIZE/dimen.width;
    } else {
        dimen *= SCENE_SIZE/dimen.height;
    }
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    state = LOADING;
    
    // IMMEDIATELY load the splash screen assets
    _assets = assets;
    _assets->loadDirectory("json/loading.json");
    auto layer = assets->get<scene2::SceneNode>("load");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    
    _bar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("load_bar"));
    _brand = assets->get<scene2::SceneNode>("load_name");
    _button = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load_play"));
    _button->addListener([=](const std::string& name, bool down) {
        this->_active = down;
    });
    
    Application::get()->setClearColor(Color4(192,192,192,255));
    addChild(layer);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LoadScene::dispose() {
    // Deactivate the button (platform dependent)
    if (isPending()) {
        _button->deactivate();
    }
    _button = nullptr;
    _brand = nullptr;
    // TODO: I comment this out because otw app will crash upon shutdown.
    // _bar->removeFromParent();
    _bar = nullptr;
    _assets = nullptr;
    _progress = 0.0f;
}


#pragma mark -
#pragma mark Progress Monitoring

void LoadScene::update(float progress) {
    if (_progress < 1) {
        _progress = _assets->progress();
        if (_progress >= 1) {
            _progress = 1.0f;
            _bar->setVisible(false);
            _brand->setVisible(false);
            _button->setVisible(true);
            _button->activate();
        }
        _bar->setProgress(_progress);
    }
    if (this -> _active == false) {
        state = LOADED;
    }
}

/**
 * Returns true if loading is complete, but the player has not pressed play
 *
 * @return true if loading is complete, but the player has not pressed play
 */
bool LoadScene::isPending( ) const {
    return _button != nullptr && _button->isVisible();
}
