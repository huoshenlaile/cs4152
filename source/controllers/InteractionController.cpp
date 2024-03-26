#include "InteractionController.h"

// TODO: are the platforms/buttons/walls going to be const? if so, make these pass by const & to avoid copying the vecotr
bool InteractionController::init(std::vector<std::shared_ptr<PlatformModel>> platforms,
    std::shared_ptr<CharacterController> characterA,
    std::shared_ptr<CharacterController> characterB,
    std::vector<std::shared_ptr<ButtonModel>> buttons, std::vector<std::shared_ptr<WallModel>> walls,
    std::shared_ptr<LevelLoader> level,
    const std::shared_ptr<cugl::JsonValue>& json) {
    
    _platforms = platforms;
    _characterControllerA = characterA;
    _characterControllerB = characterB;
    _buttons = buttons;
    _walls = walls;
    _level = level;
    for (int i = 0; i < json->get("layers")->size(); i++) {
        // Get the objects per layer
        auto objects = json->get("layers")->get(i)->get("objects");
        for (int j = 0; j < objects->size(); j++) {
            if (objects->get(j)->get("properties") != nullptr){
                auto obs = objects->get(j)->get("properties")->get(0)->get("value")->get("obstacle");
                auto pubs = obs->get("publications");
                auto subs = obs->get("subscriptions");
                
                if (pubs!=nullptr){
                    for (std::shared_ptr<JsonValue> p : pubs->children()){
                        std::unordered_map<std::string, std::string> body;
                        for (std::shared_ptr<JsonValue> b : p->get("body")->children()){
                            body[b->key()] = b->asString();
                        }
                        PublisherMessage pub = {p->get("pub_id")->asString(), p->get("trigger")->asString(), p->get("message")->asString(), body};
                        addPublisher(std::move(pub));
                    }
                }
                if (subs!=nullptr){
                    for (std::shared_ptr<JsonValue> s : subs->children()){
                        std::unordered_map<std::string, std::string> actions;
                        for (std::shared_ptr<JsonValue> a : s->get("actions")->children()){
                            actions[a->key()] = a->asString();
                        }
                        SubscriberMessage sub = {s->get("pub_id")->asString(), s->get("listening_for")->asString(), actions};
                        addSubscription(std::move(sub));
                    }
                }
            }
            
        }
    }

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
    std::cout << "SUBNEW " << subscriptions[message.pub_id][message.listening_for].back().pub_id << subscriptions[message.pub_id][message.listening_for].back().listening_for <<"\n";
    return true;
}

bool InteractionController::addPublisher(PublisherMessage &&message){
    if (publications.count(message.trigger)==0){
        publications[message.trigger] = std::unordered_map<std::string, PublisherMessage>();
    }
    publications[message.trigger][message.pub_id] = message;
    std::cout << "PUBNEW " << publications[message.trigger][message.pub_id].pub_id << publications[message.trigger][message.pub_id].trigger <<"\n";
    return true;
}

void InteractionController::beginContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
    
    // If we hit the button
    // TODO: generalize this to all buttons
    //intptr_t button_ptr = 0; //reinterpret_cast<intptr_t>(_button.get());

    InteractionController::PlayersContact contact_info = checkContactForPlayer(body1, body2);
    if (contact_info.bodyOne!=NOT_PLAYER || contact_info.bodyTwo != NOT_PLAYER){
        //std::cout << "Player touch\n";
        //std::cout << body1->GetUserData().pointer << "\n";
       // std::cout << body2->GetUserData().pointer << "\n";
        //std::cout << goal_ptr << "\n";
        cugl::physics2::Obstacle* other_body;
        if (contact_info.bodyOne!=NOT_PLAYER){
            other_body = reinterpret_cast<cugl::physics2::Obstacle*>(body2->GetUserData().pointer);
        }
        else {
            other_body = reinterpret_cast<cugl::physics2::Obstacle*>(body1->GetUserData().pointer);
        }
        std::string obj_name = other_body->getName();
        if (publications["contacted"].count(obj_name)>0){
            PublisherMessage pub = publications["contacted"][obj_name];
            publishMessage(std::move(pub));
            std::cout << "Published: " << pub.pub_id << ": " << pub.message << "\n";
        }
    }
}

/**
 * Processes the end of a collision
 *
 * @param  contact  The two bodies that collided
 */
void InteractionController::endContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();

    InteractionController::PlayersContact contact_info = checkContactForPlayer(body1, body2);
    if (contact_info.bodyOne!=NOT_PLAYER || contact_info.bodyTwo != NOT_PLAYER){
        //std::cout << "Player touch\n";
        //std::cout << body1->GetUserData().pointer << "\n";
       // std::cout << body2->GetUserData().pointer << "\n";
        //std::cout << goal_ptr << "\n";
        cugl::physics2::Obstacle* other_body;
        if (contact_info.bodyOne!=NOT_PLAYER){
            other_body = reinterpret_cast<cugl::physics2::Obstacle*>(body2->GetUserData().pointer);
        }
        else {
            other_body = reinterpret_cast<cugl::physics2::Obstacle*>(body1->GetUserData().pointer);
        }
        std::string obj_name = other_body->getName();
        if (publications["released"].count(obj_name)>0){
            PublisherMessage pub = publications["released"][obj_name];
            publishMessage(std::move(pub));
            std::cout << "Published: " << pub.pub_id << ": " << pub.message << "\n";
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
