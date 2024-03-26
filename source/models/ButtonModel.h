//
//  ButtonModel.h
//  DustyPaints
//
//  Created by Haoxuan Zou on 3/11/24.
//

#ifndef ButtonModel_h
#define ButtonModel_h
#include <cugl/cugl.h>
#include <stdio.h>

using namespace cugl;

class ButtonModel {
public:
    /** Reference to the button (for collision detection) */
    std::shared_ptr<cugl::physics2::BoxObstacle>    _button;
    

    
    /** The button texture */
    std::string _buttonTexture = "button_ingame";
    /**
     * Creates a button
     *
     * @param  pos                       The location of the polygon
     * @param  size                     The size of the button texture
     * @param  publications   The publications that the button owns
     * @param  gamescale          The scale of the game
     *
     * @return  A newly allocated Button,
     */
    ButtonModel(const Vec2 pos, const Size size, const Vec2& publications, const std::string texture_name, const float gamescale);

    /**
     * Destroys this button, releasing all resources.
     */
    ~ButtonModel(void) {}
    
};

#endif /* ButtonModel_h */
