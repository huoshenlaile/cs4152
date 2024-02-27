//
//  NLGameScene.h
//  Networked Crate Demo
//
//  This is a demo based on the original Rocket Demo, this demo is made in
//  addition to the Rocket Demo as a tutorial to the networked physics library.
//
//  The majority of this Demo is similar to the rocket demo, except for the
//  physics-related methods.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/10/17
//
#ifndef __NL_GAME_SCENE_H__
#define __NL_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <vector>
#include <format>
#include <string>
#include <random>
#include "NLInput.h"
#include "NLCrateEvent.h"

using namespace cugl::physics2::net;
using namespace cugl;

/**
 * The factory class for crate objects.
 *
 * This class is used to support automatically syncing newly added obstacle mid-simulation.
 * Obstacles added throught the ObstacleFactory class from one client will be added to all
 * clients in the simulations.
 */
class CrateFactory : public ObstacleFactory {
public:
    /** Pointer to the AssetManager for texture access, etc. */
    std::shared_ptr<cugl::AssetManager> _assets;
    /** Deterministic random generator for crate type */
    std::mt19937 _rand;
    /** Serializer for supporting parameters */
    LWSerializer _serializer;
    /** Deserializer for supporting parameters */
    LWDeserializer _deserializer;

    /**
     * Allocates a new instance of the factory using the given AssetManager.
     */
    static std::shared_ptr<CrateFactory> alloc(std::shared_ptr<AssetManager>& assets) {
        auto f = std::make_shared<CrateFactory>();
        f->init(assets);
        return f;
    };

    /**
     * Initializes empty factories using the given AssetManager.
     */
    void init(std::shared_ptr<AssetManager>& assets) {
        _assets = assets;
        _rand.seed(0xdeadbeef);
    }
    
    /**
     * Generate a pair of Obstacle and SceneNode using the given parameters
     */
    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(Vec2 pos, float scale);

    /**
     * Helper method for converting normal parameters into byte vectors used for syncing.
     */
    std::shared_ptr<std::vector<std::byte>> serializeParams(Vec2 pos, float scale);
    
    /**
     * Generate a pair of Obstacle and SceneNode using serialized parameters.
     */
    std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> createObstacle(const std::vector<std::byte>& params) override;
};

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene2 {
protected:

    std::shared_ptr<cugl::AssetManager> _assets;
    
    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    NetLabInput _input;
    
    // VIEW
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _worldnode;
    /** Reference to the debug root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _debugnode;
    /** Reference to the win message label */
    std::shared_ptr<cugl::scene2::Label> _winnode;
    
    std::shared_ptr<cugl::scene2::ProgressBar> _chargeBar;

    /** The Box2D world */
    std::shared_ptr<cugl::physics2::net::NetWorld> _world;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;

    std::mt19937 _rand;

    std::shared_ptr<CrateFactory> _crateFact;
    Uint32 _factId;

    // Physics objects for the game
    /** Reference to the player1 cannon */
    std::shared_ptr<cugl::scene2::SceneNode> _cannon1Node;
    std::shared_ptr<cugl::physics2::BoxObstacle> _cannon1;
    /** Reference to the player2 cannon */
    std::shared_ptr<cugl::scene2::SceneNode> _cannon2Node;
    std::shared_ptr<cugl::physics2::BoxObstacle> _cannon2;
    
    /** Host is by default the left cannon */
    bool _isHost;

    /** Whether we have completed this "game" */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;
    
    std::shared_ptr<NetEventController> _network;
    
#pragma mark Internal Object Management
    
    /**
     * This method adds a crate at the given position during the init process.
     */
    std::shared_ptr<cugl::physics2::Obstacle> addInitCrate(cugl::Vec2 pos);
    
    /**
     * Lays out the game geography.
     *
     * Pay close attention to how we attach physics objects to a scene graph.
     * The simplest way is to make a subclass. However,
     * for simple objects you can just use a callback function to lightly couple
     * them.  This is what we do with the crates.
     *
     * This method is really, really long.  In practice, you would replace this
     * with your serialization loader, which would process a level file.
     */
    void populate();
    
    /**
     * Adds the physics object to the physics world and loosely couples it to the scene graph
     *
     * There are two ways to link a physics object to a scene graph node on the
     * screen.  One way is to make a subclass of a physics object.
     * The other is to use callback functions to loosely couple
     * the two.  This function is an example of the latter.
     * the two.  This function is an example of the latter.
     *
     * param obj    The physics object to add
     * param node   The scene graph node to attach it to
     */
    void addInitObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj, 
                     const std::shared_ptr<cugl::scene2::SceneNode>& node);

    /**
     * This method links a scene node to the obstacle.
     *
     * This method adds a listener so that the sceneNode will move along with the obstacle.
     */
    void linkSceneToObs(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
        const std::shared_ptr<cugl::scene2::SceneNode>& node);
    
    /**
     * This method adds a crate that had been fired by the player's cannon amid the simulation.
     *
     * If this machine is host, the crate should be fire from the left cannon (_cannon1), vice versa.
     */
    void fireCrate();
    
    /**
     * This method takes a crateEvent and processes it.
     */
    void processCrateEvent(const std::shared_ptr<CrateEvent>& event);

    /**
     * Returns the active screen size of this scene.
     *
     * This method is for graceful handling of different aspect
     * ratios
     */
    cugl::Size computeActiveSize() const;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    GameScene();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  This initializer uses the default scale.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<NetEventController> network, bool isHost);

    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const cugl::Rect rect, const std::shared_ptr<NetEventController> network, bool isHost);
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     * @param gravity   The gravitational force on this Box2d world
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const cugl::Rect rect, const cugl::Vec2 gravity, const std::shared_ptr<NetEventController> network, bool isHost);
    
    
#pragma mark -
#pragma mark State Access
    /**
     * Returns true if the gameplay controller is currently active
     *
     * @return true if the gameplay controller is currently active
     */
    bool isActive( ) const { return _active; }

    /**
     * Returns true if debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @return true if debug mode is active.
     */
    bool isDebug( ) const { return _debug; }
    
    /**
     * Sets whether debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @param value whether debug mode is active.
     */
    void setDebug(bool value) { _debug = value; _debugnode->setVisible(value); }
    
    /**
     * Returns true if the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @return true if the level is completed.
     */
    bool isComplete( ) const { return _complete; }
    
    /**
     * Sets whether the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @param value whether the level is completed.
     */
    void setComplete(bool value) { _complete = value; _winnode->setVisible(value); }
    
    
#pragma mark -
#pragma mark Gameplay Handling

    virtual void preUpdate(float timestep);
    virtual void postUpdate(float timestep);
    virtual void fixedUpdate();
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);

    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();
    
#pragma mark -
#pragma mark Collision Handling
    /**
     * Processes the start of a collision
     *
     * This method is called when we first get a collision between two objects. 
     * We use this method to test if it is the "right" kind of collision.  In 
     * particular, we use it to test if we make it to the win door.
     *
     * @param  contact  The two bodies that collided
     */
    void beginContact(b2Contact* contact);

    /**
     * Handles any modifications necessary before collision resolution
     *
     * This method is called just before Box2D resolves a collision.  We use 
     * this method to implement sound on contact, using the algorithms outlined 
     * in Ian Parberry's "Introduction to Game Physics with Box2D".
     *
     * @param  contact  The two bodies that collided
     * @param  contact  The collision manifold before contact
     */
    void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);
};

#endif /* __NL_GAME_SCENE_H__ */
