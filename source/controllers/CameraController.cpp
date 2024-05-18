#include "CameraController.h"

using namespace cugl;

bool CameraController::init(const std::shared_ptr<cugl::scene2::SceneNode> target, const std::shared_ptr<cugl::scene2::SceneNode> root, float lerp, std::shared_ptr<cugl::OrthographicCamera> camera,
                            std::shared_ptr<scene2::SceneNode> ui, float maxZoom, bool horizontal, bool skipCameraSpan) {
    _target = target;
    _root = root;
    _lerp = lerp;
    _camera = camera;
    _maxZoom = maxZoom;
    _ui = ui;
    _initialStay = 0;
    _finalStay = 0;
    _horizontal = horizontal;
    _levelComplete = false;
    _counter = 0;
    _completed = false;
    _initialUpdate = false;
    _displayed = skipCameraSpan ? true : false;
    _moveToTop = false;
    _moveToLeft = false;
    _state = skipCameraSpan ? 3 : 0; // if skipping camera span is 3 just remain in game play
    _initPosOnce = 0;
    _replay = false;
    _skipCameraSpan = skipCameraSpan;
    _skipPosMove = false;
    return true;
}

/* Now it is a finite state machine */
void CameraController::update(float dt) {
    if (_replay) {
        _state = 0;
        _moveToLeft = false;
        _moveToTop = false;
        _replay = false;
        _displayed = false;
        this->setZoom(_levelCompleteZoom);
    }
    if (_skipCameraSpan && !_skipPosMove) {
        Vec2 targetPos = Vec2(_target->getWorldPosition().x, _target->getWorldPosition().y);
        _camera->setPosition(targetPos);
        _skipPosMove = true;
    }
    if (!_moveToLeft && _horizontal) {
        _camera->setPosition(Vec2(_camera->getViewport().getMaxX() / (2 * _camera->getZoom()), _camera->getViewport().getMaxY() / (2 * _camera->getZoom())));
        _moveToLeft = true;
        Vec2 uiPos =
            Vec2(_camera->getPosition().x - _camera->getViewport().getMaxX() / (2 * _camera->getZoom()), _camera->getPosition().y - _camera->getViewport().getMaxY() / (2 * _camera->getZoom()));
        _UIPosition = uiPos;
        _ui->setPosition(uiPos);
    }

    if (!_moveToTop && !_horizontal) {
        _camera->setPosition(Vec2(_root->getSize().width - _camera->getViewport().getMaxX() / (2 * _camera->getZoom()) - 20 + 100,
                                  _root->getSize().height - _camera->getViewport().getMaxY() / (2 * _camera->getZoom())));
        _moveToTop = true;
        Vec2 uiPos =
            Vec2(_camera->getPosition().x - _camera->getViewport().getMaxX() / (2 * _camera->getZoom()) - 20, _camera->getPosition().y - _camera->getViewport().getMaxY() / (2 * _camera->getZoom()));
        _UIPosition = uiPos;
        _ui->setPosition(uiPos);
    }

    switch (_state) {
    // Initial stay
    case 0: {
        _initialStay++;

        if (_initPosOnce == 0 && _horizontal) {
            _initPosOnce++;
            _camera->setPosition(Vec2(_camera->getPosition().x + 50, _camera->getPosition().y));
        }
        _camera->update();
        if (cameraStay(INITIAL_STAY)) {
            _state = 1;
        }

        break;
    }
    // Move the camera to the right
    case 1: {
        Vec2 panSpeed = _panSpeed;
        if (_horizontal) {
            if (_camera->getPosition().x <= _root->getSize().width - 500 - _camera->getViewport().getMaxX() / (2 * _camera->getZoom())) {
                _camera->translate(panSpeed.x, panSpeed.y);
                _camera->update();
            } else {
                if (cameraStay(FINAL_STAY))
                    _state = 2;
            }
        } else {
            if (_camera->getPosition().y >= 20 + _camera->getViewport().getMaxY() / (2 * _camera->getZoom())) {
                _camera->translate(panSpeed.x, panSpeed.y);
                _camera->update();
            } else {
                if (cameraStay(FINAL_STAY)) {
                    _state = 2;
                }
            }
        }
        break;
    }
    // Final stay
    case 2: {
        _finalStay++;
        _camera->update();
        if (cameraStay(FINAL_STAY)) {
            _state = 3;
            _displayed = true;
            this->setZoom(getDefaultZoom());
        }
        break;
    }
    // In the gameplay
    case 3: {
        Vec2 cameraPos = Vec2(_camera->getPosition().x, _camera->getPosition().y);
        Vec2 target;
        Vec2 *dst = new Vec2();
        // Lazily track the target using lerp
        target = Vec2(_target->getWorldPosition().x, _target->getWorldPosition().y);
        Vec2::lerp(cameraPos, target, 30 * dt, dst);
        // Make sure the camera never goes outside of the _root node's bounds
        (*dst).x = std::max(std::min(_root->getSize().width - _camera->getViewport().getMaxX() / (2 * _camera->getZoom()), (*dst).x), _camera->getViewport().getMaxX() / (2 * _camera->getZoom()));
        (*dst).y = std::max(std::min(_root->getSize().height - _camera->getViewport().getMaxY() / (2 * _camera->getZoom()), (*dst).y), _camera->getViewport().getMaxY() / (2 * _camera->getZoom()));
        _camera->translate((*dst).x - cameraPos.x, (*dst).y - cameraPos.y);
        delete dst;
        this->setZoom(getDefaultZoom());
       
        if (_levelComplete) {
            if (_doublePan)
                _state = 4;
            else
                _state = 5;
            this->setZoom(_levelCompleteZoom);
        }
        _camera->update();
        break;
    }
    // Move to the first ending frame and stay
    case 4: {
        this->setZoom(_levelCompleteZoom);
        _camera->update();
        Vec2 panSpeed = _panSpeed;
        if (_horizontal) {
            if (_camera->getPosition().x <= _root->getSize().width - 4200 - _camera->getViewport().getMaxX() / (2 * _camera->getZoom())) {
                _camera->translate(panSpeed.x, panSpeed.y);
                _camera->update();
            } else {
                if (cameraStay(END_STAY))
                    _state = 5;
            }
        } else {
            // Camera calibtrate
            _camera->setPosition(Vec2(_root->getSize().width - _camera->getViewport().getMaxX() / (2 * _camera->getZoom()), _camera->getPosition().y));
            if (_camera->getPosition().y >= (1000 + _camera->getViewport().getMaxY()) / (2 * _camera->getZoom())) {
                _camera->translate(panSpeed.x, panSpeed.y);
                _camera->update();
            } else {
                if (cameraStay(END_STAY))
                    _state = 5;
            }
        }
        break;
    }
    // Move to the second ending frame and stay
    case 5: {
        Vec2 panSpeed = _panSpeed;
        if (_horizontal) {
            if (_camera->getPosition().x <= _root->getSize().width - 500 - _camera->getViewport().getMaxX() / (2 * _camera->getZoom())) {
                _camera->translate(panSpeed.x, panSpeed.y);
                _camera->update();
            } else {
                if (cameraStay(END_STAY)) {
                    _state = 0;
                    _completed = true;
                }
            }
        } else {
            // Camera calibtrate
            _camera->setPosition(Vec2(_root->getSize().width - _camera->getViewport().getMaxX() / (2 * _camera->getZoom()), _camera->getPosition().y));
            if (_camera->getPosition().y >= _camera->getViewport().getMaxY() / (2 * _camera->getZoom())) {
                _camera->translate(panSpeed.x, panSpeed.y);
                _camera->update();
            } else {
                if (cameraStay(END_STAY)) {
                    _state = 0;
                    _completed = true;
                }
            }
        }
        break;
    }
    }

    Vec2 uiPos = Vec2(_camera->getPosition().x - _camera->getViewport().getMaxX() / (2 * _camera->getZoom()), _camera->getPosition().y - _camera->getViewport().getMaxY() / (2 * _camera->getZoom()));
    _UIPosition = uiPos;
    _ui->setPosition(uiPos);
}

