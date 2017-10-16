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
#include "ie_camera.h"
#include "ie_config.h"
#include "ie_controller.h"
#include "ie_lighting.h"
#include "ie_player.h"
#include "ie_render_engine.h"
#include "ie_scenegraph.h"
#include "ie_shader.h"
#include "ie_time.h"
#include "ie_vram_manager.h"

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
    bool initController(void);
    bool initPlayer(void);
    bool initCamera(void);

    bool run(void);
    void render(void);
    void handleEvents(void);
    void handleUpdates(void);
    void handleLogic(void);
    void handleResize(int,int);

    bool cleanup(void);

    SDL_Window * mainWindow;
    SDL_GLContext mainGlContext;
    bool engineOn;

    ie::AssetManager am;
    ie::VramManager vram;
    ie::FrameClock frameClock;
    ie::Camera eye;
    ie::Player player;
    ie::Controller control;
    ie::RenderEngine re;
    ie::SceneGraph sg;

    ie::OpenGlContextDependentConfigs openGlConfigs;
  };

}

#endif
