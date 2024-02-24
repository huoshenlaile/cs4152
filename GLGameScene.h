//
//  GLGameScene.h
//  Geometry Lab
//
//  This is the primary class file for running the game.  You should study this
//  file for ideas on how to structure your own root class. This class
//  illustrates how to use the geometry tools in CUGL, and how to combine
//  them with the physics engine.
//
//  Note that this time we do not have any additional model classes.
//
//  Author: Walker White
//  Version: 1/29/24
//
#ifndef __GL_GAME_SCENE_H__
#define __GL_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include "GLInputController.h"

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene2 {
protected:
	// CONTROLLERS are attached directly to the scene (no pointers)
	/** The controller to manage the ship */
	InputController _input;

	// MODELS should be shared pointers or a data structure of shared pointers
	/** The spline for the exterior circle */
	cugl::Spline2 _spline;
	/** The physics world to animate the falling */
	std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
	/** The outside "circle" */
	std::shared_ptr<cugl::physics2::PolygonObstacle> _center;
	/** The falling star */
	std::shared_ptr<cugl::physics2::PolygonObstacle> _star;

	// PUT OTHER ATTRIBUTES HERE AS NECESSARY
	cugl::Poly2 _poly;
	std::vector<cugl::Poly2> _handles;
	std::vector<cugl::Poly2> _knobs;
	cugl::Poly2 _starPoly;
	std::shared_ptr<cugl::physics2::PolygonObstacle> _starObs;
	std::shared_ptr<cugl::physics2::PolygonObstacle> _splineObs;

	bool _selected;
	int _selectedIdx;
	float _predAngular;
	cugl::Vec2 _predLinear;
	/**
	 * Rebuilds the geometry.
	 *
	 * This method should recreate all the polygons for the spline, the handles
	 * and the falling star. It should also recreate all physics objects.
	 *
	 * However, to cut down on performance overhead, this method should NOT add
	 * those physics objects to the world inside this method (as this method is
	 * called repeatedly while the user moves a handle). Instead, those objects
	 * should not be activated until the state is "stable".
	 */
	void buildGeometry();

public:
#pragma mark -
#pragma mark Constructors
	/**
	 * Creates a new game mode with the default values.
	 *
	 * This constructor does not allocate any objects or start the game.
	 * This allows us to use the object without a heap pointer.
	 */
	GameScene() : cugl::Scene2() {}

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
	void dispose() override;

	/**
	 * Initializes the controller contents, and starts the game
	 *
	 * The constructor does not allocate any objects or memory.  This allows
	 * us to have a non-pointer reference to this controller, reducing our
	 * memory allocation.  Instead, allocation happens in this method.
	 *
	 * @param assets    The (loaded) assets for this game mode
	 *
	 * @return true if the controller is initialized properly, false otherwise.
	 */
	bool init(const std::shared_ptr<cugl::AssetManager>& assets);

#pragma mark -
#pragma mark Gameplay Handling
	/**
	 * The method called to indicate the start of a deterministic loop.
	 *
	 * This method is used instead of {@link #update} if {@link #setDeterministic}
	 * is set to true. It marks the beginning of the core application loop,
	 * which is concluded with a call to {@link #postUpdate}.
	 *
	 * This method should be used to process any events that happen early in
	 * the application loop, such as user input or events created by the
	 * {@link schedule} method. In particular, no new user input will be
	 * recorded between the time this method is called and {@link #postUpdate}
	 * is invoked.
	 *
	 * Note that the time passed as a parameter is the time measured from the
	 * start of the previous frame to the start of the current frame. It is
	 * measured before any input or callbacks are processed. It agrees with
	 * the value sent to {@link #postUpdate} this animation frame.
	 *
	 * @param dt    The amount of time (in seconds) since the last frame
	 */
	void preUpdate(float dt);

	/**
	 * The method called to provide a deterministic application loop.
	 *
	 * This method provides an application loop that runs at a guaranteed fixed
	 * timestep. This method is (logically) invoked every {@link getFixedStep}
	 * microseconds. By that we mean if the method {@link draw} is called at
	 * time T, then this method is guaranteed to have been called exactly
	 * floor(T/s) times this session, where s is the fixed time step.
	 *
	 * This method is always invoked in-between a call to {@link #preUpdate}
	 * and {@link #postUpdate}. However, to guarantee determinism, it is
	 * possible that this method is called multiple times between those two
	 * calls. Depending on the value of {@link #getFixedStep}, it can also
	 * (periodically) be called zero times, particularly if {@link #getFPS}
	 * is much faster.
	 *
	 * As such, this method should only be used for portions of the application
	 * that must be deterministic, such as the physics simulation. It should
	 * not be used to process user input (as no user input is recorded between
	 * {@link #preUpdate} and {@link #postUpdate}) or to animate models.
	 *
	 * The time passed to this method is NOT the same as the one passed to
	 * {@link #preUpdate}. It will always be exactly the same value.
	 *
	 * @param step  The number of fixed seconds for this step
	 */
	void fixedUpdate(float step);

	/**
	 * The method called to indicate the end of a deterministic loop.
	 *
	 * This method is used instead of {@link #update} if {@link #setDeterministic}
	 * is set to true. It marks the end of the core application loop, which was
	 * begun with a call to {@link #preUpdate}.
	 *
	 * This method is the final portion of the update loop called before any
	 * drawing occurs. As such, it should be used to implement any final
	 * animation in response to the simulation provided by {@link #fixedUpdate}.
	 * In particular, it should be used to interpolate any visual differences
	 * between the the simulation timestep and the FPS.
	 *
	 * This method should not be used to process user input, as no new input
	 * will have been recorded since {@link #preUpdate} was called.
	 *
	 * Note that the time passed as a parameter is the time measured from the
	 * last call to {@link #fixedUpdate}. That is because this method is used
	 * to interpolate object position for animation.
	 *
	 * @param remain    The amount of time (in seconds) last fixedUpdate
	 */
	void postUpdate(float remain);

	/**
	 * Draws all this scene to the given SpriteBatch.
	 *
	 * The default implementation of this method simply draws the scene graph
	 * to the sprite batch.  By overriding it, you can do custom drawing
	 * in its place.
	 *
	 * @param batch     The SpriteBatch to draw with.
	 */
	void render(const std::shared_ptr<cugl::SpriteBatch>& batch) override;
};

#endif /* __SG_GAME_SCENE_H__ */
