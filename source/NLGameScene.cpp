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

///** To automate the loading of crate files */
//#define NUM_CRATES 100


// Since these appear only once, we do not care about the magic numbers.
// In an actual game, this information would go in a data file.
// IMPORTANT: Note that Box2D units do not equal drawing units
/** The wall vertices */
float WALL1[] = { 0.0f,  0.0f, 16.0f,  0.0f, 16.0f,  1.0f,
                    3.0f,  1.0f,  3.0f,  5.0f,  2.0f,  7.0f,
                      1.0f, 17.0f,  8.0f, 15.0f, 16.0f, 17.0f,
                     16.0f, 18.0f,  0.0f, 18.0f };
float WALL2[] = { 32.0f, 18.0f, 16.0f, 18.0f, 16.0f, 17.0f,
                 31.0f, 16.0f, 30.0f, 10.0f, 31.0f,  1.0f,
                 16.0f,  1.0f, 16.0f,  0.0f, 32.0f,  0.0f };
float WALL3[] = { 4.0f,  9.5f,  8.0f,  9.5f,
                8.0f, 10.5f,  4.0f, 10.5f };

/** The positions of the crate pyramid */
float BOXES[] = { 14.5f, 14.25f,
                  13.0f, 12.00f, 16.0f, 12.00f,
                  11.5f,  9.75f, 14.5f,  9.75f, 17.5f, 9.75f,
                  13.0f,  7.50f, 16.0f,  7.50f,
                  11.5f,  5.25f, 14.5f,  5.25f, 17.5f, 5.25f,
                  10.0f,  3.00f, 13.0f,  3.00f, 16.0f, 3.00f, 19.0f, 3.0f};

/** The initial cannon position */
float CAN1_POS[] = { 2, 9 };
float CAN2_POS[] = { 30,9 };

#pragma mark Assset Constants
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE       "earth"
/** The key for the cannon texture in the asset manager */
#define CANNON_TEXTURE        "rocket"
/** The key for the win door texture in the asset manager */
#define GOAL_TEXTURE        "goal"
///** The key prefix for the multiple crate assets */
//#define CRATE_PREFIX        "crate"
///** The key for the fire textures in the asset manager */
//#define MAIN_FIRE_TEXTURE   "flames"
//#define RGHT_FIRE_TEXTURE   "flames-right"
//#define LEFT_FIRE_TEXTURE   "flames-left"

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
///** Friction of the crate objects */
//#define CRATE_FRICTION      0.2f
///** Angular damping of the crate objects */
//#define CRATE_DAMPING       1.0f
///** Density of the crate objects */
//#define CRATE_DENSITY       1.0f

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
///** The position of the rope bridge */
//float BRIDGE_POS[] = { 9.0f, 3.8f };
///** The position of the spinning barrier */
//float SPIN_POS[] = { 15.5f,12.0f };


#pragma mark -
#pragma mark Physics Constants
///** The density for a bullet */
//#define HEAVY_DENSITY   10.0f
///** The width of the rope bridge */
//#define BRIDGE_WIDTH    14.0f
///** Offset for bullet when firing */
//#define BULLET_OFFSET   0.5f
///** The speed of the bullet after firing */
//#define BULLET_SPEED   20.0f
/** The number of frame to wait before reinitializing the game */
#define EXIT_COUNT      240


#pragma mark -
#pragma mark Asset Constants
/** The name of a wall (for object identification) */
#define WALL_NAME       "wall"
/** The name of a platform (for object identification) */
#define PLATFORM_NAME   "platform"
///** The key for the win door texture in the asset manager */
//#define BULLET_TEXTURE  "bullet"
///** The name of a bullet (for object identification) */
//#define BULLET_NAME     "bullet"
///** The font for victory/failure messages */
//#define MESSAGE_FONT    "retro"
///** The message for winning the game */
//#define WIN_MESSAGE     "VICTORY!"
///** The color of the win message */
//#define WIN_COLOR       Color4::YELLOW
///** The message for losing the game */
//#define LOSE_MESSAGE    "FAILURE!"
///** The color of the lose message */
//#define LOSE_COLOR      Color4::RED
///** The key the basic game music */
//#define GAME_MUSIC      "game"
///** The key the victory game music */
//#define WIN_MUSIC       "win"
///** The key the failure game music */
//#define LOSE_MUSIC      "lose"
///** The sound effect for firing a bullet */
//#define PEW_EFFECT      "pew"
///** The sound effect for a bullet collision */
//#define POP_EFFECT      "pop"
///** The sound effect for jumping */
//#define JUMP_EFFECT     "jump"
///** The volume for the music */
//#define MUSIC_VOLUME    0.7f
///** The volume for sound effects */
//#define EFFECT_VOLUME   0.8f
///** The image for the left dpad/joystick */
//#define LEFT_IMAGE      "dpad_left"
///** The image for the right dpad/joystick */
//#define RIGHT_IMAGE     "dpad_right"

