#pragma once

#include <string>

#include <glm/glm.hpp>

namespace ie
{
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
}
