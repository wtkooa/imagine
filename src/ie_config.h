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
  extern std::string HOME_PATH;
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
  extern float DEFAULT_PLAYER_MOVESPEED;
  extern float DEFAULT_PLAYER_TURNSPEED;
  extern float DEFAULT_CAMERA_LOOKSPEED; 
  

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
    bool ANISOTROPIC_AVAILABLE;
  };

}

#endif
