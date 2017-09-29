#ifndef IE_ENGINE_H
#define IE_ENGINE_H

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
    bool initRenderers(void);
    bool initAssets(void);

    bool run(void);
    void render(void);
    void handleEvents(void);
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
    ie::PointLight light;
    ie::StaticRender staticRender;

    ie::OpenGlContextDependentConfigs openGlConfigs;
  };

}

#endif
