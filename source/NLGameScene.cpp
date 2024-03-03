//
//  NLGameScene.h
//  Networked Physics Demo
//
//  This is the most important class in this demo.  This class manages the
//  gameplay for this demo.  It also handles collision detection. There is not
//  much to do for collisions; our ObstacleWorld class takes care of all
//  of that for us.  This controller mainly transforms input into gameplay.
//
//  You will notice that we do not use a Scene asset this time.  While we could
//  have done this, we wanted to highlight the issues of connecting physics
//  objects to scene graph objects.  Hence we include all of the API calls.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design.  This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together.  However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/10/17
//
#include "NLGameScene.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include "PLPlatformConstants.h"
#include <ctime>
#include <string>
#include <iostream>
#include <sstream>

using namespace cugl;
using namespace cugl::physics2::net;

#pragma mark -
#pragma mark Level Geography

/** This is the size of the active portion of the screen */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f
/** The default value of gravity (going down) */
#define DEFAULT_GRAVITY -4.9f

#define DEFAULT_TURN_RATE 0.05f

#pragma mark Assset Constants
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE       "earth"
/** The key for the cannon texture in the asset manager */
#define CANNON_TEXTURE        "rocket"
/** The key for the win door texture in the asset manager */
#define GOAL_TEXTURE        "goal"
/** Color to outline the physics nodes */
#define STATIC_COLOR    Color4::WHITE
/** Opacity of the physics outlines */
#define DYNAMIC_COLOR   Color4::YELLOW
/** The key for collisions sounds */
#define COLLISION_SOUND     "bump"
/** The key for the main afterburner sound */
#define MAIN_FIRE_SOUND     "burn"
/** The key for the right afterburner sound */
#define RGHT_FIRE_SOUND     "right-burn"
/** The key for the left afterburner sound */
#define LEFT_FIRE_SOUND     "left-burn"
/** The key for the font reference */
#define PRIMARY_FONT        "retro"
#pragma mark Physics Constants
// Physics constants for initialization
/** Density of non-crate objects */
#define BASIC_DENSITY       0.0f
/** Friction of non-crate objects */
#define BASIC_FRICTION      0.1f
/** Collision restitution for all objects */
#define BASIC_RESTITUTION   0.1f
/** Threshold for generating sound on collision */
#define SOUND_THRESHOLD     3

#define FIXED_TIMESTEP_S 0.02f
float DOLL_POS[] = { 16, 10 };

/** This is the aspect ratio for physics */
#define SCENE_ASPECT 9.0/16.0

// Since these appear only once, we do not care about the magic numbers.
// In an actual game, this information would go in a data file.
// IMPORTANT: Note that Box2D units do not equal drawing units
/** The wall vertices */
#define WALL_VERTS 12
#define WALL_COUNT  2

float WALL[WALL_COUNT][WALL_VERTS] = {
	{16.0f, 18.0f,  0.0f, 18.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  1.0f, 17.0f, 16.0f, 17.0f },
	{32.0f, 18.0f, 16.0f, 18.0f, 16.0f, 17.0f,
	 31.0f, 17.0f, 31.0f,  0.0f, 32.0f,  0.0f }
};

/** The number of platforms */
#define PLATFORM_VERTS  8
#define PLATFORM_COUNT  4

/** The outlines of all of the platforms */
float PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
	{ 1.0f, 3.0f, 1.0f, 2.5f, 15.0f, 2.5f, 15.0f, 3.0f},
	{22.0f, 4.0f,22.0f, 2.5f,31.0f, 2.5f,31.0f, 4.0f},
	{18.0f,9.5f,18.0f,9.0f,24.0f,9.0f,24.0f,9.5f},
	{ 1.0f,12.5f, 1.0f,12.0f, 15.0f,12.0f, 15.0f,12.5f}
};

/** The goal door position */
float GOAL_POS[] = { 4.0f,14.0f };
/** The initial position of the dude */
float DUDE_POS[] = { 2.5f, 5.0f };


#pragma mark -
#pragma mark Physics Constants
/** The number of frame to wait before reinitializing the game */
#define EXIT_COUNT      240