/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color4::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192

// /**
// * Generate a pair of Obstacle and SceneNode using the given parameters
// */
//std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> CrateFactory::createObstacle(Vec2 pos, float scale) {
//	int indx = (_rand() % 2 == 0 ? 2 : 1);
//	std::string name = (CRATE_PREFIX "0") + std::to_string(indx);
//	auto image = _assets->get<Texture>(name);
//	Size boxSize(image->getSize() / scale / 2.f);
//
//#pragma mark BEGIN SOLUTION
//	auto crate = physics2::BoxObstacle::alloc(pos, boxSize);
//
//	crate->setDebugColor(DYNAMIC_COLOR);
//	crate->setAngleSnap(0); // Snap to the nearest degree
//
//	crate->setDensity(CRATE_DENSITY);
//	crate->setFriction(CRATE_FRICTION);
//	crate->setAngularDamping(CRATE_DAMPING);
//	crate->setRestitution(BASIC_RESTITUTION);
//
//	crate->setShared(true);
//
//	auto sprite = scene2::PolygonNode::allocWithTexture(image);
//	sprite->setAnchor(Vec2::ANCHOR_CENTER);
//	sprite->setScale(0.5f);
//
//	return std::make_pair(crate, sprite);
//#pragma mark END SOLUTION
//}

// /**
// * Helper method for converting normal parameters into byte vectors used for syncing.
// */
//std::shared_ptr<std::vector<std::byte>> CrateFactory::serializeParams(Vec2 pos, float scale) {
//	// TODO: Use _serializer to serialize pos and scale (remember to make a shared copy of the serializer reference, otherwise it will be lost if the serializer is reset).
//#pragma mark BEGIN SOLUTION
//	_serializer.reset();
//	_serializer.writeFloat(pos.x);
//	_serializer.writeFloat(pos.y);
//	_serializer.writeFloat(scale);
//	return std::make_shared<std::vector<std::byte>>(_serializer.serialize());
//#pragma mark END SOLUTION
//}

// /**
// * Generate a pair of Obstacle and SceneNode using serialized parameters.
// */
//std::pair<std::shared_ptr<physics2::Obstacle>, std::shared_ptr<scene2::SceneNode>> CrateFactory::createObstacle(const std::vector<std::byte>& params) {
//	// TODO: Use _deserializer to deserialize byte vectors packed by {@link serializeParams()} and call the regular createObstacle() method with them.
//#pragma mark BEGIN SOLUTION
//	_deserializer.reset();
//	_deserializer.receive(params);
//	float x = _deserializer.readFloat();
//	float y = _deserializer.readFloat();
//	Vec2 pos = Vec2(x, y);
//	float scale = _deserializer.readFloat();
//	return createObstacle(pos, scale);
//#pragma mark END SOLUTION
//}

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
    Size dimen = computeActiveSize();

    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    _isHost = isHost;

    _network = network;
    
    // Start up the input handler
    _assets = assets;
    _input.init();
    _input.update();
    _rand.seed(0xdeadbeef);

