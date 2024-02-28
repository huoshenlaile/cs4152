//
//  GLGameScene.cpp
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
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "GLGameScene.h"

using namespace cugl;
using namespace std;

#pragma mark -
#pragma mark Level Layout

// Lock the screen size to fixed height regardless of aspect ratio
#define SCENE_HEIGHT 720

/** How big the handle should be */
#define KNOB_RADIUS 15

/** The extrustion width. */
#define LINE_WIDTH 50

/** The width of a handle. */
#define HANDLE_WIDTH 3

/** The ratio between the physics world and the screen. */
#define PHYSICS_SCALE 50

/** The initial control points for the spline. */
float CIRCLE[] = { 0,  200,  120,  200,
		200,  120,  200,    0,  200, -120,
		120, -200,    0, -200, -120, -200,
	   -200, -120, -200,    0, -200,  120,
	   -120,  200,    0,  200 };

float REC[] = { -400,0,-200,0,0,0,200,0,400,0,400,-200, 400,-400 };

/** The (CLOCKWISE) polygon for the star */
float STAR[] = { 0,    50,  10.75,    17,   47,     17,
				 17.88, -4.88,   29.5, -40.5,    0, -18.33,
				 -29.5, -40.5, -17.88, -4.88,  -47,     17,
				-10.75,    17 };

#pragma mark -
#pragma mark Constructors
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
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
	// Initialize the scene to a locked width
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_HEIGHT / dimen.height;
	if (assets == nullptr) {
		return false;
	}
	else if (!Scene2::init(dimen)) {
		return false;
	}

	// Start up the input handler
	_input.init();

	// ADD CODE HERE
	_world = physics2::ObstacleWorld::alloc(Rect(Vec2(0, 0), getSize() / PHYSICS_SCALE), Vec2(0, -9.8));

	_selected = false;
	_selectedIdx = -1;
	Vec2* circleVec = reinterpret_cast<Vec2*>(CIRCLE);
	_spline.set(circleVec, 13);
	bool flag = true;
	_spline.setClosed(flag);

	Vec2* recVec = reinterpret_cast<Vec2*>(REC);
	_rect.set(recVec, 7);
	flag = false;
	_rect.setClosed(flag);

	buildGeometry();
	return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
	// NOTHING TO DO THIS TIME
}

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
void GameScene::preUpdate(float dt) {
	// We always need to call this to update the state of input variables
	// This SYNCHRONIZES the call back functions with the animation frame.
	_input.update();

	// ADD CODE HERE
	Vec2 mousePos;
	if (_input.didPress()) {
		mousePos = _input.getPosition();
		Vec3 worldCoords = Scene2::screenToWorldCoords(_input.getPosition());
		mousePos = (Vec2)worldCoords - getSize() / 2;
	}

	if (_input.didRelease()) {
		_selected = false;
		_selectedIdx = -1;
		return;
	}

	if (_selectedIdx != -1) {
		Vec2 temp1 = (Vec2)Scene2::screenToWorldCoords(_input.getPosition()) - getSize() / 2;
		Vec2 temp2 = (Vec2)Scene2::screenToWorldCoords(_input.getPrevious()) - getSize() / 2;
		Vec2 movement = temp1 - temp2;
		Vec2 newControlPoint = _spline.getTangent(_selectedIdx) + movement;
		_spline.setTangent(_selectedIdx, newControlPoint, true);
		_handles.clear();
		_knobs.clear();
		_poly.clear();
		_rectPoly.clear();
		_starPoly.clear();
		buildGeometry();
		return;
	}
	else {
		// Decide whether the knob is pressed.
		for (int pos = 0; pos <= 7; pos++) {
			Vec2 dist = mousePos - _spline.getTangent(pos);
			if (dist.length() <= KNOB_RADIUS) {
				_selected = true;
				_selectedIdx = pos;
				break;
			}
		}
	}
}

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
void GameScene::fixedUpdate(float step) {
	// ADD CODE HERE
	if (_selectedIdx == -1) _world->update(step);
}

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
void GameScene::postUpdate(float remain) {
	// ADD CODE HERE
	_predAngular = _starObs->getAngularVelocity() * remain;
	_predLinear = _starObs->getLinearVelocity() * remain;
}

/**
 * Draws all this scene to the given SpriteBatch.
 *
 * The default implementation of this method simply draws the scene graph
 * to the sprite batch.  By overriding it, you can do custom drawing
 * in its place.
 *
 * @param batch     The SpriteBatch to draw with.
 */
