#include "InteractionController.h"

// TODO: are the platforms/buttons/walls going to be const? if so, make these pass by const & to avoid copying the vecotr
bool InteractionController::init(std::vector<std::shared_ptr<PlatformModel>> platforms,               std::shared_ptr<CharacterController> characterA,
    std::shared_ptr<CharacterController> characterB,
    std::vector<std::shared_ptr<ButtonModel>> buttons, std::vector<std::shared_ptr<WallModel>> walls) {
    
    _platforms = platforms;
    _characterControllerA = characterA;
    _characterControllerB = characterB;
    _buttons = buttons;
    _walls = walls;
    
    //TODO: Remove after testing
    SubscriberMessage sub = { "button1", "pressed",
        std::unordered_map<std::string, std::string>({{"open","true"}})};
    subscriptions["button1"]["pressed"].push_back(sub);
    
    return true;
}


InteractionController::PlayersContact InteractionController::checkContactForPlayer(b2Body* body1, b2Body* body2){
    std::vector<std::shared_ptr<cugl::physics2::Obstacle>>  body_p1 = _characterControllerA->getObstacles();
    std::vector<std::shared_ptr<cugl::physics2::Obstacle>>  body_p2 = _characterControllerB->getObstacles();
    std::unordered_set<intptr_t> p1_ptrs;
    std::unordered_set<intptr_t> p2_ptrs;
    
    std::transform(body_p1.begin(), body_p1.end(), std::inserter(p1_ptrs, p1_ptrs.end()), [](std::shared_ptr<cugl::physics2::Obstacle> &n) {
            return reinterpret_cast<intptr_t>(n.get());
        });
    std::transform(body_p2.begin(), body_p2.end(), std::inserter(p2_ptrs, p2_ptrs.end()), [](std::shared_ptr<cugl::physics2::Obstacle> &n) {
            return reinterpret_cast<intptr_t>(n.get());
        });
        
    PlayersContact output;
    if (p1_ptrs.find(body1->GetUserData().pointer) != p1_ptrs.end()){
        output.bodyOne = PLAYER_ONE;
    }
    else if (p2_ptrs.find(body1->GetUserData().pointer) != p2_ptrs.end()){
        output.bodyOne = PLAYER_TWO;
    }
    if (p1_ptrs.find(body2->GetUserData().pointer)!= p1_ptrs.end()){
        output.bodyTwo = PLAYER_ONE;
    }
    else if (p2_ptrs.find(body2->GetUserData().pointer)!= p2_ptrs.end()){
        output.bodyTwo = PLAYER_TWO;
    }
    return output;
}


void InteractionController::beginContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
    
    // If we hit the button
    // TODO: generalize this to all buttons
    intptr_t button_ptr = 0; //reinterpret_cast<intptr_t>(_button.get());
    bool _button_down = false;
        
    InteractionController::PlayersContact contact_info = checkContactForPlayer(body1, body2);
    if (contact_info.bodyOne!=NOT_PLAYER || contact_info.bodyTwo != NOT_PLAYER){
        if(body1->GetUserData().pointer == button_ptr || body2->GetUserData().pointer == button_ptr) {
            // TODO: generalize this to all buttons
            if (!_button_down){
                // A player has pressed the button
                //_button_down=true;
                std::shared_ptr<std::unordered_map<std::string, std::string>> body;
                
                
                PublisherMessage pub = { "button1", "pressed", "pressed", body};
                publishMessage(std::move(pub));
            }
        }
    }
}

void InteractionController::publishMessage(PublisherMessage &&message){
    messageQueue.push(message);
    if (subscriptions.count(message.pub_id)==0){
        subscriptions[message.pub_id] = {};
    }
}

/**
 * Processes the end of a collision
 *
 * @param  contact  The two bodies that collided
 */
void InteractionController::endContact(b2Contact* contact) {
    // This won't run for some reason, callback isn't being called
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
    // If we hit the button
    // TODO: generalize this to all buttons

    //intptr_t button_ptr = reinterpret_cast<intptr_t>(_button.get());
    CULog("ENDED CONTACT");
    
    InteractionController::PlayersContact contact_info = checkContactForPlayer(body1, body2);
    if (contact_info.bodyOne != NOT_PLAYER || contact_info.bodyTwo != NOT_PLAYER){
        // TODO: generalize this to all buttons
        if(body1->GetUserData().pointer == 0 || body2->GetUserData().pointer == 0) {
            // A player has released the button
            //_button_down = false;
            std::shared_ptr<std::unordered_map<std::string, std::string>> body;
            
            PublisherMessage pub = { "button1", "released", "released", body};
            messageQueue.push(pub);
            if (subscriptions.count("button1")==0){
                subscriptions["button1"] = {};
            }
        }
    }

}


void InteractionController::preUpdate(float dt) {
    while (!messageQueue.empty()){
        PublisherMessage publication = messageQueue.front();
        for(const SubscriberMessage& s : subscriptions[publication.pub_id][publication.trigger]){
            // TODO: Replace with running the action specified in s
            if (s.listening_for=="pressed"){
                CULog("Do press button action");
            }
            if (s.listening_for=="released"){
                CULog("Do release button action");
            }
            std::cout << s.pub_id<< " " << s.listening_for <<"\n";
        }
        messageQueue.pop();
    }
}



void InteractionController::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {
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
}
