#include "deviceInputScale.h"



// TODO: Implement getInputScale() function
// based on the device model identifier
// https://iosref.com/res
// https://gist.github.com/adamawolf/3048717


// float getInputScale(){
//     const char * Cidentifier = getDeviceModelIdentifier(); 
    
//     string id = string(Cidentifier);
//     CULog("Device Model Identifier: %s", id.c_str());
    
//     float screen_width = 0.0;
//     if (id == "iPhone16,2"){
//         screen_width = 
//     }
    

    
//     return 1.0;
// }


float getInputScale() {
    float ppi = cugl::Display::get()->getPixelDensity();
    auto s = cugl::Application::get()->getDisplaySize();
    float screen_height = s.height;
    float t = screen_height / (round(ppi));
    return t / 428.0 * INPUT_SCALER;
}
