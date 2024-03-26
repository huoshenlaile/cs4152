//
//  PaintGrowerModel.cpp
//  DustyPaints
//
//  Created by Emily on 3/24/24.
//

#include "PaintGrowerModel.hpp"
using namespace cugl;

PaintModel::PaintModel(void) : timer(1000){
}

bool PaintModel::init(const std::vector<cugl::Poly2>& paintFrames, const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<cugl::scene2::SceneNode>& worldnode){
    
    std::vector<Vec2> vertices;
    vertices.push_back({1, 1});
    vertices.push_back({101,1});
    vertices.push_back({101,101});
    vertices.push_back({1, 101});
    cugl::Poly2 paint(vertices);
    
    std::vector<Vec2> vertices1;
    vertices1.push_back({0, 0});
    vertices1.push_back({100,0});
    vertices1.push_back({100, 100});
    vertices1.push_back({0, 100});

    cugl::Poly2 paint1(vertices1);
        
    std::vector<Vec2> vertices2;
    vertices2.push_back({0, 0});
    vertices2.push_back({5,5});
    vertices2.push_back({0, 5});
    vertices2.push_back({5,0});

    cugl::Poly2 paint2(vertices2);
    
    Rect rec3 (0, 0, 300, 300);
    Rect rec2 ( 0, 0, 200, 200);
    Rect rec1(0, 0, 100, 100);

    std::vector<cugl::Poly2> polys{rec1, rec2, rec3};

    _paintFrames = polys;
    _currentFrame = 0;
    timer = 1000;
    color = Color4::RED;
    
    std::vector<std::string> splotches{"splotch3", "splotch2", "splotch1"};
    std::vector<Vec2> locations{{700, 700}, {700, 700}, {700, 700} };
    for(int i  = 0; i < splotches.size(); i++){
        std::shared_ptr<Texture> image = assets->get<Texture>(splotches[i]);
        if (image == nullptr) {
            return false;
        }
        else {
            _textures.push_back(image);
            std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image, polys[i]);
            //sprite->setColor(Color4::BLACK);
            if(i != 0){
                sprite->setVisible(false);
            }
            _sprites.push_back(sprite);
            sprite->setPosition(locations[i]);
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

void PaintModel::setup() {}

void PaintModel::update(const std::shared_ptr<cugl::scene2::SceneNode>& worldnode) {
    //std::cout << "PaintMODEL UPDATE CALLED" << std::endl;
    //currentNode()->setColor(Color4::BLUE);
    currentNode()->setVisible(false);
    _currentFrame = (_currentFrame + 1) % _paintFrames.size();
    currentNode()->setVisible(true);
}
void PaintModel::draw() {}

void clear() {}

