//___|"ie_engine.cpp"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_engine.h"

#include <iostream>
#include <map>
#include <string>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <SDL2/SDL.h>

#include "ie_assets.h"
#include "ie_asset_manager.h"
#include "ie_config.h"
#include "ie_const.h"
#include "ie_camera.h"
#include "ie_lighting.h"
#include "ie_messages.h"
#include "ie_packages.h"
#include "ie_render_engine.h"
#include "ie_scenegraph.h"
#include "ie_shader.h"
#include "ie_terrain.h"
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
  initSdl();
  initOpenGl();
  initLighting();
  initShaders();
  initAssets();
  initVram();
  initRenderers();
  initSceneGraph();
  initPlayer();
  initCamera();
  return true;
}


bool ie::Engine::initSdl(void)
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
}


bool ie::Engine::initOpenGl(void)
{
  openGlConfigs.fetchOpenGlConfigs(); 
  glViewport(0, 0, ie::WINDOW_WIDTH, ie::WINDOW_HEIGHT);
  if (ie::WIREFRAME_ON) {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);}
  if (ie::DEPTHTEST_ON) {glEnable(GL_DEPTH_TEST);}
  if (ie::CULLFACE_ON) {glEnable(GL_CULL_FACE);}
  glClearColor(ie::DEFAULT_CLEAR_COLOR.r, ie::DEFAULT_CLEAR_COLOR.g,
               ie::DEFAULT_CLEAR_COLOR.b, ie::DEFAULT_CLEAR_COLOR.a);
  glClear(ie::ACTIVEBUFFERS);
}


bool ie::Engine::initLighting(void)
{
  LightGenerator light;
  light.setName("light0");
  light.setPosVector(glm::vec3(0.0f, 40.0f, 0.0f));
  light.setGlobalAmbient(glm::vec3(0.1f, 0.1f, 0.1f));
  light.setLightAmbient(glm::vec3(0.1f, 0.1f, 0.1f));
  light.setLightSpecular(glm::vec3(0.5f, 0.5f, 0.5f));
  light.setLightDiffuse(glm::vec3(0.8f, 0.8f, 0.8f));
  light.setConstantFalloff(0.1f);
  light.setLinearFalloff(0.02f);
  light.setQuadraticFalloff(0.0f);
  ie::LightPackage lightPack = light.wrapLightPackage();
  am.unwrapPackage(lightPack);
  return true;
}


bool ie::Engine::initShaders(void)
{
  if (openGlConfigs.LOCAL_GLSL_VERSION_NUMERIC >= ie::GLSL_VERSION_450)
  {
    ie::ShaderProgramPackage statPack = compiler.compile("static",
                             "src/glsl/", "ie_staticVShader450.glsl",
                             "src/glsl/", "ie_staticFShader450.glsl");
    am.unwrapPackage(statPack);
    ie::ShaderProgramPackage terrainPack = compiler.compile("terrain",
                             "src/glsl/", "ie_terrainVShader450.glsl",
                             "src/glsl/", "ie_terrainFShader450.glsl");
    am.unwrapPackage(terrainPack);
  }
  else
  {
    ie::ShaderProgramPackage statPack = compiler.compile("static",
                             "src/glsl/", "ie_staticVShader130.glsl",
                             "src/glsl/", "ie_staticFShader130.glsl");
    am.unwrapPackage(statPack);
    ie::ShaderProgramPackage terrainPack = compiler.compile("terrain",
                             "src/glsl/", "ie_terrainVShader130.glsl",
                             "src/glsl/", "ie_terrainFShader130.glsl");
    am.unwrapPackage(terrainPack);
  }

  return true;
}


bool ie::Engine::initAssets(void)
{
  ie::WavefrontObjectFileReader objReader;  
  ie::WavefrontObjectFilePackage cursorPack = objReader.read("data/wavefront/",
                                                          "Cursor.obj");
  am.unwrapPackage(cursorPack);

  am.createEntity("Player", "Cursor", STATIC);
  ie::handle cursorBG = am.getHandle("material/CursorBG");
  (*cursorBG.material).usesLightFalloff = false;
  (*cursorBG.material).usesLightDiffuse = false;
  (*cursorBG.material).usesLightSpecular = false;
  (*cursorBG.material).usesLightAmbient = false;
  (*cursorBG.material).usesGlobalAmbient = false;

  ie::TerrainGenerator terrain(120);
  terrain.applyPerlin(42.0f, 32.0f, 40.0f);
  terrain.applyDemoBlends();
  terrain.addTexture("data/textures/", "001_grass.png");
  terrain.addTexture("data/textures/", "cobble.jpg");
  terrain.addTexture("data/textures/", "topsoil.jpg");
  terrain.addTexture("data/textures/", "sand.jpg");
  terrain.addTexture("data/textures/", "dirtrocks.jpg");
  terrain.addTexture("data/textures/", "cementDark.png");
  terrain.addTexture("data/textures/", "snow.jpg");
  terrain.addTexture("data/textures/", "lava.jpg");
  ie::TerrainPackage terPack = terrain.wrapTerrainPackage();
  am.unwrapPackage(terPack);
  am.createEntity("Terrain", "Terrain", TERRAIN);

  am.createQuickLists();
  return true;
}


