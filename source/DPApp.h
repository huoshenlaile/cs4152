//
//  DPApp.h
//  Prototype1
//
//  Created by Xilai Dai on 3/8/24.
//

#ifndef __DP_APP_H__
#define __DP_APP_H__

#include <cugl/cugl.h>
#include "controllers/AudioController.h"
#include "controllers/CharacterController.h"
#include "controllers/InputController.h"
#include "controllers/InteractionController.h"
#include "controllers/NetworkController.h"

using namespace cugl::physics2::net;

/**
 * This class represents the application root for the ship demo.
 */
class DpApp : public cugl::Application {
    
enum Status {
    LOAD,
    MENU,
    HOST,
    CLIENT,
    LEVEL,
    GAME,
    SETTING
};

protected:
    /** The global sprite batch for drawing (only want one of these) */
    std::shared_ptr<cugl::SpriteBatch> _batch;
    /** The global asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;

    std::shared_ptr<NetEventController> _network;
    
    
    Status _status;
    
public:
#pragma mark Constructors
    /**
     * Creates, but does not initialized a new application.
     *
     * This constructor is called by main.cpp.  You will notice that, like
     * most of the classes in CUGL, we do not do any initialization in the
     * constructor.  That is the purpose of the init() method.  Separation
     * of initialization from the constructor allows main.cpp to perform
     * advanced configuration of the application before it starts.
     */
    DpApp() : cugl::Application() {}
    
    /**
     * Disposes of this application, releasing all resources.
     *
     * This destructor is called by main.cpp when the application quits.
     * It simply calls the dispose() method in Application.  There is nothing
     * special to do here.
     */
    ~DpApp() { }
    
    
#pragma mark Application State

    /**
     * The method called after OpenGL is initialized, but before running the application.
     *
     * This is the method in which all user-defined program intialization should
     * take place.  You should not create a new init() method.
     *
     * When overriding this method, you should call the parent method as the
     * very last line.  This ensures that the state will transition to FOREGROUND,
     * causing the application to run.
     */
    virtual void onStartup() override;
    
    /**
     * The method called when the application is ready to quit.
     *
     * This is the method to dispose of all resources allocated by this
     * application.  As a rule of thumb, everything created in onStartup()
     * should be deleted here.
     *
     * When overriding this method, you should call the parent method as the
     * very last line.  This ensures that the state will transition to NONE,
     * causing the application to be deleted.
     */
    virtual void onShutdown() override;
    
    /**
     * The method called when the application is suspended and put in the background.
     *
     * When this method is called, you should store any state that you do not
     * want to be lost.  There is no guarantee that an application will return
     * from the background; it may be terminated instead.
     *
     * If you are using audio, it is critical that you pause it on suspension.
     * Otherwise, the audio thread may persist while the application is in
     * the background.
     */
    virtual void onSuspend() override;
    
    /**
     * The method called when the application resumes and put in the foreground.
     *
     * If you saved any state before going into the background, now is the time
     * to restore it. This guarantees that the application looks the same as
     * when it was suspended.
     *
     * If you are using audio, you should use this method to resume any audio
     * paused before app suspension.
     */
    virtual void onResume()  override;
    
    
#pragma mark Application Loop
    

    virtual void preUpdate(float timestep) override;

    virtual void postUpdate(float timestep) override;

    virtual void fixedUpdate() override;
    
    virtual void update(float timestep) override;
    
    virtual void draw() override;
};
#endif /* DPApp_h */
