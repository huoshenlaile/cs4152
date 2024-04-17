#include "CameraController.h"

using namespace cugl;

bool CameraController::init(const std::shared_ptr<cugl::scene2::SceneNode> target, const std::shared_ptr<cugl::scene2::SceneNode> root, float lerp, std::shared_ptr<cugl::OrthographicCamera> camera,
                            std::shared_ptr<scene2::SceneNode> ui, float maxZoom, bool horizontal) {
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
    _displayed = false;
    _state = 0;
    return true;
}

/* Now it is a finite state machine */
void CameraController::update(float dt) {
    //CULog("in CamController Update, camera state: %d", _state);
    switch (_state) {
    // Initial stay
    case 0: {
        _initialStay++;
        _camera->update();
        if (cameraStay(INITIAL_STAY))
            _state = 1;
        break;
    }
    // Move the camera to the right
    case 1: {
        if (_horizontal) {
            _camera->translate(30, 0);
            _camera->update();
            if (_camera->getPosition().x >= _root->getSize().width - _camera->getViewport().getMaxX() / (2 * _camera->getZoom())) {
                _state = 2;
            }
        } else {
            _camera->translate(0, 30);
            _camera->update();
            if (_camera->getPosition().y >= _root->getSize().height - _camera->getViewport().getMaxY() / (2 * _camera->getZoom())) {
                _state = 2;
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
        Vec2::lerp(cameraPos, target, _lerp * dt, dst);
        // Make sure the camera never goes outside of the _root node's bounds
        (*dst).x = std::max(std::min(_root->getSize().width - _camera->getViewport().getMaxX() / (2 * _camera->getZoom()), (*dst).x), _camera->getViewport().getMaxX() / (2 * _camera->getZoom()));
        (*dst).y = std::max(std::min(_root->getSize().height - _camera->getViewport().getMaxY() / (2 * _camera->getZoom()), (*dst).y), _camera->getViewport().getMaxY() / (2 * _camera->getZoom()));
        _camera->translate((*dst).x - cameraPos.x, (*dst).y - cameraPos.y);
        delete dst;
        _camera->update();
        if (_levelComplete)
            _state = 4;
        break;
    }
    // Move to the first ending frame and stay
    case 4: {
        if (_horizontal) {
            if (_camera->getPosition().x <= _root->getSize().width - 1280 - _camera->getViewport().getMaxX() / (2 * _camera->getZoom())) {
                _camera->translate(30, 0);
                _camera->update();
            } else {
                if (cameraStay(END_STAY))
                    _state = 5;
            }
        } else {
            if (_camera->getPosition().x <= _root->getSize().height - 720 - _camera->getViewport().getMaxX() / (2 * _camera->getZoom())) {
                _camera->translate(0, 30);
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
        if (_horizontal) {
            if (_camera->getPosition().x <= _root->getSize().width - _camera->getViewport().getMaxX() / (2 * _camera->getZoom())) {
                _camera->translate(30, 0);
                _camera->update();
            } else {
                if (cameraStay(END_STAY)) {
                    _state = 0;
                    _completed = true;
                }
            }
        } else {
            if (_camera->getPosition().x <= _root->getSize().height - _camera->getViewport().getMaxX() / (2 * _camera->getZoom())) {
                _camera->translate(0, 30);
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
    CULog("current zoom is: %f", _camera->getZoom());
    // If this causes the camera to go out of bounds, revert the change
    /*if (_root->getSize().width < _camera->getViewport().getMaxX() / _camera->getZoom() || _root->getSize().height < _camera->getViewport().getMaxY() / _camera->getZoom()) {
            _camera->setZoom(originalZoom);
    }*/
    // Scale the UI so that it always looks the same size
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
    /* if (_root->getSize().width < _camera->getViewport().getMaxX() / _camera->getZoom() || _root->getSize().height < _camera->getViewport().getMaxY() / _camera->getZoom()) {
                 _camera->setZoom(originalZoom);
         }*/
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

void CameraController::levelComplete() {
    _levelComplete = true;
    _camera->setZoom(_levelCompleteZoom);
}

void CameraController::setCamera(std::string selectedLevelKey) {
    if (selectedLevelKey == "alpharelease") {
        setMode(true);
        setDefaultZoom(DEFAULT_ZOOM);
        _levelCompleteZoom = DEFAULT_ZOOM;
    } else if (selectedLevelKey == "tube") {
        setMode(false);
        setDefaultZoom(0.2);
        _levelCompleteZoom = 0.15;
    } else if (selectedLevelKey == "doodlejump") {
        setMode(true);
        setDefaultZoom(0.2);
        _levelCompleteZoom = 0.15;
    } else if (selectedLevelKey == "falldown") {
        setMode(false);
        setDefaultZoom(0.2);
        _levelCompleteZoom = 0.15;
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
