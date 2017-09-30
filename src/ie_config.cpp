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
#include <sstream>

#define GL_GLEXT_PROTOTYPES //Needs to be define for some GL func to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <glm/trigonometric.hpp>
#include <glm/vec4.hpp>

#include "ie_utils.h"


char* homePath = SDL_GetBasePath();
std::string ie::HOME_PATH = std::string(homePath); 
std::string ie::WINDOW_TITLE = "Imagine Engine";
unsigned short ie::WINDOW_WIDTH = 800;
unsigned short ie::WINDOW_HEIGHT = 600;
float ie::ASPECT_RATIO = float(WINDOW_WIDTH) / float(WINDOW_HEIGHT);
float ie::FIELD_OF_VIEW = glm::radians(60.0f);
float ie::Z_NEAR = 0.01;
float ie::Z_FAR = 30.0;
unsigned int ie::REQUIRED_SDL_MODULES = SDL_INIT_EVERYTHING;
unsigned int ie::SDL_MODE = SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL;
unsigned int ie::ACTIVEBUFFERS = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;
bool ie::WIREFRAME_ON = false;
bool ie::DEPTHTEST_ON = true;
bool ie::CULLFACE_ON = true;
glm::vec4 ie::DEFAULT_CLEAR_COLOR = glm::vec4(0.0, 0.0, 0.0, 1.0);


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
  const unsigned char* local_gl_version_numeric = glGetString(GL_SHADING_LANGUAGE_VERSION);
  ss << local_gl_version_numeric;
  std::string gls = ss.str();
  gls = ie::split(gls, ' ', 0);
  gls = ie::removeChar(gls, '.');
  LOCAL_GL_VERSION_NUMERIC = stoi(gls);

  ss.str("");
  ss.clear();
  const unsigned char* local_glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
  ss << local_glsl_version;
  LOCAL_GLSL_VERSION = ss.str();

  ss.str("");
  ss.clear();
  const unsigned char* local_glsl_version_numeric = glGetString(GL_SHADING_LANGUAGE_VERSION);
  ss << local_glsl_version_numeric;
  std::string glsls = ss.str();
  glsls = ie::split(glsls, ' ', 0);
  glsls = ie::removeChar(glsls, '.');
  LOCAL_GLSL_VERSION_NUMERIC = stoi(glsls);
}
