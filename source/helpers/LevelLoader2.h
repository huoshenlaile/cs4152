#ifndef __LEVEL_LOADER_2_H__
#define __LEVEL_LOADER_2_H__

#include <stdio.h>
#include <math.h>
#include <cugl/assets/CUAsset.h>
#include <cugl/cugl.h>
#include <cugl/io/CUJsonReader.h>
#include "LevelConstants.h"
// include interactables
#include "../objects/interactable.h"
#include "../objects/objectImport.h"
#include "../controllers/CharacterController.h"
using namespace cugl;

class LevelLoader2 : public Asset {
protected:

    std::shared_ptr<cugl::JsonValue> _json;

    std::shared_ptr<scene2::SceneNode> _root;
    Rect _bounds;
    Vec2 _gravity;
    Vec2 _scale = Vec2(32, 32);
    Vec2 _charPos;

    std::shared_ptr<scene2::SceneNode> _worldnode;
    std::shared_ptr<scene2::SceneNode> _charNode; //character node
    std::shared_ptr<scene2::SceneNode> _platformNode; // platform node

    // game scene UI and complete should be managed by game scene it self, 
    // level loader only cares about what is inside the level
    std::shared_ptr<scene2::SceneNode> _uinode;
    std::shared_ptr<scene2::SceneNode> _levelCompletenode;
    std::shared_ptr<cugl::scene2::PolygonNode> _backgroundNode;

    std::shared_ptr<cugl::physics2::net::NetWorld> _world;
    std::shared_ptr<CharacterController> _character;

    std::string _background_name = ALPHA_RELEASE_BACKGROUND;
    // a vector of interactables shared pointer
    std::vector<std::shared_ptr<Interactable>> _interactables;

    cugl::Size computeActiveSize() const;
    bool loadObject(const std::shared_ptr<cugl::JsonValue>& json);
    Vec2 getObjectPos(const std::shared_ptr<JsonValue>& json);
public:
    // status of load
    enum LevelLoadState {
        LOADING,
        DONE
    };
    
    std::string goodBg;
    std::string badBg;
    std::string defaultBg;

    LevelLoadState _state;

    const Vec2 getCharacterPos() const {return _charPos;}
    const Rect& getBounds() const   { return _bounds; }
    const Vec2& getGravity() const { return _gravity; }

    // getters for GameScene and Interaction Controller
    std::shared_ptr<scene2::SceneNode> getWorldNode() { return _worldnode; }
    std::shared_ptr<cugl::physics2::net::NetWorld> getWorld() { return _world; }
    std::shared_ptr<CharacterController> getCharacter() { return _character; }
    std::vector<std::shared_ptr<Interactable>> getInteractables() { return _interactables; } // this will make a copy of the vector
    
    // overrided preload for Assets Manager
    void unload();
    virtual bool preload(const std::shared_ptr<cugl::JsonValue>& json) override;
    virtual bool preload(const std::string& file) {
        std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset(file);
        return preload(reader->readJson());
    }
    
    // construction of the level
    bool construct(std::shared_ptr<cugl::AssetManager>& _assets);


    // constructor and destructor
    LevelLoader2(void) : Asset(), _root(nullptr), _world(nullptr), _worldnode(nullptr), _scale(32, 32), _state(LOADING) {}
    virtual ~LevelLoader2(void) { unload(); }

    // static sallocator
    static std::shared_ptr<LevelLoader2> alloc() {
        std::shared_ptr<LevelLoader2> result = std::make_shared<LevelLoader2>();
        return (result->init("") ? result : nullptr);
    }

    static std::shared_ptr<LevelLoader2> alloc(std::string file) {
        std::shared_ptr<LevelLoader2> result = std::make_shared<LevelLoader2>();
        return (result->init(file) ? result : nullptr);
    }

};

#endif
