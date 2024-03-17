#include "InteractionController.h"

// TODO: are the platforms/buttons/walls going to be const? if so, make these pass by const & to avoid copying the vecotr
bool InteractionController::init(std::vector<std::shared_ptr<PlatformModel>> platforms,
    std::shared_ptr<CharacterController> characterA,
    std::shared_ptr<CharacterController> characterB,
    std::vector<std::shared_ptr<ButtonModel>> buttons, std::vector<std::shared_ptr<WallModel>> walls,
    std::shared_ptr<LevelLoader> level) {
    
    _platforms = platforms;
    _characterControllerA = characterA;
    _characterControllerB = characterB;
    _buttons = buttons;
    _walls = walls;
    _level = level;
    
    return true;
}


InteractionController::PlayersContact InteractionController::checkContactForPlayer(b2Body* body1, b2Body* body2){
    std::vector<std::shared_ptr<cugl::physics2::Obstacle>>  body_p1 = _characterControllerA->getObstacles();
    //TODO: Need second player
    //std::vector<std::shared_ptr<cugl::physics2::Obstacle>>  body_p2 = _characterControllerB->getObstacles();
    std::unordered_set<intptr_t> p1_ptrs;
    std::unordered_set<intptr_t> p2_ptrs;
    
    std::transform(body_p1.begin(), body_p1.end(), std::inserter(p1_ptrs, p1_ptrs.end()), [](std::shared_ptr<cugl::physics2::Obstacle> &n) {
            return reinterpret_cast<intptr_t>(n.get());
        });
   // std::transform(body_p2.begin(), body_p2.end(), std::inserter(p2_ptrs, p2_ptrs.end()), [](std::shared_ptr<cugl::physics2::Obstacle> &n) {
      //      return reinterpret_cast<intptr_t>(n.get());
    //    });
        
    PlayersContact output;
    if (p1_ptrs.find(body1->GetUserData().pointer) != p1_ptrs.end()){
        output.bodyOne = PLAYER_ONE;
    }
   // else if (p2_ptrs.find(body1->GetUserData().pointer) != p2_ptrs.end()){
    //    output.bodyOne = PLAYER_TWO;
    //}
    if (p1_ptrs.find(body2->GetUserData().pointer)!= p1_ptrs.end()){
        output.bodyTwo = PLAYER_ONE;
    }
    //else if (p2_ptrs.find(body2->GetUserData().pointer)!= p2_ptrs.end()){
    //    output.bodyTwo = PLAYER_TWO;
    //}
    return output;
}


void InteractionController::beginContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
    
    // If we hit the button
    // TODO: generalize this to all buttons
    //intptr_t button_ptr = 0; //reinterpret_cast<intptr_t>(_button.get());
    intptr_t goal_ptr = reinterpret_cast<intptr_t>(_level->getExit().get());
    InteractionController::PlayersContact contact_info = checkContactForPlayer(body1, body2);
    if (contact_info.bodyOne!=NOT_PLAYER || contact_info.bodyTwo != NOT_PLAYER){
        if(body1->GetUserData().pointer == goal_ptr || body2->GetUserData().pointer == goal_ptr) {
            // TODO: generalize this to all buttons
            // A player has pressed the button
            //_button_down=true;
            std::shared_ptr<std::unordered_map<std::string, std::string>> body;
            
            
            PublisherMessage pub = { "goalDoor", "contacted", "contacted", body};
            CULog("Reached goal");
            publishMessage(std::move(pub));
        }
    }
}

void InteractionController::publishMessage(PublisherMessage &&message){
    messageQueue.push(message);
    if (subscriptions.count(message.pub_id)==0){
        subscriptions[message.pub_id] = {};
    }
}

bool InteractionController::addSubscription(SubscriberMessage &&message){
    if (subscriptions.count(message.pub_id)==0){
        subscriptions[message.pub_id] = std::unordered_map<std::string, std::vector<SubscriberMessage>>();
    }
    if (subscriptions[message.pub_id].count(message.listening_for)>0){
        subscriptions[message.pub_id][message.listening_for].push_back(message);
    }
    else{
        subscriptions[message.pub_id][message.listening_for] = std::vector<SubscriberMessage>({message});
        
    }
   // std::cout << "SUBNEW " << subscriptions[message.pub_id][message.listening_for].back().pub_id << subscriptions[message.pub_id][message.listening_for].back().listening_for <<"\n";
    return true;
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
    
    InteractionController::PlayersContact contact_info = checkContactForPlayer(body1, body2);
    if (contact_info.bodyOne != NOT_PLAYER || contact_info.bodyTwo != NOT_PLAYER){
        // TODO: generalize this to all buttons
        if(body1->GetUserData().pointer == 0 || body2->GetUserData().pointer == 0) {
            // A player has released the button
            //_button_down = false;
            std::shared_ptr<std::unordered_map<std::string, std::string>> body;
            
            PublisherMessage pub = { "button1", "released", "released", body};
            publishMessage(std::move(pub));
        }
    }

}


void InteractionController::preUpdate(float dt) {
   
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