void CameraController::setZoom(float zoom) {
    float originalZoom = _camera->getZoom();
    // Don't let it be greater than max zoom
    if (zoom > _maxZoom)
        return;
    _camera->setZoom(zoom);
    // CULog("current zoom is: %f", _camera->getZoom());
    //  If this causes the camera to go out of bounds, revert the change
    //  if (_root->getSize().width < _camera->getViewport().getMaxX() / _camera->getZoom() || _root->getSize().height < _camera->getViewport().getMaxY() / _camera->getZoom()) {
    //     _camera->setZoom(originalZoom);
    // }
    //  Scale the UI so that it always looks the same size
    _ui->setScale(1 / _camera->getZoom());
}

void CameraController::addZoom(float zoom) {
    float originalZoom = _camera->getZoom();
    // Don't let it be greater than max zoom
    if (originalZoom + zoom > _maxZoom)
        return;
    float truezoom = std::max(originalZoom + zoom, 0.01f);
    _camera->setZoom(truezoom);
    // If this causes the camera to go out of bounds, revert the change
    // if (_root->getSize().width < _camera->getViewport().getMaxX() / _camera->getZoom() || _root->getSize().height < _camera->getViewport().getMaxY() / _camera->getZoom()) {
    //    _camera->setZoom(originalZoom);
    //}
    // Scale the UI so that it always looks the same size
    _ui->setScale(1 / _camera->getZoom());
}

