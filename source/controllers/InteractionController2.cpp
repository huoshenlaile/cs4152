#include "InteractionController2.h"

// std::unordered_map<std::string, std::vector<std::shared_ptr<Interactable>>> _HeadToInteractable;
bool InteractionController2::init(std::shared_ptr<LevelLoader2> level) {
    if (level == nullptr) {
        return false;
    }
    _level = level;
    _character = level->getCharacter();
    _world = level->getWorld();
    _worldnode = level->getWorldNode();
    _interactables = level->getInteractables();

    // query all the interactables
    for (auto interactable : _interactables) {
        if (interactable->hasTimeUpdate()) {
            _timeUpdateInteractables.push_back(interactable);
        }
        if (interactable->hasOnBeginContact()) {
            _BeginContactInteractable[interactable->getObstacleRawPtr()] = interactable;
        }
        if (interactable->hasOnEndContact()) {
            _EndContactInteractable[interactable->getObstacleRawPtr()] = interactable;
        }
        if (interactable->hasOnPreSolve()) {
            _PreSolveInteractable[interactable->getObstacleRawPtr()] = interactable;
        }
        if (interactable->hasOnPostSolve()) {
            _PostSolveInteractable[interactable->getObstacleRawPtr()] = interactable;
        }
        auto action_list = interactable->getActions();
        // Add current interactable to _HeadToInteractable for each string key in action_list
        for (const auto &key : action_list) {
            _HeadToInteractable[key.first].push_back(interactable);
        }
    }
    // link raw pointer to interactable
    for (auto interactable : _interactables) {
        _obstacleToInteractable[interactable->getObstacleRawPtr()] = interactable;
    }

    // link character parts
    characterLH = _character->getLHObstacle();
    characterRH = _character->getRHObstacle();
    characterBODY = _character->getBodyObstacle();
    characterLHRawPtr = characterLH.get();
    characterRHRawPtr = characterRH.get();
    characterBODYRawPtr = characterBODY.get();

    // clear message queue
    while (!_messageQueue.empty()) {
        _messageQueue.pop();
    }
    _levelComplete = false;

    // CULog the size of 5 methods.
    CULog("Size of _timeUpdateInteractables: %lu", _timeUpdateInteractables.size());
    CULog("Size of _BeginContactInteractable: %lu", _BeginContactInteractable.size());
    CULog("Size of _EndContactInteractable: %lu", _EndContactInteractable.size());
    CULog("Size of _PreSolveInteractable: %lu", _PreSolveInteractable.size());
    CULog("Size of _PostSolveInteractable: %lu", _PostSolveInteractable.size());

    return true;
}

