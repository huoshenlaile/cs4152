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

bool PaintModel::init(const std::vector<cugl::Poly2>& paintFrames, const std::vector<Vec2>& locations, const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<cugl::scene2::SceneNode>& worldnode, float scale){
    
    /*std::vector<Vec2> vertices;
    vertices.push_back({0, 0});
    vertices.push_back({0, 500});
    vertices.push_back({500,500});
    vertices.push_back({500, 0});
    cugl::Poly2 paint1(vertices);
    
    std::vector<Vec2> vertices1;
    vertices1.push_back({0, 0});
    vertices1.push_back({0,600});
    vertices1.push_back({600, 600});
    vertices1.push_back({600, 0});

    cugl::Poly2 paint2(vertices1);
        
    std::vector<Vec2> vertices2;
    vertices2.push_back({0, 0});
    vertices2.push_back({0,700});
    vertices2.push_back({700, 700});
    vertices2.push_back({700,0});

    cugl::Poly2 paint3(vertices2);*/

    _paintFrames = paintFrames;
    _currentFrame = 0;
    timer = 1000;
    active = false;
    color = Color4::RED;
    
    std::vector<std::string> splotches{"splotch3", "splotch2", "splotch1"};
    //std::vector<Vec2> locations{{180, 50}, {180, 50}, {180, 50} };
    for(int i  = 0; i < splotches.size(); i++){
        std::shared_ptr<Texture> image = assets->get<Texture>(splotches[i]);
        if (image == nullptr) {
            return false;
        }
        else {
            _textures.push_back(image);
            std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image, _paintFrames[i]);
            //sprite->setColor(Color4::BLACK);
            sprite->setVisible(false);
            
            _sprites.push_back(sprite);
            sprite->setPosition(locations[i] * scale);
            worldnode->addChild(sprite);
            std::cout << "POLY POSITION" << sprite->getPositionX() << ", " << sprite->getPositionY() << std::endl;
        }
    }
    return true;

}

cugl::Poly2 PaintModel::currentFrame() const {
    return _paintFrames[_currentFrame];
}

std::shared_ptr<Texture> PaintModel::currentTexture() const {
    return _textures[_currentFrame];
}
std::shared_ptr<scene2::PolygonNode> PaintModel::currentNode() const {
    return _sprites[_currentFrame];
}

void PaintModel::trigger(){
    if(!active){
        active = true;
        timer = 1000;
        currentNode()->setVisible(true);
    }
}

void PaintModel::setup() {}

void PaintModel::update(const std::shared_ptr<cugl::scene2::SceneNode>& worldnode, float millis) {
    //std::cout << "PaintMODEL UPDATE CALLED" << std::endl;
    if(active){
        if(_currentFrame == _paintFrames.size() - 1){ // DONE
            active = false;
            return;
        }
        
        if(timer <= 0){
            CULog("UPDATE!");
            currentNode()->setVisible(false);
            _currentFrame = (_currentFrame + 1) % _paintFrames.size();
            currentNode()->setVisible(true);
            timer = 1000;
        } else {
            timer -= millis;
        }
        
    }
}
void PaintModel::draw() {}

void clear() {}