void CameraController::setTarget(std::shared_ptr<cugl::scene2::SceneNode> target) { _target = target; }

void CameraController::process(int zoomIn, float speed) {
    if (zoomIn == 0)
        return;
    float originalZoom = _camera->getZoom();
    float truezoom;
    if (zoomIn == 1) {
        if (originalZoom + speed > _maxZoom)
            return;
        truezoom = std::min(originalZoom + speed, _maxZoom);
    } else {
        if (originalZoom - speed < 0.25)
            return;
        truezoom = std::max(originalZoom - speed, 0.25f);
    }
    _camera->setZoom(truezoom);
    _ui->setScale(1 / _camera->getZoom());
}

void CameraController::levelComplete() { _levelComplete = true; }

void CameraController::setCamera(std::string selectedLevelKey, Vec2 activeSize) {
    //CULog("active size: %f, %f", activeSize.x, activeSize.y);
    if (selectedLevelKey == "tutorial") {
        // TODO: Implement Tutorial Zooms (a state machine)
        setMode(true);
        setDefaultZoom(activeSize.y / 576 * 0.4);
        _levelCompleteZoom = activeSize.y / 576 * 0.245;
        _panSpeed = Vec2(50, 0);
        _doublePan = false;
    } else if (selectedLevelKey == "level1" || selectedLevelKey == "level2" || selectedLevelKey == "level3") {
        setMode(true);
        setDefaultZoom(activeSize.y / 576 * 0.245);
        _levelCompleteZoom = activeSize.y / 576 * 0.245;
        _panSpeed = Vec2(50, 0);
        _doublePan = true;
    } else if (selectedLevelKey == "level4" || selectedLevelKey == "level5" || selectedLevelKey == "level6") {
        setMode(false);
        setDefaultZoom(activeSize.x / 1248.73 * 0.2);
        _levelCompleteZoom = activeSize.x / 1248.73 * 0.162;
        _panSpeed = Vec2(0, -50);
        _doublePan = true;
    } else if (selectedLevelKey == "level7" || selectedLevelKey == "level8" || selectedLevelKey == "level9") {
        setMode(false);
        setDefaultZoom(activeSize.x / 1248.73 * 0.2);
        _levelCompleteZoom = activeSize.x / 1248.73 * 0.162;
        _panSpeed = Vec2(0, -40);
        _doublePan = false;
    } else if (selectedLevelKey == "level10" || selectedLevelKey == "level11" || selectedLevelKey == "level12") {
        setMode(true);
        setDefaultZoom(activeSize.y / 576 * 0.245);
        _levelCompleteZoom = activeSize.y / 576 * 0.245;
        _panSpeed = Vec2(50, 0);
        _doublePan = false;
    } else if (selectedLevelKey == "level13" || selectedLevelKey == "level14" || selectedLevelKey == "level15") {
        setMode(false);
        setDefaultZoom(activeSize.x / 1248.73 * 0.225);
        _levelCompleteZoom = activeSize.x / 1248.73 * 0.2;
        _panSpeed = Vec2(0, -50);
        _doublePan = false;
    }
}

bool CameraController::cameraStay(int time) {
    if (_counter < time) {
        _counter++;
        return false;
    } else {
        _counter = 0;
        return true;
    }
}
