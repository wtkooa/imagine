#ifndef IE_CONFIG_H
#define IE_CONFIG_H

#include <string>

#include <glm/glm.hpp>

namespace ie
{
  extern std::string HOME_PATH;
  extern std::string WINDOW_TITLE;
  extern glm::vec4 DEFAULT_CLEAR_COLOR;
  extern unsigned short WINDOW_WIDTH;
  extern unsigned short WINDOW_HEIGHT;
  extern float FIELD_OF_VIEW;
  extern float ASPECT_RATIO;
  extern float Z_NEAR;
  extern float Z_FAR;
  extern unsigned int REQUIRED_SDL_MODULES;
  extern unsigned int SDL_MODE;
  extern unsigned int ACTIVEBUFFERS;
  extern bool WIREFRAME_ON;
  extern bool DEPTHTEST_ON;
  extern bool CULLFACE_ON;
  
  class OpenGlContextDependentConfigs
  {
    public:
    void fetchOpenGlConfigs(void);

    std::string LOCAL_GL_VENDOR;
    std::string LOCAL_GL_RENDERER;
    std::string LOCAL_GL_VERSION;
    int LOCAL_GL_VERSION_NUMERIC;
    std::string LOCAL_GLSL_VERSION;
    int LOCAL_GLSL_VERSION_NUMERIC;
  };

}

#endif
