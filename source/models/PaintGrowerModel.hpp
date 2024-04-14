//
//  PaintGrowerModel.hpp
//  DustyPaints
//
//  Created by Emily on 3/24/24.
//

#ifndef PaintGrowerModel_hpp
#define PaintGrowerModel_hpp

#include <stdio.h>
#include <vector>

#include <cugl/cugl.h>

using namespace cugl;

class PaintModel {
private:
  CU_DISALLOW_COPY_AND_ASSIGN(PaintModel);
protected:
    cugl::Poly2 _paintFrames;
    Vec2 _locations;
    std::string _textures;
    std::shared_ptr<scene2::PolygonNode> _sprites;
    int _currentFrame;
    std::shared_ptr<cugl::scene2::SceneNode> _worldnode;
    
    std::shared_ptr<cugl::scene2::ActionManager> _actions;
    std::shared_ptr<cugl::scene2::Animate> _animate;
    std::shared_ptr<scene2::SpriteNode> _texture;

public:
    std::string color;
    int timer;
    bool active;
    PaintModel(void);
    int paint_id;

    static std::shared_ptr<PaintModel> alloc(cugl::Poly2 paintFrames, Vec2 locations){
        std::shared_ptr<PaintModel> result = std::make_shared<PaintModel>();
        return (result->init(paintFrames, locations) ? result : nullptr);

    }
    
    bool init(cugl::Poly2 paintFrames, Vec2 locations);
    
    cugl::Poly2 currentFrame() const;
    std::string currentTexture() const;
    std::shared_ptr<scene2::PolygonNode> currentNode() const;
    
    cugl::Poly2 getPaintFrames() const { return _paintFrames; }
    
    std::string getTextures() const { return _textures; }
    std::shared_ptr<scene2::PolygonNode> getPolygons() const { return _sprites; }
    
    void setNode(std::shared_ptr<scene2::PolygonNode> node) { _sprites = node; }
    
    void setSpriteNode(const std::shared_ptr<scene2::SpriteNode>& node) { _texture = node; }

    void setColor(const std::string& c) { color = c; }
    Vec2 getLocations() const { return _locations; }
    
    void trigger();
    void setup();
    void update(const std::shared_ptr<cugl::scene2::SceneNode>&, float millis);
    void draw();
    
    void clear();
        
};

#endif /* PaintGrowerModel_hpp */
