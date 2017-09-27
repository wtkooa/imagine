#include "ie_config.h"

#define GL_GLEXT_PROTOTYPES //Needs to be define for some GL func to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>


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
