//
//  GameSceneConstants.h
//  DustyPaints
//
//  Created by Haoxuan Zou on 3/12/24.
//

#ifndef GameSceneConstants_h
#define GameSceneConstants_h

/** THESE BELOW are useful for CHARACTER TUNING!*/
/** The default value of gravity - not used now. */
#define CHARACTER_GRAVITY -75.0f
/** This is not used for now. */
#define MAX_TORQUE 10000000.0f
/** Character's max force */
#define MAX_FORCE 30000.0f
/** The density for the character's body */
#define BODY_DENSITY 0.9f
#define BODY_ANGULAR_DAMPING 40.0f
/** The density for character's hands*/
#define HAND_DENSITY 1.0f
/** The density for other body parts. Right now we don't have other body parts (I think - from George). */
#define DEFAULT_DENSITY 1.0f
/** Friction for character's hands*/
#define HAND_FRICTION 150000000.0f
/** Friction for other body parts (right now it's just the big head I think - from George) */
#define OTHER_FRICTION 1.0f
/** input scaler - how effective can the finger input affect the character movement.
 * NOTE THAT this scaler will be multiplied by the height of screen size. So the scaler itself should be very small.
 */
#define INPUT_SCALER 0.0015f
/** HOW LONG the arms can stretch*/
#define MAX_ARM_LENGTH 7.2f
#define MIN_ARM_LENGTH 0.0f
/** THESE ABOVE are useful for CHARACTER TUNING!*/
#define HAND_K 2.0f
#define MIN_HAND_DISTANCE 2.5f
/** This is the size of the active portion of the screen */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH 32.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT 18.0f

#define DEFAULT_TURN_RATE 0.05f
#define FIXED_TIMESTEP_S 0.02f

#define DEFAULT_ZOOM 0.25
#define ZOOMIN_SPEED 0.01
#define NOZOOM 0
#define ZOOMIN 1
#define ZOOMOUT 2
#define INITIAL_STAY 30
#define FINAL_STAY 60
#define END_STAY 120

#define CJOINT_OFFSET 3.0f
#define HALF_CJOINT_OFFSET 1.5f

#endif /* GameSceneConstants_h */
