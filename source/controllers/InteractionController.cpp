#include "InteractionController.h"

bool InteractionController::init(
    std::shared_ptr<CharacterController> characterA,
    std::shared_ptr<CharacterController> characterB,
    std::vector<std::shared_ptr<ButtonModel>> buttons, 
    std::vector<std::shared_ptr<WallModel>> walls,
    std::shared_ptr<LevelLoader> level,
    std::shared_ptr<cugl::physics2::net::NetWorld> &world,
    const std::shared_ptr<cugl::JsonValue>& json) {
    
    _characterControllerA = characterA;
    _characterControllerB = characterB;
    _buttons = buttons;
    _walls = walls;
    _level = level;
    _levelJson = json;
    this -> _world = world;
    _obstaclesForJoint = {};
    leftHandReverse = false;
    rightHandReverse = false;
    publications = {};
    subscriptions = {};
    for (int i = 0; i < json->get("layers")->size(); i++) {
        // Get the objects per layer
        auto objects = json->get("layers")->get(i)->get("objects");
        for (int j = 0; j < objects->size(); j++) {
            if (objects->get(j)->get("properties") != nullptr){
#pragma mark Refactored Pub/Sub Init
//                std::cout << (objects -> get(j) -> get("type") -> toString()) << std::endl;
                // how WEIRD! get("type") returns ""sensor"" or ""wall"" (TWO layers), NOT "sensor" or "wall"!
                std::shared_ptr<JsonValue> properties = objects -> get(j) -> get("properties");
                for (auto jsonChild : properties -> children()) {
                    if (jsonChild -> getString("name") == "Publication") {
                        std::shared_ptr<JsonValue> p = jsonChild -> get("value");
                        std::cout << "\nThis is a Publication! \nPublication message: " << p -> getString("Message") << ", pub_id: " << p -> getString("Pub_id") << ", trigger: " << p -> getString("Trigger") << std::endl << std::endl;
                        PublisherMessage pub = {p->getString("Pub_id"),
                                                p->getString("Trigger"),
                                                p->getString("Message")};
                        addPublisher(pub);
                    }
                    if (jsonChild -> getString("name") == "Subscription") {
                        std::shared_ptr<JsonValue> s = jsonChild -> get("value");
                        std::cout << "\n This is a Subscription! \nSubcription message: " << s -> getString("Listening_for") << ", pub_id: " << s -> getString("Pub_id") << ", sub_id: " << s -> getString("Sub_id") << std::endl;
                        std::unordered_map<std::string, std::string> actions;
                        for (std::shared_ptr<JsonValue> a : s->get("Actions")->children()){
                            actions[a->key()] = a->asString();
                            std::cout << "Actions: " << a->key() << ": " << a->asString() << std::endl;
                        }
                        SubscriberMessage sub = {s->getString("Pub_id"), 
                                                s->getString("Listening_for"),
                                                actions};
                        addSubscription(sub);
                    }
                }
            }
            
        }
    }

    return true;
}


InteractionController::PlayerCounter InteractionController::checkContactForPlayer(b2Body* body1, b2Body* body2){
    std::vector<std::shared_ptr<cugl::physics2::Obstacle>>  characterObstacles = _characterControllerA->getObstacles();
    //TODO: Need second player?
    std::unordered_set<intptr_t> p1_ptrs;
    std::unordered_set<intptr_t> p2_ptrs;
    
    std::transform(characterObstacles.begin(), characterObstacles.end(), std::inserter(p1_ptrs, p1_ptrs.end()), [](std::shared_ptr<cugl::physics2::Obstacle> &n) {
            intptr_t bodyPartIntPtr = reinterpret_cast<intptr_t>(n.get());
            return bodyPartIntPtr;
        });
    PlayerCounter output;
    if (p1_ptrs.find(body1->GetUserData().pointer) != p1_ptrs.end()){
        output.bodyOne = PLAYER_ONE;
    }
    if (p1_ptrs.find(body2->GetUserData().pointer)!= p1_ptrs.end()){
        output.bodyTwo = PLAYER_ONE;
    }
    
    
    // check for hands
    std::vector<std::shared_ptr<cugl::physics2::Obstacle>> handObstacles = _characterControllerA -> getHandObstacles();
    p1_ptrs.clear();
    p2_ptrs.clear();
    std::transform(handObstacles.begin(), handObstacles.end(), std::inserter(p1_ptrs, p1_ptrs.end()), [](std::shared_ptr<cugl::physics2::Obstacle> &n) {
            intptr_t bodyPartIntPtr = reinterpret_cast<intptr_t>(n.get());
            return bodyPartIntPtr;
        });
    if (p1_ptrs.find(body1->GetUserData().pointer) != p1_ptrs.end()){
        if (reinterpret_cast<intptr_t>(handObstacles[0].get()) == body1->GetUserData().pointer) {
            // it is left hand
            output.handIsLeft = true;
        } else {
            // it is right hand
            output.handIsRight = true;
        }
        output.bodyOneIsHand = true;
    }
    if (p1_ptrs.find(body2->GetUserData().pointer)!= p1_ptrs.end()){
        if (reinterpret_cast<intptr_t>(handObstacles[0].get()) == body2->GetUserData().pointer) {
            // it is left hand
            output.handIsLeft = true;
        } else {
            // it is right hand
            output.handIsRight = true;
        }
        output.bodyTwoIsHand = true;
    }
    
    return output;
}