//    _crateFact = CrateFactory::alloc(_assets);

    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    _scale = dimen.width == SCENE_WIDTH ? dimen.width/rect.size.width : dimen.height/rect.size.height;
    Vec2 offset((dimen.width-SCENE_WIDTH)/2.0f,(dimen.height-SCENE_HEIGHT)/2.0f);

    // Create the scene graph
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);
    
    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);
    
    _chargeBar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("load_bar"));
    _chargeBar->setPosition(Vec2(dimen.width/2.0f,dimen.height*0.9f));
    
    addChild(_worldnode);
    addChild(_debugnode);
    addChild(_chargeBar);
    
    _world = physics2::net::NetWorld::alloc(Rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT),Vec2(0,DEFAULT_GRAVITY));
    _world->onBeginContact = [this](b2Contact* contact) {
            beginContact(contact);
        };
    _world->update(FIXED_TIMESTEP_S);
    
    populate();
    _active = true;
    _complete = false;
    setDebug(false);

    //Make a std::function reference of the linkSceneToObs function in game scene for network controller
    std::function<void(const std::shared_ptr<physics2::Obstacle>&,const std::shared_ptr<scene2::SceneNode>&)> linkSceneToObsFunc = [=](const std::shared_ptr<physics2::Obstacle>& obs, const std::shared_ptr<scene2::SceneNode>& node) {
        this->linkSceneToObs(obs,node);
    };
    
    /**
     * TODO: Call network controller to enable physics with linkSceneToObsFunc as the method to attach nodes to obstacles and attach the crate factory to the physics controller and set _factId to its return value.
     *
     * TODO: Acquire the ownership of _cannon2 if this machine is not the host.
     */
#pragma mark BEGIN SOLUTION
	_network->enablePhysics(_world, linkSceneToObsFunc);

	if (!isHost) {
		_network->getPhysController()->acquireObs(_cannon2, 0);
	}

//	_factId = _network->getPhysController()->attachFactory(_crateFact);
#pragma mark END SOLUTION

	//TODO: For task 5, attach CrateEvent to the network controller
#pragma mark BEGIN SOLUTION
	_network->attachEventType<CrateEvent>();
#pragma mark END SOLUTION

	// XNA nostalgia
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
		_world = nullptr;
		_worldnode = nullptr;
		_debugnode = nullptr;
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
	_worldnode->removeAllChildren();
	_debugnode->removeAllChildren();
	setComplete(false);
	populate();
	Application::get()->resetFixedRemainder();
}

// /**
// * This method adds a crate at the given position during the init process.
// */
//std::shared_ptr<physics2::Obstacle> GameScene::addInitCrate(cugl::Vec2 pos) {
//    auto pair =  _crateFact->createObstacle(pos, _scale);
//    addInitObstacle(pair.first,pair.second);
//    return pair.first;
//}

// /**
// * This method adds a crate that had been fired by the player's cannon amid the simulation.
// *
// * If this machine is host, the crate should be fire from the left cannon (_cannon1), vice versa.
// */
//void GameScene::fireCrate() {
//    //TODO: Add a new crate to the simulation using the addSharedObstacle() method from the physics controller, and set its velocity in the direction the cannon is aimed scaled by (50 * _input.getFirePower()).
//    //HINT: You can use the serializedParams() method of the crate factory to help you serialize the parameters.
//#pragma mark BEGIN SOLUTION
//    auto cannon = _isHost ? _cannon1 : _cannon2;
//    auto params = _crateFact->serializeParams(cannon->getPosition(), _scale);
//    auto pair = _network->getPhysController()->addSharedObstacle(_factId, params);
//    float angle = cannon->getAngle() + M_PI_2;
//    Vec2 forward(SDL_cosf(angle), SDL_sinf(angle));
//    pair.first->setLinearVelocity(forward * 50 *1);
//#pragma mark END SOLUTION
//}