void InteractionController2::beginContact(b2Contact *contact) {
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();

    physics2::Obstacle *obs1rawPtr = reinterpret_cast<physics2::Obstacle *>(body1->GetUserData().pointer);
    physics2::Obstacle *obs2rawPtr = reinterpret_cast<physics2::Obstacle *>(body2->GetUserData().pointer);
    // 4 cases: character + character, character + interactable (*2), interactable + interactable
    if (isCharacterObs(obs1rawPtr) && isCharacterObs(obs2rawPtr)) {
        return;
    } else if (isCharacterObs(obs1rawPtr) || isCharacterObs(obs2rawPtr)) {
        // TODO: port logic to deal with grab

        // logic to deal with interactable
        // 1: determine which is the interactable and which is the character

        physics2::Obstacle *interactableObsRawPtr;
        std::shared_ptr<physics2::Obstacle> characterPart;
        if (isCharacterObs(obs1rawPtr)) {
            interactableObsRawPtr = obs2rawPtr;
            characterPart = characterRawPtrToObstacle(obs1rawPtr);
        } else {
            interactableObsRawPtr = obs1rawPtr;
            characterPart = characterRawPtrToObstacle(obs2rawPtr);
        }

        // 2: find if the interactableObsRawPtr is using begin contact;
        auto beginContactIt = _BeginContactInteractable.find(interactableObsRawPtr);
        if (beginContactIt != _BeginContactInteractable.end()) {
            std::shared_ptr<Interactable> interactable = beginContactIt->second;
            // 3: call the onBeginContact function
            PublishedMessage message = interactable->onBeginContact(characterPart, contact, nullptr, true);
            if (message.Head != "") {
                _messageQueue.push(message);
            }
        }
    } else {
        // 2 interactables

        // 1: interactable 1 events
        auto i1BeginContactIt = _BeginContactInteractable.find(obs1rawPtr);
        if (i1BeginContactIt != _BeginContactInteractable.end()) {
            std::shared_ptr<Interactable> interactable1 = i1BeginContactIt->second;
            auto i2 = _obstacleToInteractable.find(obs2rawPtr);
            PublishedMessage message;
            // check if the second is also registered in the interactable list
            if (i2 != _obstacleToInteractable.end()) {
                std::shared_ptr<Interactable> interactable2 = i2->second;
                message = interactable1->onBeginContact(interactable2->getObstacle(), contact, interactable2, false);
            } else {
                message = interactable1->onBeginContact(nullptr, contact, nullptr, false);
            }
            if (message.Head != "") {
                _messageQueue.push(message);
            }
        }

        // 2: interactable 2 events
        auto i2BeginContactIt = _BeginContactInteractable.find(obs2rawPtr);
        if (i2BeginContactIt != _BeginContactInteractable.end()) {
            std::shared_ptr<Interactable> interactable2 = i2BeginContactIt->second;
            auto i1 = _obstacleToInteractable.find(obs1rawPtr);
            PublishedMessage message;
            // check if the second is also registered in the interactable list
            if (i1 != _obstacleToInteractable.end()) {
                std::shared_ptr<Interactable> interactable1 = i1->second;
                message = interactable2->onBeginContact(interactable1->getObstacle(), contact, interactable1, false);
            } else {
                message = interactable2->onBeginContact(nullptr, contact, nullptr, false);
            }
            if (message.Head != "") {
                _messageQueue.push(message);
            }
        }
    }
}

void InteractionController2::endContact(b2Contact *contact) {
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();

    physics2::Obstacle *obs1rawPtr = reinterpret_cast<physics2::Obstacle *>(body1->GetUserData().pointer);
    physics2::Obstacle *obs2rawPtr = reinterpret_cast<physics2::Obstacle *>(body2->GetUserData().pointer);
    // 4 cases: character + character, character + interactable (*2), interactable + interactable
    if (isCharacterObs(obs1rawPtr) && isCharacterObs(obs2rawPtr)) {
        return;
    } else if (isCharacterObs(obs1rawPtr) || isCharacterObs(obs2rawPtr)) {
        // TODO: port logic to deal with grab

        // logic to deal with interactable
        // 1: determine which is the interactable and which is the character

        physics2::Obstacle *interactableObsRawPtr;
        std::shared_ptr<physics2::Obstacle> characterPart;
        if (isCharacterObs(obs1rawPtr)) {
            interactableObsRawPtr = obs2rawPtr;
            characterPart = characterRawPtrToObstacle(obs1rawPtr);
        } else {
            interactableObsRawPtr = obs1rawPtr;
            characterPart = characterRawPtrToObstacle(obs2rawPtr);
        }

        // 2: find if the interactableObsRawPtr is using end contact;
        auto endContactIt = _EndContactInteractable.find(interactableObsRawPtr);
        if (endContactIt != _EndContactInteractable.end()) {
            std::shared_ptr<Interactable> interactable = endContactIt->second;
            // 3: call the onEndContact function
            PublishedMessage message = interactable->onEndContact(characterPart, contact, nullptr, true);
            if (message.Head != "") {
                _messageQueue.push(message);
            }
        }
    } else {
        // 2 interactables

        // 1: interactable 1 events
        auto i1EndContactIt = _EndContactInteractable.find(obs1rawPtr);
        if (i1EndContactIt != _EndContactInteractable.end()) {
            std::shared_ptr<Interactable> interactable1 = i1EndContactIt->second;
            auto i2 = _obstacleToInteractable.find(obs2rawPtr);
            PublishedMessage message;
            // check if the second is also registered in the interactable list
            if (i2 != _obstacleToInteractable.end()) {
                std::shared_ptr<Interactable> interactable2 = i2->second;
                message = interactable1->onEndContact(interactable2->getObstacle(), contact, interactable2, false);
            } else {
                message = interactable1->onEndContact(nullptr, contact, nullptr, false);
            }
            if (message.Head != "") {
                _messageQueue.push(message);
            }
        }

        // 2: interactable 2 events
        auto i2EndContactIt = _EndContactInteractable.find(obs2rawPtr);
        if (i2EndContactIt != _EndContactInteractable.end()) {
            std::shared_ptr<Interactable> interactable2 = i2EndContactIt->second;
            auto i1 = _obstacleToInteractable.find(obs1rawPtr);
            PublishedMessage message;
            // check if the second is also registered in the interactable list
            if (i1 != _obstacleToInteractable.end()) {
                std::shared_ptr<Interactable> interactable1 = i1->second;
                message = interactable2->onEndContact(interactable1->getObstacle(), contact, interactable1, false);
            } else {
                message = interactable2->onEndContact(nullptr, contact, nullptr, false);
            }
            if (message.Head != "") {
                _messageQueue.push(message);
            }
        }
    }
}

