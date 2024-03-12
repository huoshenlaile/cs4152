#include "MenuScene.h"

using namespace cugl;
using namespace std;

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

#pragma mark Constructors

bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("menu");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    status = NONE;
    _hostbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu_host"));
    _joinbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu_join"));
    _settingbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu_setting"));
    
    _hostbutton->addListener([this](const std::string& name, bool down) {
        if (down) {
            status = HOST;
        }
    });
    _joinbutton->addListener([this](const std::string& name, bool down) {
        if (down) {
            status = JOIN;
        }
    });
    _settingbutton->addListener([this](const std::string& name, bool down) {
        if (down) {
            status = SETTING;
        }
    });

    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}


/**
 * Sets whether the scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void MenuScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            status = NONE;
            _hostbutton->activate();
            _joinbutton->activate();
            _settingbutton -> activate();
        } else {
            _hostbutton->deactivate();
            _joinbutton->deactivate();
            _settingbutton -> deactivate();
            // If any were pressed, reset them
            _hostbutton->setDown(false);
            _joinbutton->setDown(false);
            _settingbutton -> setDown(false);
        }
    }
}
