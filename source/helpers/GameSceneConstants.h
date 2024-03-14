//
//  GameSceneConstants.h
//  DustyPaints
//
//  Created by Haoxuan Zou on 3/12/24.
//

#ifndef GameSceneConstants_h
#define GameSceneConstants_h

#include <cugl/cugl.h>
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include "../controllers/InputController.h"
#include "../controllers/InteractionController.h"
#include "../controllers/CharacterController.h"
#include "../controllers/AudioController.h"
#include "../models/PlatformModel.h"
#include "../models/ButtonModel.h"
#include "../models/WallModel.h"
#include "../helpers/LevelLoader.h"
#include "../helpers/LevelConstants.h"
#include "../controllers/CameraController.h"
#include "../controllers/PauseEvent.h"
#include "../DPInput.h"

/** This is the size of the active portion of the screen */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f
/** The default value of gravity (going down) */
#define DEFAULT_GRAVITY -9.8f

#define DEFAULT_TURN_RATE 0.05f
#define FIXED_TIMESTEP_S 0.02f

#endif /* GameSceneConstants_h */
