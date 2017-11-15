#ifndef IE_ENGINE_H
#define IE_ENGINE_H

//___|"ie_engine.h"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <SDL2/SDL.h>

#include "ie_asset_manager.h"
#include "ie_config.h"
#include "ie_controller.h"
#include "ie_creation.h"
#include "ie_import.h"
//#include "ie_physics_engine.h"
//#include "ie_render_engine.h"
//#include "ie_scenegraph.h"
#include "ie_time.h"
//#include "ie_vram_manager.h"

namespace ie
{

  class Engine
  {
    public:
    Engine();

    private:
    bool init(void);
    bool initSdl(void);
    bool initOpenGl(void);
    bool initShaders(void);
    bool initAssets(void);
    bool initVram(void);
    bool initRenderers(void);
    bool initSceneGraph(void);
    bool initPhysics(void);
    bool initController(void);

    bool run(void);
    void update(void);
    void render(void);

    bool cleanup(void);

    SDL_Window * mainWindow;
    SDL_GLContext mainGlContext;
    bool engineRun;

    ie::AssetManager am;
    ie::LoadManager local; 
    ie::CreationManager editor;
    //ie::VramManager vram;
    ie::FrameClock clock;
    ie::Controller ctrl;
    //ie::RenderEngine re;
    //ie::PhysicsEngine fizx;
    //ie::SceneGraph sg;

    ie::OpenGlContextDependentConfigs openGlConfigs;
  };

}

#endif