// /**
// * This method takes a crateEvent and processes it.
// */
//void GameScene::processCrateEvent(const std::shared_ptr<CrateEvent>& event){
//    //Choose randomly between wooden crates and iron crates.
//    int indx = (_rand() % 2 == 0 ? 2 : 1);
//    std::string name = (CRATE_PREFIX "0") + std::to_string(indx);
//    auto image = _assets->get<Texture>(name);
//    Size boxSize(image->getSize() / _scale);
//    
//    auto crate = physics2::BoxObstacle::alloc(Vec2(event->getPos().x,event->getPos().y), boxSize);
//    
//    crate->setDebugColor(DYNAMIC_COLOR);
//    crate->setAngleSnap(0); // Snap to the nearest degree
//    
//    // Set the physics attributes
//    crate->setDensity(CRATE_DENSITY);
//    crate->setFriction(CRATE_FRICTION);
//    crate->setAngularDamping(CRATE_DAMPING);
//    crate->setRestitution(BASIC_RESTITUTION);
//
//    crate->setShared(true);
//    
//    auto sprite = scene2::PolygonNode::allocWithTexture(image);
//    sprite->setAnchor(Vec2::ANCHOR_CENTER);
//    sprite->setScale(1.0f);
//    
//    //TODO: add the crate and sprite to the simulation
//    //NOTE: since both the host and client will receive a CrateEvent, we don't want to use addSharedObstacle() for it because it will create two separate crate. Instead you should use addInitObstacle(), which has the same top-bit id and if all clients called init obstacle the same amount of times, the same low-bit id. There is a potential race condition where multiple clients calling addInitObstacle() can cause id to be mixed up(clients send CrateEvent at the same time). In this lab, we will not address that race condition. But you could send along an obstacle id to ensure that all clients have that id for the obstacle.
//#pragma mark BEGIN SOLUTION
//    addInitObstacle(crate,sprite);
//#pragma mark END SOLUTION
//}

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
    _world = physics2::net::NetWorld::alloc(Rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT),Vec2(0,DEFAULT_GRAVITY));
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact* contact) {
        beginContact(contact);
    };
    _world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
        beforeSolve(contact,oldManifold);
    };
    CULog("Populating ragdoll");
#pragma mark : Ragdoll
    // Allocate the ragdoll and set its (empty) node. Its model handles creation of parts
    // (both obstacles and nodes to be drawn) upon alllocation and setting the scene node.
    _ragdoll = RagdollModel::alloc(DOLL_POS, _scale);
    _ragdoll->buildParts(_assets);
    
    
    _ragdoll->createJoints();
    
    auto ragdollNode = scene2::SceneNode::alloc();
    // Add the ragdollNode to the world before calling setSceneNode,
    // as noted in the documentation for the Ragdoll's method.
    _worldnode->addChild(ragdollNode);
    _ragdoll->setSceneNode(ragdollNode);
    
    _ragdoll->setDrawScale(_scale);
    _ragdoll->activate(_world);
    
    std::shared_ptr<Texture> image;
    std::shared_ptr<scene2::PolygonNode> sprite;
    std::shared_ptr<scene2::WireNode> draw;

  #pragma mark : Goal door
    image = _assets->get<Texture>(GOAL_TEXTURE);

    // Create obstacle
    Vec2 goalPos = GOAL_POS;
    Size goalSize(image->getSize().width / _scale,
      image->getSize().height / _scale);
    _goalDoor = physics2::BoxObstacle::alloc(goalPos, goalSize);

    // Set the physics attributes
    _goalDoor->setBodyType(b2_staticBody);
    _goalDoor->setDensity(0.0f);
    _goalDoor->setFriction(0.0f);
    _goalDoor->setRestitution(0.0f);
    _goalDoor->setSensor(true);

    // Add the scene graph nodes to this object
    sprite = scene2::PolygonNode::allocWithTexture(image);
    _goalDoor->setDebugColor(DEBUG_COLOR);
  addObstacle(_goalDoor, sprite);
  
#pragma mark : Wall polygon 1

	// Create ground pieces
	// All walls share the same texture
	image = _assets->get<Texture>(EARTH_TEXTURE);
	std::string wname = "wall";

	// Create the polygon outline
	Poly2 wall1(reinterpret_cast<Vec2*>(WALL1), 11);
	EarclipTriangulator triangulator;
	triangulator.set(wall1.vertices);
	triangulator.calculate();
	wall1.setIndices(triangulator.getTriangulation());
	triangulator.clear();

	//std::shared_ptr<physics2::PolygonObstacle> wallobj;
	wallobj1 = physics2::PolygonObstacle::allocWithAnchor(wall1, Vec2::ANCHOR_CENTER);
	wallobj1->setDebugColor(STATIC_COLOR);
	wallobj1->setName(wname);

	// Set the physics attributes
	wallobj1->setBodyType(b2_staticBody);
	wallobj1->setDensity(BASIC_DENSITY);
	wallobj1->setFriction(BASIC_FRICTION);
	wallobj1->setRestitution(BASIC_RESTITUTION);

	// Add the scene graph nodes to this object
	wall1 *= _scale;
	wallsprite1 = scene2::PolygonNode::allocWithTexture(image, wall1);

