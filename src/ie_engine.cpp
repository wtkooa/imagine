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
#include "ie_compiler.h"
#include "ie_config.h"
#include "ie_const.h"
#include "ie_controller.h"
#include "ie_lighting.h"
#include "ie_messages.h"
#include "ie_nodes.h"
#include "ie_packages.h"
#include "ie_physics_engine.h"
#include "ie_render_engine.h"
#include "ie_scenegraph.h"
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
  initShaders();
  initAssets();
  initVram();
  initRenderers();
  initSceneGraph();
  initPhysics();
  initController();
  return true;
}


bool ie::Engine::initSdl(void)
{
  SDL_Init(ie::REQUIRED_SDL_MODULES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, ie::DEFAULT_GL_MAJOR_VERSION);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, ie::DEFAULT_GL_MINOR_VERSION);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  int major;
  int minor;
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
  std::cout << "Imagine OpenGL Default Version: " << major << " " << minor << std::endl;

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  if (ie::SDL_MULTISAMPLING_ON)
  {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,
                        ie::SDL_MULTISAMPLING_SAMPLE_AMOUNT);
  }
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


bool ie::Engine::initShaders(void)
{

  GlslCompiler compiler;

  ie::ShaderProgramPackage statPack = compiler.compile("static",
                           "src/glsl/", "ie_staticVShader330c.glsl",
                           "src/glsl/", "ie_staticFShader330c.glsl");
  am.unwrapPackage(statPack);
  ie::ShaderProgramPackage terrainPack = compiler.compile("terrain",
                           "src/glsl/", "ie_terrainVShader330c.glsl",
                           "src/glsl/", "ie_terrainFShader330c.glsl");
  am.unwrapPackage(terrainPack);
  return true;
}


bool ie::Engine::initAssets(void)
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

  ie::WavefrontObjectFileReader objReader;  
  ie::WavefrontObjectFilePackage cursorPack = objReader.read("data/wavefront/",
                                                          "Cursor.obj");
  am.unwrapPackage(cursorPack);

  ie::WavefrontObjectFilePackage treePack = objReader.read("data/wavefront/",
                                                           "AcaciaTree.obj");
  am.unwrapPackage(treePack);

  ie::WavefrontObjectFilePackage signPack = objReader.read("data/wavefront/",
                                                           "Face.obj");
  am.unwrapPackage(signPack);

  ie::WavefrontObjectFilePackage chestPack = objReader.read("data/wavefront/",
                                                           "TreasureChest.obj");
  am.unwrapPackage(chestPack);

  ie::WavefrontObjectFilePackage pinePack = objReader.read("data/wavefront/",
                                                           "PineTree.obj");
  am.unwrapPackage(pinePack);

  ie::handle cursorBG = am.getHandle("material/CursorBG");
  (*cursorBG.material).usesLightFalloff = false;
  (*cursorBG.material).usesLightDiffuse = false;
  (*cursorBG.material).usesLightSpecular = false;
  (*cursorBG.material).usesLightAmbient = false;
  (*cursorBG.material).usesGlobalAmbient = false;

  ie::TerrainGenerator terrain(120, 2.0f);
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

  return true;
}


bool ie::Engine::initVram(void)
{
  ie::AssetStatusMessage assetStatusMsg = am.sendAssetStatusMessage();
  vram.receiveMessage(assetStatusMsg);
  vram.setGlConfig(&openGlConfigs);
  vram.createVao();
  vram.createAndLoadVbos();
  return true;
}


bool ie::Engine::initRenderers(void)
{
  ie::AssetStatusMessage assetStatusMsg = am.sendAssetStatusMessage();
  re.receiveMessage(assetStatusMsg);
  re.setShader("static", STATIC_SHADER);
  re.setShader("terrain", TERRAIN_SHADER);
}

