#pragma once
#include <string>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "ie_assets.h"
#include "ie_camera.h"
#include "ie_lighting.h"
#include "ie_shader.h"
#include "ie_time.h"
#include "ie_wavefront.h"

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
  bool loadOBJ(std::string);
  std::string vertexShaderFile;
  std::string fragmentShaderFile;
  SDL_Window * p_window;
  SDL_GLContext glContext;
  std::string WINDOW_TITLE;
  bool engine_on;
  unsigned short WINDOW_WIDTH;
  unsigned short WINDOW_HEIGHT;
  float FIELD_OF_VIEW;
  float ASPECT_RATIO;
  float Z_NEAR;
  float Z_FAR;
  unsigned int REQUIRED_SDL_MODULES;
  unsigned int SDL_MODE;
  unsigned int ACTIVEBUFFERS;
  bool WIREFRAME;
  bool DEPTHTEST;
  bool CULLFACE;
  glm::vec4 DEFAULT_CLEAR_COLOR;
  ie::AssetManager am;
  ie::GlslCompiler compiler;
  ie::FrameClock frameClock;
  ie::Camera eye;
  ie::PointLight light;
};