// WHY DO YOU use the RValue Reference as the parameter???
// I don't get it! -- George
// I have CHANGED it to LValue Ref. DO NOT USE RVALUE if you have a good reason. I don't see any here.
void InteractionController::publishMessage(PublisherMessage &message){
    messageQueue.push(message);
    if (subscriptions.count(message.pub_id)==0){
        subscriptions[message.pub_id] = {};
    }
}



bool InteractionController::addSubscription(SubscriberMessage &message){
    if (subscriptions.count(message.pub_id)==0){
        subscriptions[message.pub_id] = std::unordered_map<std::string, std::vector<SubscriberMessage>>();
    }
    if (subscriptions[message.pub_id].count(message.listening_for)>0){
        subscriptions[message.pub_id][message.listening_for].push_back(message);
    }
    else{
        subscriptions[message.pub_id][message.listening_for] = std::vector<SubscriberMessage>({message});
        
    }
    std::cout << "adding Sub (from addSupscription):  " << subscriptions[message.pub_id][message.listening_for].back().pub_id << " (pub_id) and " << subscriptions[message.pub_id][message.listening_for].back().listening_for <<" (listening_for) \n";
    return true;
}


bool InteractionController::addPublisher(PublisherMessage &message){
    if (publications.count(message.trigger)==0){
        publications[message.trigger] = std::unordered_map<std::string, PublisherMessage>();
    }
    publications[message.trigger][message.pub_id] = message;
    std::cout << "addign Pub (from addPublisher):  " << publications[message.trigger][message.pub_id].pub_id << " (pub_id) and " << publications[message.trigger][message.pub_id].trigger << " (trigger) \n";
    return true;
}


#pragma mark Begin Contact
void InteractionController::beginContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();

    InteractionController::PlayerCounter contact_info = checkContactForPlayer(body1, body2);
    
    if ((contact_info.bodyOneIsHand || contact_info.bodyTwoIsHand) && !(contact_info.bodyOneIsHand && contact_info.bodyTwoIsHand)) {
        // Please do not remove the comments in this if block. I'm using it to 
        // reserve some implementational thoughts on grabbing.
        // one of the collision body is player's hand. We now grab it!
        auto obstacleA = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
        auto obstacleB = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);
        
        // now we push it into the vector to create joints later.
        // NOTE: we should care about which obstacle is the hand and push it first.
        if (contact_info.bodyOneIsHand) {
            std::string platformName = obstacleB -> getName();
            // NOTE: Below, you MUST check platformname != "" before
            // you go publications["whatever"][platformName].message ... . Because
            // Otherwise it will CREATE this entry!
            if (publications["contacted"].count(platformName) < 0 || platformName == "" ||  publications["contacted"][platformName].message != "grabbed") {
                // this platform cannot be grabbed.
            } else {
                // obstacle A is the hand, B is the platform
//                _obstaclesForJoint.push_back(_obstacleMap[obstacleA]);
//                _obstaclesForJoint.push_back(_obstacleMap[obstacleB]);
                if (contact_info.handIsLeft) {
                    // it is left hand
                    if (_leftHandIsHeld && _LHGrabCD <= 0) {
                        _obstaclesForJoint.push_back(_obstacleMap[obstacleA]);
                        _obstaclesForJoint.push_back(_obstacleMap[obstacleB]);
                        _leftHandIsGrabbed = true;
                    }
                } else {
                    if (_rightHandIsHeld && _RHGrabCD <= 0) {
                        _obstaclesForJoint.push_back(_obstacleMap[obstacleA]);
                        _obstaclesForJoint.push_back(_obstacleMap[obstacleB]);
                        _rightHandIsGrabbed = true;
                    }
                }
            }
        } else {
            std::string platformName = obstacleA -> getName();
            if (publications["contacted"].count(platformName) < 0 || platformName == "" || publications["contacted"][platformName].message != "grabbed") {
                // this platform cannot be grabbed.
            } else {
                // obstacle B is the hand, A is the platform
//                _obstaclesForJoint.push_back(_obstacleMap[obstacleB]);
//                _obstaclesForJoint.push_back(_obstacleMap[obstacleA]);
                if (contact_info.handIsLeft) {
                    // it is left hand
                    if (_leftHandIsHeld && _LHGrabCD <= 0) {
                        _obstaclesForJoint.push_back(_obstacleMap[obstacleB]);
                        _obstaclesForJoint.push_back(_obstacleMap[obstacleA]);
                        _leftHandIsGrabbed = true;
                    }
                } else {
                    if (_rightHandIsHeld && _RHGrabCD <= 0) {
                        _obstaclesForJoint.push_back(_obstacleMap[obstacleB]);
                        _obstaclesForJoint.push_back(_obstacleMap[obstacleA]);
                        _rightHandIsGrabbed = true;
                    }
                }
            }
        }
    }

    if (contact_info.bodyOne!=NOT_PLAYER || contact_info.bodyTwo != NOT_PLAYER){
        cugl::physics2::Obstacle* other_body;
        if (contact_info.bodyOne!=NOT_PLAYER){
            other_body = reinterpret_cast<cugl::physics2::Obstacle*>(body2->GetUserData().pointer);
        }
        else {
            other_body = reinterpret_cast<cugl::physics2::Obstacle*>(body1->GetUserData().pointer);
        }
        std::string obj_name = other_body->getName();

        if (publications["contacted"].count(obj_name) > 0){
            // so, obj_name is the pub_id. "contacted" is the trigger.
            PublisherMessage pub = publications["contacted"][obj_name];
            publishMessage(pub);
            // do you even KNOW what you are doing with Object c(std::move(a))? You are saying: Dear constructor, do whatever you want with 'a' in order to initialize 'c'; I don't care about a anymore. Feel free to have your way with 'a'.
            // check: https://stackoverflow.com/questions/3106110/what-is-move-semantics
            std::cout << "Published (begin contact): " << pub.pub_id << ": " << pub.message << "\n";
        }
    }
}