bool ie::Engine::initSceneGraph(void)
{
  ie::AssetStatusMessage assetStatusMsg = am.sendAssetStatusMessage();
  sg.receiveMessage(assetStatusMsg);

  ie::TerrainNode* terrain = new TerrainNode("Terrain", "Terrain");
  ie::StaticNode* cursor = new StaticNode("Cursor", "Player");
  ie::StaticNode* tree = new StaticNode("Acacia", "AcaciaTree");
  ie::StaticNode* sign = new StaticNode("Face", "Face");
  ie::StaticNode* chest = new StaticNode("TreasureChest", "Chest");
  ie::StaticNode* lid = new StaticNode("TreasureChestLid", "Lid");
  ie::StaticNode* pinetree = new StaticNode("PineTree", "PineTree");
  sign->translation = glm::vec3(5.0f, 0.0f, 5.0f);
  sign->rotation = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
  sign->usesLightDiffuse = false;
  sign->usesLightSpecular = false;
  sign->usesLightFalloff = false;
  sign->usesLightAmbient = false;
  sign->usesCullFace = false;
  tree->translation = glm::vec3(12.0f, -3.2f, 12.0f);
  ie::PlayerNode* player = new PlayerNode();
  ie::CameraNode* camera = new CameraNode();

  sg.root->addChild(terrain);
  terrain->addChild(tree);
  terrain->addChild(sign);
  terrain->addChild(chest);
  terrain->addChild(pinetree);
  chest->addChild(lid);
  chest->translation = glm::vec3(11.5f, -3.2f, 11.0f);
  chest->rotation = glm::vec3(0.0f, glm::radians(210.0f), 0.0f);
  pinetree->translation = glm::vec3(5.0f, -3.0f, 11.0f);
  sg.root->addChild(player);
  player->linkedCamera = camera;
  player->linkedEntity = cursor;
  player->translation = glm::vec3(1.0f, 0.0f, 1.0f);
  sg.update();
}

bool ie::Engine::initPhysics(void)
{
  ie::AssetStatusMessage assetStatusMsg = am.sendAssetStatusMessage();
  fizx.receiveMessage(assetStatusMsg);

  ie::GraphStatusMessage graphStatusMsg = sg.sendGraphStatusMessage();
  fizx.receiveMessage(graphStatusMsg);
  fizx.setController(&ctrl);
}

bool ie::Engine::initController(void)
{
  ctrl.setWindow(mainWindow);
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
  fizx.receiveMessage(timeStatusMsg);

  ie::AssetStatusMessage assetStatusMsg = am.sendAssetStatusMessage();
  vram.receiveMessage(assetStatusMsg);

  ie::VramStatusToRenderMessage vramToRenderMsg  = vram.sendVramStatusToRenderMessage();
  re.receiveMessage(vramToRenderMsg);

  re.receiveMessage(assetStatusMsg);

  sg.physics();
  fizx.update();
  sg.update();
  sg.render();

  ie::GraphStatusMessage graphStatusMsg = sg.sendGraphStatusMessage();
  re.receiveMessage(graphStatusMsg);
}


void ie::Engine::handleLogic(void)
{
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
          ctrl.rotateEventVec.x += evnt.motion.xrel;  
          ctrl.rotateEventVec.y += evnt.motion.yrel;  
        }
        break;
      case SDL_MOUSEWHEEL:
        if (evnt.wheel.y == 1)
        {
          ctrl.scrollEvent += evnt.wheel.y;
        }
        else if (evnt.wheel.y == -1)
        {
          ctrl.scrollEvent += evnt.wheel.y;
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
              ctrl.toggleGrabMode();
            }
            break;
          case SDLK_c:
            if (!evnt.key.repeat)
            {
              ctrl.togglePlayerMode();
            }
            break;
          case SDLK_w:
            if (!evnt.key.repeat)
            { 
              ctrl.translEventVec.z += 1.0;
            }
            break;
          case SDLK_s:
            if (!evnt.key.repeat)
            {
              ctrl.translEventVec.z -= 1.0;
            }
            break;
          case SDLK_d:
            if (!evnt.key.repeat)
            {
              ctrl.translEventVec.x += 1.0;
            }
            break;
          case SDLK_a:
            if (!evnt.key.repeat)
            {
              ctrl.translEventVec.x -= 1.0;
            }
            break;
          case SDLK_SPACE:
            if (!evnt.key.repeat)
            {
              ctrl.translEventVec.y += 1.0;
            }
            break;
          case SDLK_LSHIFT:
            if (!evnt.key.repeat)
            {
              ctrl.translEventVec.y -= 1.0;
            }
            break;
        }
        break;        
      case SDL_KEYUP:
        switch(evnt.key.keysym.sym)
        {
          case SDLK_w:
              ctrl.translEventVec.z -= 1.0;
            break;
          case SDLK_s:
              ctrl.translEventVec.z += 1.0;
            break;
          case SDLK_d:
              ctrl.translEventVec.x -= 1.0;
            break;
          case SDLK_a:
              ctrl.translEventVec.x += 1.0;
            break;
          case SDLK_SPACE:
              ctrl.translEventVec.y -= 1.0;
            break;
          case SDLK_LSHIFT:
              ctrl.translEventVec.y += 1.0;
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
  sg.updateAspectRatio(aspectRatio);
}


bool ie::Engine::cleanup(void)
{
  vram.quit();
  am.quit();
  SDL_Quit();
  return true;
}
