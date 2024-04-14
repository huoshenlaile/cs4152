#include "InteractionController2.h"

    // std::unordered_map<std::string, std::vector<std::shared_ptr<Interactable>>> _HeadToInteractable;
bool InteractionController2::init(std::shared_ptr<LevelLoader2> level){
    if (level == nullptr){
        return false;
    }
    _level = level;
    _character = level->getCharacter();
    _world = level->getWorld();
    _worldnode = level->getWorldNode();
    _interactables = level->getInteractables();

    // query all the interactables
    for (auto interactable : _interactables){
        if (interactable->hasTimeUpdate()){
            _timeUpdateInteractables.push_back(interactable);
        }
        if (interactable->hasOnBeginContact()){
            _BeginContactInteractable[interactable->getObstacleRawPtr()] = interactable;
        }
        if (interactable->hasOnEndContact()){
            _EndContactInteractable[interactable->getObstacleRawPtr()] = interactable;
        }
        if (interactable->hasOnPreSolve()){
            _PreSolveInteractable[interactable->getObstacleRawPtr()] = interactable;
        }
        if (interactable->hasOnPostSolve()){
            _PostSolveInteractable[interactable->getObstacleRawPtr()] = interactable;
        }
        std::map<std::string, std::function<PublishedMessage(ActionParams)>> action_list = interactable->getActions();
        // Add current interactable to _HeadToInteractable for each string key in action_list
        for (const auto& key : action_list){
            _HeadToInteractable[key.first].push_back(interactable);
        }
    }

    while (!_messageQueue.empty()){
        _messageQueue.pop();
    }
    _levelComplete = false;
    return true;
}

void InteractionController2::beginContact(b2Contact* contact){

}

void InteractionController2::endContact(b2Contact* contact){

}
void InteractionController2::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold){

}

void InteractionController2::afterSolve(b2Contact* contact, const b2ContactImpulse* impulse){

}

void InteractionController2::runMessageQueue(){
    // for each message in the queue, process it, call it's subscribers
    while (!_messageQueue.empty){
        PublishedMessage message = _messageQueue.pop();
        std::string head = message.Head;
        auto headToInteractableIt = _HeadToInteractable.find(head);
        if (headToInteractableIt != _HeadToInteractable.end()){
            for (auto interactable : headToInteractableIt->second){ 
                auto actions = interactable->getActions();
                auto actionIt = actions.find(head); 
                if (actionIt != actions.end()) { 
                    ActionParams params = ConvertToActionParams(message);
                    PublishedMessage new_message = actionIt->second(params); 
                    if (new_message.Head != ""){
                        _messageQueue.push(new_message);
                    }
                }
            }
        }
    }
}

void InteractionController2::preUpdate(float timestep){
    // for each interactable that has time update, call time update
    for (auto interactable : _timeUpdateInteractables){
        PublishedMessage message = interactable->timeUpdate(timestep);
        if (message.Head != ""){
            _messageQueue.push(message);
        }
    }
    runMessageQueue();
}

void InteractionController2::postUpdate(float timestep){
    runMessageQueue();
}

void InteractionController2::activateController(){
    // link the callbacks to the world
    if (_world == nullptr){
        return;
    }
    _world->onBeginContact = [this](b2Contact* contact){
        beginContact(contact);
    };
    _world->onEndContact = [this](b2Contact* contact){
        endContact(contact);
    };
    _world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold){
        beforeSolve(contact, oldManifold);
    };
    _world->afterSolve = [this](b2Contact* contact, const b2ContactImpulse* impulse){
        afterSolve(contact, impulse);
    };
}