#pragma mark : Wall polygon 2
    Poly2 wall2(reinterpret_cast<Vec2*>(WALL2),9);
    triangulator.set(wall2.vertices);
    triangulator.calculate();
    wall2.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    wallobj2 = physics2::PolygonObstacle::allocWithAnchor(wall2,Vec2::ANCHOR_CENTER);
    wallobj2->setDebugColor(STATIC_COLOR);
    wallobj2->setName(wname);

    // Set the physics attributes
    wallobj2->setBodyType(b2_staticBody);
    wallobj2->setDensity(BASIC_DENSITY);
    wallobj2->setFriction(BASIC_FRICTION);
    wallobj2->setRestitution(BASIC_RESTITUTION);

    // Add the scene graph nodes to this object
    wall2 *= _scale;
    wallsprite2 = scene2::PolygonNode::allocWithTexture(image,wall2);
        
#pragma mark : Crates
    /*
    float f1 = _rand() % (int)(DEFAULT_WIDTH - 4) + 2;
    float f2 = _rand() % (int)(DEFAULT_HEIGHT - 4) + 2;
    Vec2 boxPos(f1, f2);
        
    for (int ii = 0; ii < NUM_CRATES; ii++) {
        f1 = _rand() % (int)(DEFAULT_WIDTH - 6) + 3;
        f2 = _rand() % (int)(DEFAULT_HEIGHT - 6) + 3;
        // Pick a crate and random and generate the key
        Vec2 boxPos(f1, f2);
        addInitCrate(boxPos);
    }
     */
        
#pragma mark : Cannon
//    image  = _assets->get<Texture>(CANNON_TEXTURE);
//    _cannon1Node = scene2::PolygonNode::allocWithTexture(image);
//    Size canSize(image->getSize()/_scale);
//        
//    Vec2 canPos1 = ((Vec2)CAN1_POS);
//    _cannon1 = cugl::physics2::BoxObstacle::alloc(canPos1,canSize);
//    //_cannon1->setBodyType(b2BodyType::b2_kinematicBody);
//    _cannon1->setAngle(-M_PI_2);
//    _cannon1->setDebugColor(DYNAMIC_COLOR);
//    _cannon1->setSensor(true);
//        
//    image  = _assets->get<Texture>(CANNON_TEXTURE);
//    _cannon2Node = scene2::PolygonNode::allocWithTexture(image);
//    
//    Vec2 canPos2 = ((Vec2)CAN2_POS);
//    _cannon2= cugl::physics2::BoxObstacle::alloc(canPos2,canSize);
//    //_cannon2->setBodyType(b2BodyType::b2_kinematicBody);
//    _cannon2->setAngle(M_PI_2);
//    _cannon2->setDebugColor(DYNAMIC_COLOR);
//    _cannon2->setSensor(true);
    
//    addInitObstacle(wallobj1, wallsprite1);  // All walls share the same texture
//    addInitObstacle(wallobj2, wallsprite2);  // All walls share the same texture
//    addInitObstacle(_cannon1, _cannon1Node);
//    addInitObstacle(_cannon2, _cannon2Node);

#pragma mark : Walls
	// All walls and platforms share the same texture
	image = _assets->get<Texture>(EARTH_TEXTURE);
	//std::string wname = "wall";
	for (int ii = 0; ii < WALL_COUNT; ii++) {
		std::shared_ptr<physics2::PolygonObstacle> wallobj;

		Poly2 wall(reinterpret_cast<Vec2*>(WALL[ii]), WALL_VERTS / 2);
		// Call this on a polygon to get a solid shape
		EarclipTriangulator triangulator;
		triangulator.set(wall.vertices);
		triangulator.calculate();
		wall.setIndices(triangulator.getTriangulation());
		triangulator.clear();

		wallobj = physics2::PolygonObstacle::allocWithAnchor(wall, Vec2::ANCHOR_CENTER);
		// You cannot add constant "".  Must stringify
		wallobj->setName(std::string(WALL_NAME) + cugl::strtool::to_string(ii));
		wallobj->setName(wname);

		// Set the physics attributes
		wallobj->setBodyType(b2_staticBody);
		wallobj->setDensity(BASIC_DENSITY);
		wallobj->setFriction(BASIC_FRICTION);
		wallobj->setRestitution(BASIC_RESTITUTION);
		wallobj->setDebugColor(DEBUG_COLOR);

		wall *= _scale;
		sprite = scene2::PolygonNode::allocWithTexture(image, wall);
		addObstacle(wallobj, sprite, 1);  // All walls share the same texture
	}

