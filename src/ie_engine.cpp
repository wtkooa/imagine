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

#include "ie_asset_manager.h"
#include "ie_glsl.h"
#include "ie_config.h"
#include "ie_const.h"
#include "ie_controller.h"
#include "ie_import.h"
//#include "ie_nodes.h"
//#include "ie_physics_engine.h"
//#include "ie_render_engine.h"
//#include "ie_scenegraph.h"
//#include "ie_terrain.h"
#include "ie_time.h"
#include "ie_utils.h"
//#include "ie_vram.h"
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
  initAssets();
  initShaders();
  //initVram();
  //initRenderers();
  //initSceneGraph();
  //initPhysics();
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
  local.load("Static", "ie_staticVShader330c.glsl",
             "ie_staticFShader330c.glsl");
  return true;
}

bool ie::Engine::initAssets(void)
{
  local.setLoadDestination(&am);

  local.load("TreasureChest.obj");
  local.load("Cursor.obj");
  local.load("Face.obj");

  return true;
}

/*
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

*/
bool ie::Engine::initController(void)
{
  ctrl.setWindow(mainWindow);
  ctrl.setEngineOnOffSwitch(&engineRun);
}



bool ie::Engine::run(void)
{
  engineRun = true;
  while (engineRun) {
    update();
    render();
  } 
  return true;
}



void ie::Engine::update(void)
{
  clock.update();
  ctrl.update();

  //sg.physics();
  //fizx.update();
  //sg.update();
  //sg.render();
}


void ie::Engine::render(void)
{
  glClear(ACTIVEBUFFERS);

  //re.render();

  SDL_GL_SwapWindow(mainWindow);
}

bool ie::Engine::cleanup(void)
{
  //vram.quit();
  local.quit();
  am.quit();
  SDL_Quit();
  return true;
}
