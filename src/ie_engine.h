#pragma once

#include <SDL2/SDL.h>

#include "ie_assets.h"
#include "ie_camera.h"
#include "ie_lighting.h"
#include "ie_shader.h"
#include "ie_time.h"

namespace ie
{

  class Engine
  {
    public:
    Engine();
    private:
    bool init(void);
    bool loadAssets(void);
    bool run(void);
    bool cleanup(void);
    void render(void);
    void handleEvents(void);
    void handleLogic(void);
    void handleResize(int,int);
    bool initCamera(void);
    bool initLighting(void);
    bool initShaders(void);
    SDL_Window * mainWindow;
    SDL_GLContext mainGlContext;
    bool engineOn;
    ie::AssetManager am;
    ie::GlslCompiler compiler;
    ie::FrameClock frameClock;
    ie::Camera eye;
    ie::PointLight light;
  };

}