#pragma mark : Platforms
	for (int ii = 0; ii < PLATFORM_COUNT; ii++) {
		std::shared_ptr<physics2::PolygonObstacle> platobj;
		Poly2 platform(reinterpret_cast<Vec2*>(PLATFORMS[ii]), 4);

		EarclipTriangulator triangulator;
		triangulator.set(platform.vertices);
		triangulator.calculate();
		platform.setIndices(triangulator.getTriangulation());
		triangulator.clear();

		platobj = physics2::PolygonObstacle::allocWithAnchor(platform, Vec2::ANCHOR_CENTER);
		// You cannot add constant "".  Must stringify
		platobj->setName(std::string(PLATFORM_NAME) + cugl::strtool::to_string(ii));

		// Set the physics attributes
		platobj->setBodyType(b2_staticBody);
		platobj->setDensity(BASIC_DENSITY);
		platobj->setFriction(BASIC_FRICTION);
		platobj->setRestitution(BASIC_RESTITUTION);
		platobj->setDebugColor(DEBUG_COLOR);

		platform *= _scale;
		sprite = scene2::PolygonNode::allocWithTexture(image, platform);
		addObstacle(platobj, sprite, 1);
	}
}

void GameScene::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
	const std::shared_ptr<cugl::scene2::SceneNode>& node,
	bool useObjPosition) {
	_world->addObstacle(obj);
	obj->setDebugScene(_debugnode);

	// Position the scene graph node (enough for static objects)
	if (useObjPosition) {
		node->setPosition(obj->getPosition() * _scale);
	}
	_worldnode->addChild(node);

	// Dynamic objects need constant updating
	if (obj->getBodyType() == b2_dynamicBody) {
		scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
		obj->setListener([=](physics2::Obstacle* obs) {
			weak->setPosition(obs->getPosition() * _scale);
			weak->setAngle(obs->getAngle());
			});
	}
}

void GameScene::linkSceneToObs(const std::shared_ptr<physics2::Obstacle>& obj,
	const std::shared_ptr<scene2::SceneNode>& node) {
	node->setPosition(obj->getPosition() * _scale);
	_worldnode->addChild(node);

	// Dynamic objects need constant updating
	if (obj->getBodyType() == b2_dynamicBody) {
		scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
		obj->setListener([=](physics2::Obstacle* obs) {
			float leftover = Application::get()->getFixedRemainder() / 1000000.f;
			Vec2 pos = obs->getPosition() + leftover * obs->getLinearVelocity();
			float angle = obs->getAngle() + leftover * obs->getAngularVelocity();
			weak->setPosition(pos * _scale);
			weak->setAngle(angle);
			});
	}
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
	_world->initObstacle(obj);
	if (_isHost) {
		_world->getOwnedObstacles().insert({ obj,0 });
	}
	linkSceneToObs(obj, node);
}

#pragma mark -
#pragma mark Physics Handling

