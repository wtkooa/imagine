#include "ie_engine.h"

#include <iostream>
#include <map>
#include <string>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

#include "ie_assetmanager.h"
#include "ie_config.h"
#include "ie_const.h"
#include "ie_camera.h"
#include "ie_lighting.h"
#include "ie_messages.h"
#include "ie_packages.h"
#include "ie_render.h"
#include "ie_shader.h"
#include "ie_time.h"
#include "ie_utils.h"
#include "ie_vram.h"
#include "ie_wavefront.h"

ie::Engine::Engine(void)
{
  init();
  run();
  cleanup();
}

bool ie::Engine::init(void)
{
  SDL_Init(ie::REQUIRED_SDL_MODULES);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  mainWindow = SDL_CreateWindow(ie::WINDOW_TITLE.c_str(),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                ie::WINDOW_WIDTH, ie::WINDOW_HEIGHT,
                ie::SDL_MODE);
  mainGlContext = SDL_GL_CreateContext(mainWindow);
  SDL_GL_SetSwapInterval(ie::LOCK_TO_LOCAL_FRAMERATE);
  glViewport(0, 0, ie::WINDOW_WIDTH, ie::WINDOW_HEIGHT);
  if (ie::WIREFRAME_ON) {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);}
  if (ie::DEPTHTEST_ON) {glEnable(GL_DEPTH_TEST);}
  if (ie::CULLFACE_ON) {glEnable(GL_CULL_FACE);}
  glClearColor(ie::DEFAULT_CLEAR_COLOR.r, ie::DEFAULT_CLEAR_COLOR.g,
               ie::DEFAULT_CLEAR_COLOR.b, ie::DEFAULT_CLEAR_COLOR.a);
  glClear(ie::ACTIVEBUFFERS);
  initCamera();
  initLighting();
  initShaders();
  initAssets();
  initRenders();
  return true;
}

bool ie::Engine::initCamera(void)
{
  eye.setWindow(mainWindow);
  eye.setMoveSpeed(3.0f); //Meters per Second
  eye.setLookSpeed(glm::radians(0.05f)); //Degrees per rel movment
  eye.setProjectionMatrix(glm::perspective(ie::FIELD_OF_VIEW,
                                           ie::ASPECT_RATIO,
                                           ie::Z_NEAR, ie::Z_FAR));
  return true;
}

bool ie::Engine::initLighting(void)
{
  light.setPosVector(glm::vec3(0.0f, 3.0f, 0.0f));
  light.setGlobalAmbient(glm::vec3(0.1f, 0.1f, 0.1f));
  light.setLightAmbient(glm::vec3(0.2f, 0.2f, 0.2f));
  light.setLightSpecular(glm::vec3(0.4f, 0.4f, 0.4f));
  light.setLightDiffuse(glm::vec3(0.8f, 0.8f, 0.8f));
  light.setConstantFalloff(1.0f);
  light.setLinearFalloff(0.1f);
  light.setQuadraticFalloff(0.0f);
  return true;
}

bool ie::Engine::initShaders(void)
{
  ie::ShaderProgramPackage statPack = compiler.compile("Static",
                                                       "src/glsl/vstaticshader.glsl",
                                                       "src/glsl/fstaticshader.glsl");
  am.unwrapPackage(statPack);
  return true;
}

bool ie::Engine::initAssets(void)
{
  ie::WavefrontObjectFileReader objReader;  
  ie::WavefrontObjectFilePackage pack1 = objReader.read("data/Cube.obj");
  am.unwrapPackage(pack1);

  ie::CreateVboMessage vboMsg = am.sendCreateVboMessage();
  vram.receiveMessage(vboMsg);
  am.createQuickLists();
  return true;
}

bool ie::Engine::initRenders(void)
{
  ie::RenderAssetMessage assetsToRender = am.sendRenderAssetMessage("Static", "vtnList");
  ie::RenderMemoryMessage memoryToRender = vram.sendRenderMemoryMessage("vtnPair");
  staticRender.receiveMessage(assetsToRender);
  staticRender.receiveMessage(memoryToRender);
}