#pragma mark -
#pragma mark Asset Constants
/** The name of a wall (for object identification) */
#define WALL_NAME       "wall"
/** The name of a platform (for object identification) */
#define PLATFORM_NAME   "platform"

/** The message to display on a level reset */
#define RESET_MESSAGE       "Resetting"
/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color4::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192

#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameScene::GameScene() : cugl::Scene2(),
_complete(false),
_debug(false),
_isHost(false)
{
}

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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<cugl::physics2::net::NetEventController> network, bool isHost) {
	return init(assets, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT), Vec2(0, DEFAULT_GRAVITY), network, isHost);
}

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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, const Rect rect, const std::shared_ptr<NetEventController> network, bool isHost) {
	return init(assets, rect, Vec2(0, DEFAULT_GRAVITY), network, isHost);
}

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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, const Rect rect, const Vec2 gravity, const std::shared_ptr<NetEventController> network, bool isHost) {
    Size dimen = computeActiveSize(); //compute the dimension of the screen
    
    if (assets == nullptr) { //check if assets exist
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    //make the getter for loading the map
    _level = assets->get<PlatformModel>(LEVEL_ONE_KEY);
    if (_level == nullptr) {
        CULog("Fail!");
        return false;
    }
    
    //set assets and the physics world
    _assets = assets;
    std::shared_ptr<cugl::physics2::net::NetWorld> platformWorld = _level->getWorld();
    activateWorldCollisions(platformWorld);
    
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / platformWorld->getBounds().getMaxX() :
            dimen.height / platformWorld->getBounds().getMaxY();
    
    Vec2 offset((dimen.width-SCENE_WIDTH)/2.0f,(dimen.height-SCENE_HEIGHT)/2.0f);
    
    _rootnode = scene2::SceneNode::alloc();
    _rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _rootnode->setPosition(offset);
    
    _winnode = scene2::Label::allocWithText("VICTORY!", _assets->get<Font>(PRIMARY_FONT));
    _winnode->setAnchor(Vec2::ANCHOR_CENTER);
    _winnode->setPosition(computeActiveSize() / 2);
    _winnode->setForeground(STATIC_COLOR);
    _winnode->setVisible(false);

    
    _loadnode = scene2::Label::allocWithText(RESET_MESSAGE, _assets->get<Font>(PRIMARY_FONT));
    _loadnode->setAnchor(Vec2::ANCHOR_CENTER);
    _loadnode->setPosition(computeActiveSize() / 2);
    _loadnode->setForeground(STATIC_COLOR);
    _loadnode->setVisible(false);

    _uinode = scene2::SceneNode::alloc();
    _uinode->setContentSize(dimen);
    _uinode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _uinode->addChild(_winnode);
    _uinode->addChild(_loadnode);
    
    addChild(_rootnode);
    addChild(_uinode);
    
    
    _rootnode->setContentSize(Size(SCENE_WIDTH,SCENE_HEIGHT));
    _level->setAssets(_assets);
    _level->setRootNode(_rootnode); // Obtains ownership of root.
    
    _camera.init(_rootnode, _rootnode, 1.0f, std::dynamic_pointer_cast<OrthographicCamera>(getCamera()), _uinode, 2.0f);

    _active = true;
    _complete = false;
	Application::get()->setClearColor(Color4f::CORNFLOWER);
	return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
	if (_active) {
		removeAllChildren();
		_input.dispose();
		
        _level = nullptr;
		_rootnode = nullptr;
		
		_winnode = nullptr;
		_complete = false;
		_debug = false;
		Scene2::dispose();
	}
}

#pragma mark -
#pragma mark Level Layout

std::shared_ptr<physics2::PolygonObstacle> wallobj1;
std::shared_ptr<physics2::PolygonObstacle> wallobj2;
std::shared_ptr<scene2::PolygonNode> wallsprite1;
std::shared_ptr<scene2::PolygonNode> wallsprite2;

std::vector<std::shared_ptr<physics2::BoxObstacle>> boxes;
std::vector<std::shared_ptr<scene2::PolygonNode>> nodes;

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void GameScene::reset() {
	setComplete(false);
	Application::get()->resetFixedRemainder();
}


/**
 * Lays out the game geography.
 *
 * Pay close attention to how we attach physics objects to a scene graph.
 * The simplest way is to make a subclass.  However,
 * for simple objects you can just use a callback function to lightly couple
 * them.  This is what we do with the crates.
 *
 * This method is really, really long.  In practice, you would replace this
 * with your serialization loader, which would process a level file.
 */
void GameScene::populate() {
    
}

void GameScene::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
const std::shared_ptr<cugl::scene2::SceneNode>& node,
                            bool useObjPosition){
	
}