void GameScene::preUpdate(float dt) {
    _input.update();
    int knob_radius_multiplier = 1;
    std::shared_ptr<cugl::physics2::Obstacle> leftArm = _ragdoll->getPartObstacle(PART_LEFT_HAND);
    std::shared_ptr<cugl::physics2::Obstacle> rightArm = _ragdoll->getPartObstacle(PART_RIGHT_HAND);
    std::unordered_map<std::string,cugl::Vec2> inputs = _input.getPosition();
    for (const auto & [ key, value ] : inputs) {
        if (_input_to_arm.count(key)==0){ // Add touchpoint
            cugl::Vec2 pos2d = ((cugl::Vec2)screenToWorldCoords(value));
            if(_arm_to_input.count(PART_LEFT_HAND)==0 && (leftArm->getPosition()*_scale).distance(pos2d)<50*knob_radius_multiplier){
                _input_to_arm[key]=PART_LEFT_HAND;
                _arm_to_input[PART_LEFT_HAND]=key;
                
            }
            else if(_arm_to_input.count(PART_RIGHT_HAND)==0 && (rightArm->getPosition()*_scale).distance(pos2d)<50*knob_radius_multiplier){
                _input_to_arm[key]=PART_RIGHT_HAND;
                _arm_to_input[PART_RIGHT_HAND]=key;
            }
        }
        else{ // Do stuff with existing touchpoint
            cugl::Vec2 pos_now =  ((cugl::Vec2)screenToWorldCoords(value));
            std::shared_ptr<cugl::physics2::Obstacle> arm = _ragdoll->getPartObstacle(_input_to_arm[key]);
            arm->setPosition(pos_now/_scale);
        }
    }
    auto it = _arm_to_input.begin();
    while (it != _arm_to_input.end()){ // Remove old touchpoints
        auto id = it->second;
        if (inputs.count(id)==0){
            it = _arm_to_input.erase(it);
            _input_to_arm.erase(id);
        }
        else{
            ++it;
        }
    }
    
   // if(_input.getFirePower()>0.f){
  //      _chargeBar->setVisible(true);
  //      _chargeBar->setProgress(_input.getFirePower());
  //  }
  //  else{
  //      _chargeBar->setVisible(false);
  //  }

    // Process the toggled key commands
    //if (_input.didDebug()) { setDebug(!isDebug()); }

   // if (_input.didExit()) {
   //     CULog("Shutting down");
   //     Application::get()->quit();
  //  }
    
   // if (_input.didFire()) {
    //    fireCrate();
    //}
    
//TODO: if _input.didBigCrate(), allocate a crate event for the center of the screen(use DEFAULT_WIDTH/2 and DEFAULT_HEIGHT/2) and send it using the pushOutEvent() method in the network controller.
#pragma mark BEGIN SOLUTION
   // if (_input.didBigCrate()){
   //     CULog("BIG CRATE COMING");
   //     _network->pushOutEvent(CrateEvent::allocCrateEvent(Vec2(DEFAULT_WIDTH/2,DEFAULT_HEIGHT/2)));
  //  }
#pragma mark END SOLUTION
    
  //  float turnRate = _isHost ? DEFAULT_TURN_RATE : -DEFAULT_TURN_RATE;
  //  auto cannon = _isHost ? _cannon1 : _cannon2;
  //  cannon->setAngle(_input.getVertical() * turnRate + cannon->getAngle());
}

void GameScene::postUpdate(float dt) {
    //Nothing to do now
    _ragdoll->update(dt);
}

void GameScene::fixedUpdate() {
	//TODO: check for available incoming events from the network controller and call processCrateEvent if it is a CrateEvent.

	//Hint: You can check if ptr points to an object of class A using std::dynamic_pointer_cast<A>(ptr). You should always check isInAvailable() before popInEvent().

#pragma mark BEGIN SOLUTION
//	if (_network->isInAvailable()) {
//		auto e = _network->popInEvent();
//		if (auto crateEvent = std::dynamic_pointer_cast<CrateEvent>(e)) {
//			CULog("BIG CRATE GOT");
//			//processCrateEvent(crateEvent);
//		}
//	}
#pragma mark END SOLUTION
	_world->update(FIXED_TIMESTEP_S);
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
	float speed = 0;

	// Use Ian Parberry's method to compute a speed threshold
	b2Body* body1 = contact->GetFixtureA()->GetBody();
	b2Body* body2 = contact->GetFixtureB()->GetBody();
	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);
	b2PointState state1[2], state2[2];
	b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());
	for (int ii = 0; ii < 2; ii++) {
		if (state2[ii] == b2_addState) {
			b2Vec2 wp = worldManifold.points[0];
			b2Vec2 v1 = body1->GetLinearVelocityFromWorldPoint(wp);
			b2Vec2 v2 = body2->GetLinearVelocityFromWorldPoint(wp);
			b2Vec2 dv = v1 - v2;
			speed = b2Dot(dv, worldManifold.normal);
		}
	}

	// Play a sound if above threshold
	if (speed > SOUND_THRESHOLD) {
		// These keys result in a low number of sounds.  Too many == distortion.
		physics2::Obstacle* data1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
		physics2::Obstacle* data2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);

		if (data1 != nullptr && data2 != nullptr) {
			std::string key = (data1->getName() + data2->getName());
			auto source = _assets->get<Sound>(COLLISION_SOUND);
			if (!AudioEngine::get()->isActive(key)) {
				AudioEngine::get()->play(key, source, false, source->getVolume());
			}
		}
	}
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