void InteractionController2::beforeSolve(b2Contact *contact, const b2Manifold *oldManifold) { return; }

void InteractionController2::afterSolve(b2Contact *contact, const b2ContactImpulse *impulse) { return; }

void InteractionController2::runMessageQueue() {
    // for each message in the queue, process it, call it's subscribers
    while (!_messageQueue.empty()) {
        PublishedMessage message = _messageQueue.front();
        _messageQueue.pop();
        std::string head = message.Head;
        // message to interactable
        auto headToInteractableIt = _HeadToInteractable.find(head);
        if (headToInteractableIt != _HeadToInteractable.end()) {
            for (auto interactable : headToInteractableIt->second) {
                auto actions = interactable->getActions();
                auto actionIt = actions.find(head);
                if (actionIt != actions.end()) {
                    ActionParams params = Interactable::ConvertToActionParams(message);
                    PublishedMessage new_message = actionIt->second(params);
                    if (new_message.Head != "") {
                        _messageQueue.push(new_message);
                    }
                }
            }
        }
        // message to character
        auto characterActions = _character->getActions();
        auto characterActionIt = characterActions.find(head);
        if (characterActionIt != characterActions.end()) {
            ActionParams params = Interactable::ConvertToActionParams(message);
            PublishedMessage new_message = characterActionIt->second(params);
            if (new_message.Head != "") {
                _messageQueue.push(new_message);
            }
        }
        // message to level
        if (head == "LevelComplete") {
            _levelComplete = true;
        }
    }
}

void InteractionController2::preUpdate(float timestep) {
    // for each interactable that has time update, call time update
    for (auto interactable : _timeUpdateInteractables) {
        PublishedMessage message = interactable->timeUpdate(timestep);
        if (message.Head != "") {
            _messageQueue.push(message);
        }
    }
    runMessageQueue();
}

void InteractionController2::postUpdate(float timestep) { runMessageQueue(); }

void InteractionController2::activateController() {
    // link the callbacks to the world
    if (_world == nullptr) {
        return;
    }
    _world->onBeginContact = [this](b2Contact *contact) { beginContact(contact); };
    _world->onEndContact = [this](b2Contact *contact) { endContact(contact); };
    _world->beforeSolve = [this](b2Contact *contact, const b2Manifold *oldManifold) { beforeSolve(contact, oldManifold); };
    _world->afterSolve = [this](b2Contact *contact, const b2ContactImpulse *impulse) { afterSolve(contact, impulse); };
    _world->activateCollisionCallbacks(true);
}