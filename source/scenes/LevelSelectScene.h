#ifndef LevelSelectScene_h
#define LevelSelectScene_h

#include <cugl/cugl.h>
#include <vector>

#define PING_TEST_COUNT 5

using namespace cugl::physics2::net;

class LevelSelectScene : public cugl::Scene2 {
  
protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;

    std::shared_ptr<cugl::scene2::Button> _level1;
    std::shared_ptr<cugl::scene2::Button> _level2;
    std::shared_ptr<cugl::scene2::Button> _level3;
    std::shared_ptr<cugl::scene2::Button> _level4;
    std::shared_ptr<cugl::scene2::Button> _level5;
    std::shared_ptr<cugl::scene2::Button> _level6;
    std::shared_ptr<cugl::scene2::Button> _level7;
    std::shared_ptr<cugl::scene2::Button> _level8;
    std::shared_ptr<cugl::scene2::Button> _level9;
    std::shared_ptr<cugl::scene2::Button> _level10;
    std::shared_ptr<cugl::scene2::Button> _level11;
    std::shared_ptr<cugl::scene2::Button> _level13;

    std::shared_ptr<cugl::scene2::Button> _backout;

/** The network configuration */
//    cugl::net::NetcodeConfig _config;
    
//    cugl::Timestamp _pingTimer;
//    Uint64 _totalPing;
//    int _sendCount;
//    int _receiveCount;

public:
    /** the state of this scene, referenced by DPApp*/
    enum SceneState {
        INSCENE,
        STARTGAME,
        BACK
    };
    SceneState state;
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new host scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    LevelSelectScene() : cugl::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~LevelSelectScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    

    // string of selected level name, initialized to "" (empty string)
    std::string selectedLevelFile = "json/alpharelease.json";
    std::string selectedLevelKey = "alpharelease";

    std::string getSelectedLevelFile() {
        return this->selectedLevelFile;
    }

    std::string getSelectedLevelKey() {
        return this->selectedLevelKey;
    }

    /**
     * Initializes the controller contents, and starts the game
     *
     * In previous labs, this method "started" the scene.  But in this
     * case, we only use to initialize the scene user interface.  We
     * do not activate the user interface yet, as an active user
     * interface will still receive input EVEN WHEN IT IS HIDDEN.
     *
     * That is why we have the method {@link #setActive}.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetEventController> network);
    
    /** NOW we are NOT doing multiplayer.  So we need this simpler init.*/
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    /**
     * Sets whether the scene is currently active
     *
     * This method should be used to toggle all the UI elements.  Buttons
     * should be activated when it is made active and deactivated when
     * it is not.
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;

    /**
     * The method called to update the scene.
     *
     * We need to update this method to constantly talk to the server
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;

private:
    /**
     * Updates the text in the given button.
     *
     * Techincally a button does not contain text. A button is simply a scene graph
     * node with one child for the up state and another for the down state. So to
     * change the text in one of our buttons, we have to descend the scene graph.
     * This method simplifies this process for you.
     *
     * @param button    The button to modify
     * @param text      The new text value
     */
    void updateText(const std::shared_ptr<cugl::scene2::Button>& button, const std::string text);
    
    /**
     * This method prompts the network controller to start the game.
     */
    void startGame();
    
    /**
     * This method changes the texture of a specific level button, together with activating/deactivating that button.
     */
    void lockOrUnlockLevelButton(int levelNumber, bool isLock);
    
    void lockAllLevels();
    void unlockAllLevels();
};

#endif /* LevelSelectScene_h */
