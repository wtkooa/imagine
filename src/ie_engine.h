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

#include "ie_assetmanager.h"
#include "ie_camera.h"
#include "ie_config.h"
#include "ie_lighting.h"
#include "ie_render.h"
#include "ie_shader.h"
#include "ie_time.h"
#include "ie_vram.h"

namespace ie
{

  class Engine
  {
    public:
    Engine();

    private:
    bool init(void);
    bool initOpenGl(void);
    bool initSdl(void);
    bool initCamera(void);
    bool initLighting(void);
    bool initShaders(void);
    bool initAssets(void);
    bool initVram(void);
    bool initRenderers(void);

    bool run(void);
    void render(void);
    void handleEvents(void);
    void handleUpdates(void);
    void handleMessages(void);
    void handleLogic(void);
    void handleResize(int,int);

    bool cleanup(void);

    SDL_Window * mainWindow;
    SDL_GLContext mainGlContext;
    bool engineOn;

    ie::AssetManager am;
    ie::VramManager vram;
    ie::GlslCompiler compiler;
    ie::FrameClock frameClock;
    ie::Camera eye;
    ie::RenderManager rm;

    ie::OpenGlContextDependentConfigs openGlConfigs;
  };

}

#endif
