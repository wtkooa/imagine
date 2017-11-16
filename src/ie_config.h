#ifndef IE_CONFIG_H
#define IE_CONFIG_H

//___|"ie_config.h"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <string>

#include <glm/vec4.hpp>

namespace ie
{
  extern int IE_MAJOR_VERSION;
  extern int IE_MINOR_VERSION;
  extern int IE_PATCH_VERSION;
  extern int DEFAULT_GL_MAJOR_VERSION;
  extern int DEFAULT_GL_MINOR_VERSION;
  extern unsigned int DEFAULT_IE_LOGGING;
  extern std::string DEFAULT_IE_LOGFILE;
  extern std::string HOME_PATH;
  extern std::string DEFAULT_WAVEFRONT_PATH;
  extern std::string DEFAULT_GLSL_PATH;
  extern std::string WINDOW_TITLE;
  extern glm::vec4 DEFAULT_CLEAR_COLOR;
  extern unsigned short WINDOW_WIDTH;
  extern unsigned short WINDOW_HEIGHT;
  extern float FIELD_OF_VIEW;
  extern float ASPECT_RATIO;
  extern float NEAR_PLANE;
  extern float FAR_PLANE;
  extern unsigned int REQUIRED_SDL_MODULES;
  extern unsigned int SDL_MODE;
  extern unsigned int ACTIVEBUFFERS;
  extern bool WIREFRAME_ON;
  extern bool DEPTHTEST_ON;
  extern bool CULLFACE_ON;
  extern bool SDL_MULTISAMPLING_ON;
  extern int SDL_MULTISAMPLING_SAMPLE_AMOUNT;
  extern const float TEXTURE_ANISOTROPY_LEVEL;
  extern const float TEXTURE_MIPMAP_LOD_BIAS;
  extern const float TEXTURE_ANISO_LOD_BIAS;
  extern float DEFAULT_PLAYER_MOVESPEED;
  extern float DEFAULT_PLAYER_TURNSPEED;
  extern float DEFAULT_CAMERA_LOOKSPEED; 
  extern const short MAX_TERRAIN_MATERIALS;
  extern const float DEFAULT_OBJ_SENSITIVITY;
  extern const short DEFAULT_TERRAIN_DIM;
  extern const short DEFAULT_TERRAIN_UNITS;

  class SDLContextDependentConfigs
  {
    public:
    void fetchSDLConfigs(void);
    void fetchSDLGLConfigs(void);

    int SDL_GL_MAJOR_VERSION;
    int SDL_GL_MINOR_VERSION;
    int SDL_MAJOR_VERSION_COMPILED;
    int SDL_MINOR_VERSION_COMPILED;
    int SDL_PATCH_VERSION_COMPILED;
    int SDL_MAJOR_VERSION_LINKED;
    int SDL_MINOR_VERSION_LINKED;
    int SDL_PATCH_VERSION_LINKED;
  };
  

  class OpenGlContextDependentConfigs
  {
    public:
    void fetchOpenGlConfigs(void);

    std::string LOCAL_GL_VENDOR;
    std::string LOCAL_GL_RENDERER;
    std::string LOCAL_GL_VERSION;
    std::string LOCAL_SHADING_LANGUAGE_VERSION;
    int LOCAL_GL_MAJOR_VERSION;
    int LOCAL_GL_MINOR_VERSION;
    bool ANISOTROPIC_AVAILABLE;
  };

}

#endif