bool ie::Engine::run(void)
{
  engineOn = true;
  while (engineOn) {
    frameClock.measure();
    handleEvents();
    handleLogic();
    render();
  } 
  return true;
}

void ie::Engine::handleLogic(void)
{
  unsigned int CubeId = am.modelNameIdMap["Cube"];
  glm::mat4 transMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f));
  glm::mat4 rotMatrix = glm::rotate(glm::mat4(), glm::radians(0.5f), glm::vec3(1.0f, 1.0f, 0.0f));
  am.modelAssets[CubeId].translationMatrix = transMatrix; 
  am.modelAssets[CubeId].rotationMatrix *= rotMatrix;
}

void ie::Engine::render(void)
{
  glClear(ACTIVEBUFFERS);

  float frameDelta = frameClock.getFrameDelta();
  eye.frameUpdate(frameDelta);
  ie::RenderCameraMessage cameraMsg = eye.sendRenderCameraMessage();
  ie::RenderLightMessage lightMsg = light.sendRenderLightMessage();

  staticRender.receiveMessage(lightMsg);
  staticRender.receiveMessage(cameraMsg);
  staticRender.render();

  SDL_GL_SwapWindow(mainWindow);
}

void ie::Engine::handleEvents(void)
{
  SDL_Event evnt;
  while (SDL_PollEvent(&evnt))
  {
    switch (evnt.type)
    {
      case SDL_QUIT:
        engineOn = false;
        break;
      case SDL_WINDOWEVENT:
        switch (evnt.window.event)
        {
        case SDL_WINDOWEVENT_RESIZED:
          handleResize(evnt.window.data1, evnt.window.data2);
          break;
        }
        break;
      case SDL_MOUSEMOTION:
        if (SDL_GetRelativeMouseMode() == SDL_TRUE)
        {
          eye.rotateEventVec.x += evnt.motion.xrel;  
          eye.rotateEventVec.y += evnt.motion.yrel;  
        }
        break;
      case SDL_KEYDOWN:
        switch (evnt.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            engineOn = false;
            break;
          case SDLK_e:
            if (!evnt.key.repeat)
            {
              eye.toggleGrabMode();
            }
            break;
          case SDLK_w:
            if (!evnt.key.repeat)
            { 
              eye.translEventVec.z += 1.0;
            }
            break;
          case SDLK_s:
            if (!evnt.key.repeat)
            {
              eye.translEventVec.z -= 1.0;
            }
            break;
          case SDLK_d:
            if (!evnt.key.repeat)
            {
              eye.translEventVec.x += 1.0;
            }
            break;
          case SDLK_a:
            if (!evnt.key.repeat)
            {
              eye.translEventVec.x -= 1.0;
            }
            break;
          case SDLK_SPACE:
            if (!evnt.key.repeat)
            {
              eye.translEventVec.y += 1.0;
            }
            break;
          case SDLK_LSHIFT:
            if (!evnt.key.repeat)
            {
              eye.translEventVec.y -= 1.0;
            }
            break;
        }
        break;        
      case SDL_KEYUP:
        switch(evnt.key.keysym.sym)
        {
          case SDLK_w:
              eye.translEventVec.z -= 1.0;
            break;
          case SDLK_s:
              eye.translEventVec.z += 1.0;
            break;
          case SDLK_d:
              eye.translEventVec.x -= 1.0;
            break;
          case SDLK_a:
              eye.translEventVec.x += 1.0;
            break;
          case SDLK_SPACE:
              eye.translEventVec.y -= 1.0;
            break;
          case SDLK_LSHIFT:
              eye.translEventVec.y += 1.0;
            break;
        }
        break;
    }
  }
}

void ie::Engine::handleResize(int width, int height)
{
  float aspectRatio = float(width) / float(height);
  SDL_SetWindowSize(mainWindow, width, height);
  glViewport(0, 0, width, height);
  eye.setProjectionMatrix(glm::perspective(ie::FIELD_OF_VIEW,
                                           aspectRatio,
                                           ie::Z_NEAR, ie::Z_FAR));
}

bool ie::Engine::cleanup(void)
{
  vram.quit();
  am.quit();
  SDL_Quit();
  return true;
}
