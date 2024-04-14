//
//  PaintGrowerModel.cpp
//  DustyPaints
//
//  Created by Emily on 3/24/24.
//

#include "PaintGrowerModel.hpp"
using namespace cugl;

PaintModel::PaintModel(void) : timer(1000), active(false){
}

bool PaintModel::init(cugl::Poly2 paintFrames, Vec2 locations){
    _actions = scene2::ActionManager::alloc();
    _animate = scene2::Animate::alloc(0, 8, 1.0f, 0);

    _locations = locations;
    _paintFrames = paintFrames;
    _currentFrame = 0;
    timer = 1000;
    active = false;
    //color = Color4::RED;
    
    std::vector<std::string> splotches{"splotch3"};
    //std::vector<Vec2> locations{{180, 50}, {180, 50}, {180, 50} };
    for(int i  = 0; i < splotches.size(); i++){
        std::string image = splotches[i];
        _textures = image;
        //std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image, _paintFrames);
        //sprite->setColor(Color4::BLACK);
        //sprite->setVisible(false);
        
        //_sprites = sprite;
        //sprite->setPosition(locations);
        //worldnode->addChild(sprite);
        //std::cout << "Paint Polygon Position: " << sprite->getPositionX() << ", " << sprite->getPositionY() << std::endl;
        
    }
    //trigger();
    return true;

}

cugl::Poly2 PaintModel::currentFrame() const {
    return _paintFrames;
}

std::string PaintModel::currentTexture() const {
    return _textures;
}
std::shared_ptr<scene2::PolygonNode> PaintModel::currentNode() const {
    return _sprites;
}

void PaintModel::trigger(){
    if(!active){
        active = true;
        timer = 1000;
        if(currentNode()){
            currentNode()->setVisible(true);
        }
    }
}

void PaintModel::setup() {}

void PaintModel::update(const std::shared_ptr<cugl::scene2::SceneNode>& worldnode, float millis) {
    //std::cout << "PaintMODEL UPDATE CALLED" << std::endl;
    if(active){
        _actions->activate("other", _animate, _texture);
        _actions->update(millis);

        if(_currentFrame == _paintFrames.size() - 1){ // DONE
            active = false;
            return;
        }
                
    }
}
void PaintModel::draw() {}

void PaintModel::clear() {
    _worldnode->removeChild(_sprites);
}