void GameScene::linkSceneToObs(const std::shared_ptr<physics2::Obstacle>& obj,
	const std::shared_ptr<scene2::SceneNode>& node) {

}

/**
 * Adds the physics object to the physics world and loosely couples it to the scene graph
 *
 * There are two ways to link a physics object to a scene graph node on the
 * screen.  One way is to make a subclass of a physics object.
 * The other is to use callback functions to loosely couple
 * the two.  This function is an example of the latter.
 *
 * param obj    The physics object to add
 * param node   The scene graph node to attach it to
 */
void GameScene::addInitObstacle(const std::shared_ptr<physics2::Obstacle>& obj,
	const std::shared_ptr<scene2::SceneNode>& node) {

}


/**
 * Activates world collision callbacks on the given physics world and sets the onBeginContact and beforeSolve callbacks
 *
 * @param world the physics world to activate world collision callbacks on
 */
void GameScene::activateWorldCollisions(const std::shared_ptr<cugl::physics2::net::NetWorld>& world) {

}


#pragma mark -
#pragma mark Physics Handling

void GameScene::preUpdate(float dt) {
    if (_level == nullptr) {
        return;
    }

    // Check to see if new level loaded yet
    if (_loadnode->isVisible()) {
        if (_assets->complete()) {
            _level = nullptr;

            // Access and initialize level
            _level = _assets->get<PlatformModel>(LEVEL_ONE_KEY);
            _level->setAssets(_assets);
            _level->setRootNode(_rootnode); // Obtains ownership of root.
            _level->showDebug(_debug);

            activateWorldCollisions(_level->getWorld());

            _loadnode->setVisible(false);
        }
        else {
            // Level is not loaded yet; refuse input
            return;
        }
    }
    
    
//    _camera.addZoom(_input.getZoom() * 0.01);
    _camera.update(dt);
    _level->getWorld()->update(dt);
}

void GameScene::postUpdate(float dt) {

}

void GameScene::fixedUpdate() {

}

/**
 * Executes the core gameplay loop of this world.
 *
 * This method contains the specific update code for this mini-game. It does
 * not handle collisions, as those are managed by the parent class WorldController.
 * This method is called after input is read, but before collisions are resolved.
 * The very last thing that it should do is apply forces to the appropriate objects.
 *
 * @param  delta    Number of seconds since last animation frame
 */
void GameScene::update(float dt) {
	//deprecated
}

/**
 * Processes the start of a collision
 *
 * Since this game has no real need to determine collisions, right now this is left empty.
 *
 * @param  contact  The two bodies that collided
 */
void GameScene::beginContact(b2Contact* contact) {
}

/**
 * Handles any modifications necessary before collision resolution
 *
 * This method is called just before Box2D resolves a collision.  We use this method
 * to implement sound on contact, using the algorithms outlined in Ian Parberry's
 * "Introduction to Game Physics with Box2D".
 *
 * @param  contact      The two bodies that collided
 * @param  oldManfold      The collision manifold before contact
 */
void GameScene::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {
	
}

/**
 * Returns the active screen size of this scene.
 *
 * This method is for graceful handling of different aspect
 * ratios
 */
Size GameScene::computeActiveSize() const {
	Size dimen = Application::get()->getDisplaySize();
	float ratio1 = dimen.width / dimen.height;
	float ratio2 = ((float)SCENE_WIDTH) / ((float)SCENE_HEIGHT);
	if (ratio1 < ratio2) {
		dimen *= SCENE_WIDTH / dimen.width;
	}
	else {
		dimen *= SCENE_HEIGHT / dimen.height;
	}
	return dimen;
}