void GameScene::render(const std::shared_ptr<cugl::SpriteBatch>& batch) {
	// DO NOT DO THIS IN YOUR FINAL GAME
	batch->begin(getCamera()->getCombined());

	// ADD CODE HERE
	batch->setColor(Color4::BLACK);
	Affine2 splineTrans;
	splineTrans = splineTrans.rotate(_splineObs->getAngle());
	splineTrans = splineTrans.translate(_splineObs->getPosition() * PHYSICS_SCALE);
	//batch->fill(_poly, Vec2(0, 0), splineTrans);

	//Affine2 rectTrans;
	//rectTrans = rectTrans.rotate(_rectObs->getAngle());
	//rectTrans = rectTrans.translate(_rectObs->getPosition() * PHYSICS_SCALE);
	batch->setColor(Color4::BLACK);
	for (auto it : _rectPoly) batch->fill(it, getSize() / 2);

	/*batch->setColor(Color4::WHITE);
	for (auto it : _handles) batch->fill(it, getSize() / 2);

	batch->setColor(Color4::RED);
	for (auto it : _knobs) batch->fill(it, getSize() / 2);*/

	batch->setColor(Color4::BLUE);
	Affine2 starTrans;
	starTrans = starTrans.rotate(_starObs->getAngle() + _predAngular);
	starTrans = starTrans.translate((_starObs->getPosition() + _predLinear) * PHYSICS_SCALE);
	batch->fill(_starPoly, Vec2(0, 0), starTrans);

	batch->end();
}

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
void GameScene::buildGeometry() {
	// ADD CODE HERE
	_world->clear();

	SplinePather splinePather;
	splinePather.set(&_spline);
	splinePather.calculate();
	Path2 _path = splinePather.getPath();

	SimpleExtruder simpleExtruder;
	simpleExtruder.set(_path);
	simpleExtruder.calculate(LINE_WIDTH);
	_poly = simpleExtruder.getPolygon();

	Poly2 splineCopy = _poly;
	splineCopy /= PHYSICS_SCALE;
	_splineObs = physics2::PolygonObstacle::alloc(splineCopy);
	_splineObs->setBodyType(b2_staticBody);
	_splineObs->setPosition(getSize() / (2 * PHYSICS_SCALE));
	//_world->addObstacle(_splineObs);

	configure();

	//// Rectangle
	//SplinePather splinePather1;
	//splinePather1.set(&_rect);
	////splinePather1.calculate();
	//Path2 _path1 = splinePather1.getPath();

	/*SimpleExtruder simpleExtruder1;
	simpleExtruder1.set(_path1);
	simpleExtruder1.calculate(LINE_WIDTH);
	_rectPoly = simpleExtruder1.getPolygon();

	Poly2 rectCopy = _rectPoly;
	rectCopy /= PHYSICS_SCALE;
	_rectObs = physics2::PolygonObstacle::alloc(rectCopy);
	_rectObs->setBodyType(b2_staticBody);
	_rectObs->setPosition((0.5 * getSize()) / (2 * PHYSICS_SCALE));
	_world->addObstacle(_rectObs);*/

	/*PolyFactory rect;
	_rectPoly = rect.makeRect(-1 * getSize() / 2, Vec2(600, 100));
	Poly2 rectCopy = _rectPoly;
	rectCopy /= PHYSICS_SCALE;
	_rectObs = physics2::PolygonObstacle::alloc(rectCopy);
	_rectObs->setBodyType(b2_staticBody);
	_rectObs->setPosition(getSize() / (2 * PHYSICS_SCALE));
	_world->addObstacle(_rectObs);

	_rectPoly2 = rect.makeRect(Vec2(40, -getSize().height / 2), Vec2(600, 100));
	Poly2 rectCopy2 = _rectPoly2;
	rectCopy2 /= PHYSICS_SCALE;
	_rectObs2 = physics2::PolygonObstacle::alloc(rectCopy2);
	_rectObs2->setBodyType(b2_staticBody);
	_rectObs2->setPosition(getSize() / (2 * PHYSICS_SCALE));
	_world->addObstacle(_rectObs2);*/

	/*for (int pos = 0; pos <= 7; pos = pos + 2) {
		vector<Vec2> controlPoint(2);
		controlPoint[0] = _spline.getTangent(pos);
		if (pos == 0) controlPoint[1] = _spline.getTangent(7);
		else controlPoint[1] = _spline.getTangent(pos - 1);

		Path2 line(controlPoint);
		simpleExtruder.set(line);
		simpleExtruder.calculate(HANDLE_WIDTH);
		Poly2 handle = simpleExtruder.getPolygon();
		_handles.push_back(handle);

		PolyFactory polyfact;
		Poly2 left_knob = polyfact.makeRect(Vec2(0, 0), Vec2(50, 100));
		Poly2 right_knob = polyfact.makeRect(Vec2(0, 0), Vec2(50, 100));
		_knobs.push_back(left_knob);
		_knobs.push_back(right_knob);
	}*/

	Vec2* starVec = reinterpret_cast<Vec2*>(STAR);
	Path2 starPath(starVec, 10);
	starPath = starPath.reverse();
	EarclipTriangulator earclipTriangulator;
	earclipTriangulator.set(starPath);
	earclipTriangulator.calculate();
	_starPoly = earclipTriangulator.getPolygon();

	Poly2 starCopy = _starPoly;
	starCopy /= PHYSICS_SCALE;
	_starObs = physics2::PolygonObstacle::alloc(starCopy);
	_starObs->setBodyType(b2_dynamicBody);
	_starObs->setDensity(1);
	_starObs->setPosition(getSize() / (2 * PHYSICS_SCALE));
	_world->addObstacle(_starObs);
}
void GameScene::configure() {
	//std::ifstream i("rect.json");
	//nlohmann::json j;
	//i >> j;
	PolyFactory rect;
	Poly2 rectPoly = rect.makeRect(-1 * getSize() / 2, Vec2(600, 100));
	_rectPoly.push_back(rectPoly);
	Poly2 rectCopy = rectPoly;
	rectCopy /= PHYSICS_SCALE;
	_rectObs = physics2::PolygonObstacle::alloc(rectCopy);
	_rectObs->setBodyType(b2_staticBody);
	_rectObs->setPosition(getSize() / (2 * PHYSICS_SCALE));
	_world->addObstacle(_rectObs);

	Poly2 rectPoly2 = rect.makeRect(Vec2(40, -getSize().height / 2), Vec2(600, 100));
	_rectPoly.push_back(rectPoly2);
	Poly2 rectCopy2 = rectPoly2;
	rectCopy2 /= PHYSICS_SCALE;
	_rectObs2 = physics2::PolygonObstacle::alloc(rectCopy2);
	_rectObs2->setBodyType(b2_staticBody);
	_rectObs2->setPosition(getSize() / (2 * PHYSICS_SCALE));
	_world->addObstacle(_rectObs2);
}