bool ie::Engine::initVram(void)
{
  ie::AssetStatusToVramMessage statusMsg = am.sendAssetStatusToVramMessage();
  vram.receiveMessage(statusMsg);
  vram.setGlContext(mainGlContext);
  vram.createAndLoadVbos();
  return true;
}


bool ie::Engine::initRenderers(void)
{
}

bool ie::Engine::initSceneGraph(void)
{
  ie::AssetStatusToScenegraphMessage toSG = am.sendAssetStatusToScenegraphMessage();
  sg.receiveMessage(toSG);

  ie::EntityNode* terrain = new EntityNode("Terrain", "Terrain", TERRAIN);
  ie::EntityNode* cursor = new EntityNode("Player", "Cursor", STATIC);
  sg.root->addChild(terrain);
  terrain->addChild(cursor); 
  sg.update();
}


bool ie::Engine::initPlayer(void)
{
  player.setWindow(mainWindow);
  player.setPlayerEntity("Player");
  player.setPlayerPosition(glm::vec3(0.0f, 0.0f, 0.0f));
  player.setPlayerRotation(glm::vec3(0.0f, 0.0f, -1.0));
  ie::AssetStatusToPlayerMessage toPlayerMsg = am.sendAssetStatusToPlayerMessage();
  player.receiveMessage(toPlayerMsg);
}


bool ie::Engine::initCamera(void)
{
  return true;
}


bool ie::Engine::run(void)
{
  engineOn = true;
  while (engineOn) {
    handleEvents();
    handleUpdates();
    handleLogic();
    render();
  } 
  return true;
}


void ie::Engine::handleUpdates(void)
{
  frameClock.measure();

  ie::TimeStatusMessage timeStatusMsg = frameClock.sendTimeStatusMessage();
  eye.receiveMessage(timeStatusMsg);
  player.receiveMessage(timeStatusMsg);

  ie::AssetStatusToPlayerMessage toPlayerMsg = am.sendAssetStatusToPlayerMessage();
  player.receiveMessage(toPlayerMsg);

  player.update();

  ie::PlayerStatusToCameraMessage toCameraMsg = player.sendPlayerStatusToCameraMessage();
  eye.receiveMessage(toCameraMsg);

  eye.update();

  ie::AssetManagerInstructions assetInstrucMsg = player.sendAssetManagerInstructionsMessage();
  am.receiveMessage(assetInstrucMsg);

  am.update();

  ie::CameraStatusToRenderMessage cameraToRenderMsg = eye.sendCameraStatusToRenderMessage();
  re.receiveMessage(cameraToRenderMsg);

  ie::AssetStatusToVramMessage toVramMsg = am.sendAssetStatusToVramMessage();
  vram.receiveMessage(toVramMsg);

  ie::VramStatusToRenderMessage vramToRenderMsg  = vram.sendVramStatusToRenderMessage();
  re.receiveMessage(vramToRenderMsg);

  ie::AssetStatusToRenderMessage assetToRenderMsg = am.sendAssetStatusToRenderMessage();
  re.receiveMessage(assetToRenderMsg);
}


void ie::Engine::handleLogic(void)
{
  ie::handle player = am.getHandle("entity/Player");

  glm::mat4 rotMatrix = glm::rotate(glm::mat4(), glm::radians(0.5f),
                                    glm::vec3(0.0f, 1.0f, 0.0f));

  (*player.entity).rotationMatrix *= rotMatrix;
}


void ie::Engine::render(void)
{
  glClear(ACTIVEBUFFERS);

  re.render();

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
          player.rotateEventVec.x += evnt.motion.xrel;  
          player.rotateEventVec.y += evnt.motion.yrel;  
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
              player.toggleGrabMode();
            }
            break;
          case SDLK_w:
            if (!evnt.key.repeat)
            { 
              player.translEventVec.z += 1.0;
            }
            break;
          case SDLK_s:
            if (!evnt.key.repeat)
            {
              player.translEventVec.z -= 1.0;
            }
            break;
          case SDLK_d:
            if (!evnt.key.repeat)
            {
              player.translEventVec.x += 1.0;
            }
            break;
          case SDLK_a:
            if (!evnt.key.repeat)
            {
              player.translEventVec.x -= 1.0;
            }
            break;
          case SDLK_SPACE:
            if (!evnt.key.repeat)
            {
              player.translEventVec.y += 1.0;
            }
            break;
          case SDLK_LSHIFT:
            if (!evnt.key.repeat)
            {
              player.translEventVec.y -= 1.0;
            }
            break;
        }
        break;        
      case SDL_KEYUP:
        switch(evnt.key.keysym.sym)
        {
          case SDLK_w:
              player.translEventVec.z -= 1.0;
            break;
          case SDLK_s:
              player.translEventVec.z += 1.0;
            break;
          case SDLK_d:
              player.translEventVec.x -= 1.0;
            break;
          case SDLK_a:
              player.translEventVec.x += 1.0;
            break;
          case SDLK_SPACE:
              player.translEventVec.y -= 1.0;
            break;
          case SDLK_LSHIFT:
              player.translEventVec.y += 1.0;
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
                                           ie::NEAR_PLANE, ie::FAR_PLANE));
}


bool ie::Engine::cleanup(void)
{
  vram.quit();
  am.quit();
  SDL_Quit();
  return true;
}
