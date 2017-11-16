//___|"ie_config.cpp"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_config.h"

#include <iostream>
#include <string>
#include <cstring>
#include <sstream>

#define GL_GLEXT_PROTOTYPES //Needs to be define for some GL func to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/trigonometric.hpp>
#include <glm/vec4.hpp>
#include <SDL2/SDL.h>

#include "ie_const.h"
#include "ie_utils.h"

int ie::IE_MAJOR_VERSION = 0;
int ie::IE_MINOR_VERSION = 1;
int ie::IE_PATCH_VERSION = 0;


int ie::DEFAULT_GL_MAJOR_VERSION = 4;
int ie::DEFAULT_GL_MINOR_VERSION = 5;
unsigned int ie::DEFAULT_IE_LOGGING = ie::IE_LOG_ALL;
std::string ie::DEFAULT_IE_LOGFILE = "ie_engine.log";
char* homePath = SDL_GetBasePath();
std::string ie::HOME_PATH = std::string(homePath); 
std::string ie::DEFAULT_WAVEFRONT_PATH = ie::HOME_PATH + "data/wavefront/";
std::string ie::DEFAULT_GLSL_PATH = ie::HOME_PATH + "src/glsl/";
std::string ie::WINDOW_TITLE = "Imagine Engine";
unsigned short ie::WINDOW_WIDTH = 800;
unsigned short ie::WINDOW_HEIGHT = 600;
float ie::ASPECT_RATIO = float(WINDOW_WIDTH) / float(WINDOW_HEIGHT);
float ie::FIELD_OF_VIEW = glm::radians(60.0f);
float ie::NEAR_PLANE = 0.01;
float ie::FAR_PLANE = 250.0;
unsigned int ie::REQUIRED_SDL_MODULES = SDL_INIT_EVERYTHING;
unsigned int ie::SDL_MODE = SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL;
unsigned int ie::ACTIVEBUFFERS = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;
bool ie::WIREFRAME_ON = false;
bool ie::DEPTHTEST_ON = true;
bool ie::CULLFACE_ON = true;
bool ie::SDL_MULTISAMPLING_ON = true;
int ie::SDL_MULTISAMPLING_SAMPLE_AMOUNT = 4;
glm::vec4 ie::DEFAULT_CLEAR_COLOR = glm::vec4(0.466f, 0.709f, 0.996, 1.0);
float ie::DEFAULT_PLAYER_MOVESPEED = 6.0; //Meters Per Second
float ie::DEFAULT_PLAYER_TURNSPEED = glm::radians(0.05f); //Degrees per rel mouse movement 
float ie::DEFAULT_CAMERA_LOOKSPEED = glm::radians(0.05f); //Degrees per rel mouse  movment
const short ie::MAX_TERRAIN_MATERIALS = 8; 
const float ie::TEXTURE_MIPMAP_LOD_BIAS = -1.0f;
const float ie::TEXTURE_ANISO_LOD_BIAS = 0.0f;
const float ie::TEXTURE_ANISOTROPY_LEVEL = 4.0f;
const float ie::DEFAULT_OBJ_SENSITIVITY = 0.0001f;
const short ie::DEFAULT_TERRAIN_DIM = 100;
const short ie::DEFAULT_TERRAIN_UNITS = 1;

void ie::SDLContextDependentConfigs::fetchSDLGLConfigs(void)
{
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                      &SDL_GL_MAJOR_VERSION);
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                      &SDL_GL_MINOR_VERSION);
}

void ie::SDLContextDependentConfigs::fetchSDLConfigs(void)
{

  SDL_version compiled;
  SDL_version linked;
  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);
  SDL_MAJOR_VERSION_COMPILED = compiled.major;
  SDL_MINOR_VERSION_COMPILED = compiled.minor;
  SDL_PATCH_VERSION_COMPILED = compiled.patch;
  SDL_MAJOR_VERSION_LINKED = linked.major;
  SDL_MINOR_VERSION_LINKED = linked.minor;
  SDL_PATCH_VERSION_LINKED = linked.patch;
}


bool checkForExtension(std::string extension)
{
  std::stringstream ss;
  int extAmount;
  glGetIntegerv(GL_NUM_EXTENSIONS, &extAmount);

  for (int n = 0; n < extAmount; n++)
  {
    const GLubyte* str = glGetStringi(GL_EXTENSIONS, n);
    ss <<str;
    std::string newStr = ss.str();
    if (newStr == extension)
    {
      return true;
    }
    ss.str("");
    ss.clear();
  }
  return false;
}


//FETCHING LOCAL OPENGL VERSIONING INFO FROM VIDEO HARDWARE
void ie::OpenGlContextDependentConfigs::fetchOpenGlConfigs(void)
{

  const unsigned char* local_gl_vendor = glGetString(GL_VENDOR);
  std::stringstream ss;
  ss << local_gl_vendor;
  LOCAL_GL_VENDOR = ss.str(); 

  ss.str("");
  ss.clear();
  const unsigned char* local_gl_renderer = glGetString(GL_RENDERER);
  ss << local_gl_renderer;
  LOCAL_GL_RENDERER = ss.str();

  ss.str("");
  ss.clear();
  const unsigned char* local_gl_version = glGetString(GL_VERSION);
  ss << local_gl_version;
  LOCAL_GL_VERSION = ss.str();

  ss.str("");
  ss.clear();
  const unsigned char* local_shading_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
  ss << local_shading_version;
  LOCAL_SHADING_LANGUAGE_VERSION = ss.str();

  glGetIntegerv(GL_MAJOR_VERSION, &LOCAL_GL_MAJOR_VERSION);
  glGetIntegerv(GL_MINOR_VERSION, &LOCAL_GL_MINOR_VERSION);

  ANISOTROPIC_AVAILABLE = checkForExtension("GL_EXT_texture_filter_anisotropic");

}
