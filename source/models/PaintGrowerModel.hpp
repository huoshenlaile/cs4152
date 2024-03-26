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
  std::vector<cugl::Poly2> _paintFrames;
  std::vector<std::shared_ptr<Texture>> _textures;
    std::vector<std::shared_ptr<scene2::PolygonNode>> _sprites;
  int _currentFrame;

public:
    Color4 color;
    int timer;
    bool active;
    PaintModel(void);

    static std::shared_ptr<PaintModel> alloc(const std::vector<cugl::Poly2>& paintFrames, const std::vector<Vec2>& locations, const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<cugl::scene2::SceneNode>& worldnode, float scale){
        std::shared_ptr<PaintModel> result = std::make_shared<PaintModel>();
        return (result->init(paintFrames, locations, assets, worldnode, scale) ? result : nullptr);

    }
    
    bool init(const std::vector<cugl::Poly2>& paintFrames, const std::vector<Vec2>& locations, const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<cugl::scene2::SceneNode>& worldnode, float scale);
    
    cugl::Poly2 currentFrame() const;
    std::shared_ptr<Texture> currentTexture() const;
    std::shared_ptr<scene2::PolygonNode> currentNode() const;
    
    void trigger();
    void setup();
    void update(const std::shared_ptr<cugl::scene2::SceneNode>&, float millis);
    void draw();
    
    void clear();
        
};

#endif /* PaintGrowerModel_hpp */
