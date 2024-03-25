//
//  GameSceneConstants.h
//  DustyPaints
//
//  Created by Haoxuan Zou on 3/12/24.
//

#ifndef GameSceneConstants_h
#define GameSceneConstants_h

/** THESE BELOW are useful for CHARACTER TUNING!*/
/** The default value of gravity (going down) */
#define CHARACTER_GRAVITY -75.0f
#define MAX_TORQUE 10000000.0f
#define MAX_FORCE  50000.0f
/** The density for each body part */
#define DEFAULT_DENSITY  1.0f
/** The density for the center of mass */
#define CENTROID_DENSITY 0.1f
/** The radius for the center of mass */
#define CENTROID_RADIUS  0.1f
#define BODY_DENSITY 0.8f
#define HAND_DENSITY 0.7f
#define HAND_FRICTION 1.0f
/** input scaler - how effective can the finger input affect the character movement*/
#define INPUT_SCALER 1.5f
/** THESE ABOVE are useful for CHARACTER TUNING!*/

/** This is the size of the active portion of the screen */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f

#define DEFAULT_TURN_RATE 0.05f
#define FIXED_TIMESTEP_S 0.02f

#define DEFAULT_ZOOM 0.25
#define ZOOMIN_SPEED 0.01
#define NOZOOM 0
#define ZOOMIN 1
#define ZOOMOUT 2

#define CJOINT_OFFSET 3.0f
#define HALF_CJOINT_OFFSET 1.5f
#define MAX_ARM_LENGTH 6.8f
#define MIN_ARM_LENGTH 0.0f

#endif /* GameSceneConstants_h */