#pragma mark End Contact
void InteractionController::endContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();

    InteractionController::PlayerCounter contact_info = checkContactForPlayer(body1, body2);
    if (contact_info.bodyOne!=NOT_PLAYER || contact_info.bodyTwo != NOT_PLAYER){
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
            publishMessage(pub);
            std::cout << "Published (end contact) : " << pub.pub_id << ": " << pub.message << "\n";
        }
    }

}


void InteractionController::preUpdate(float dt) {
    
   
}


void InteractionController::detectPolyContact(const std::shared_ptr<scene2::PolygonNode>& poly2, float scale){
    
    // convert poly2 vertices
    b2DistanceProxy pol2;
    std::vector<b2Vec2> vertices;
    
//    for(const Vec2& v : poly2.getVertices() ){
//        vertices.push_back({v.x, v.y});
//    }
//    pol2.Set(vertices.data(), poly2.vertices.size(), 0);
    
    auto right_hand = _characterControllerA->getRightHandPosition() * scale;
    if(poly2->getBoundingBox().contains(right_hand)){
        // TODO: Change this to color of the paint, should be easy
        _characterControllerA->setColor("orange");
    } else if(poly2->getBoundingBox().contains(_characterControllerA->getLeftHandPosition() * scale)) {
        _characterControllerA->setColor("orange");
    }
}


void InteractionController::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {
}


void InteractionController::connectGrabJoint() {
    if (!_obstaclesForJoint.empty()) {
        std::shared_ptr<physics2::Obstacle> obs1 = _obstaclesForJoint.at(0);
        std::shared_ptr<physics2::Obstacle> obs2 = _obstaclesForJoint.at(1);
        Vec2 difference = obs2 -> getPosition() - obs1 -> getPosition();
        _joint = physics2::RevoluteJoint::allocWithObstacles(_obstaclesForJoint.at(0), _obstaclesForJoint.at(1));
        // Anchor A is the anchor for the Hand.
        _joint -> setLocalAnchorA(difference);
        _joint -> setLocalAnchorB(0, 0);
        _world -> addJoint(_joint);
        
        // why are we not judging if it is a lh joint or rh joint according to
        // _leftHandIsGrabbed and _rHIG? Because those might be already true!
        float LHDistance = obs1 -> getPosition().distance(_characterControllerA -> getLeftHandPosition());
        float RHDistance = obs1 -> getPosition().distance(_characterControllerA -> getRightHandPosition());
        if (LHDistance < RHDistance) {
            std::cout << "Now reversing left hand" << std::endl;
            this -> leftHandReverse = true;
            _leftHandJoint = _joint;
        } else {
            std::cout << "Now reversing right hand" << std::endl;
            this -> rightHandReverse = true;
            _rightHandJoint = _joint;
        }

        _obstaclesForJoint.clear();
    }
}

void InteractionController::ungrabIfNecessary() {
    if (_leftHandIsGrabbed && !_leftHandIsHeld) {
        std::cout << "Removing LH Joint - from ungrab" << std::endl;
        _world -> removeJoint(_leftHandJoint);
        _leftHandJoint = nullptr;
        _leftHandIsGrabbed = false;
        leftHandReverse = false;
        _LHGrabCD = GRAB_CD;
    }
    if (_rightHandIsGrabbed && !_rightHandIsHeld) {
        std::cout << "Removing RH Joint - from ungrab" << std::endl;
        _world -> removeJoint(_rightHandJoint);
        _rightHandJoint = nullptr;
        _rightHandIsGrabbed = false;
        rightHandReverse = false;
        _RHGrabCD = GRAB_CD;
    }
}

void InteractionController::grabCDIfNecessary(float dt) {
    if (!_leftHandIsGrabbed && _LHGrabCD >= 0) {
        _LHGrabCD -= dt;
    }
    if (!_rightHandIsGrabbed && _RHGrabCD >= 0) {
        _RHGrabCD -= dt;
    }
}
