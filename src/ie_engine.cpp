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
#include "ie_communication.h"
#include "ie_const.h"
#include "ie_controller.h"
#include "ie_editor.h"
#include "ie_enum.h"
#include "ie_import.h"
#include "ie_material.h"
#include "ie_messages.h"
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
  quit();
}


bool ie::Engine::init(void)
{
  nexus.addConnection(IE_ENGINE_ICOM_HANDLE, this);
  initLog();

  LogMsg msg(IE_LOG_INFO_CMD);
  msg.fString("Initializing the Imagine Engine v%i.%i.%i",
           IE_MAJOR_VERSION, IE_MINOR_VERSION, IE_PATCH_VERSION); 
  txMsg(&msg);

  initNexus();
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


bool ie::Engine::initLog(void)
{
  log.connectNexus(&nexus);
  log.init();
}


bool ie::Engine::initNexus(void)
{
  nexus.addSubscriber(&log);
  log.addSubscription(&nexus);
}


bool ie::Engine::initSdl(void)
{
  LogMsg info(IE_LOG_INFO_CMD);
  LogMsg warning(IE_LOG_WARNING_CMD);
  info.fString("Initializing SDL");
  txMsg(&info);


  int result = SDL_Init(ie::REQUIRED_SDL_MODULES);
  bool sdlInitSuccess = (result == 0);

  if (sdlInitSuccess)
  {
    info.fString("SDL modules successfully initialized");
    txMsg(&info);
  }
  else
  {
    warning.fString("SDL modeules failed to initialize");
    txMsg(&warning);
    warning.fString("%s", SDL_GetError());
    txMsg(&warning);
  }

  sdlConfigs.fetchSDLConfigs();
  
  info.fString("SDL Compiled Version = %i.%i.%i",
               sdlConfigs.SDL_MAJOR_VERSION_COMPILED,
               sdlConfigs.SDL_MINOR_VERSION_COMPILED,
               sdlConfigs.SDL_PATCH_VERSION_COMPILED);
  txMsg(&info);
  info.fString("SDL Linked Version = %i.%i.%i",
               sdlConfigs.SDL_MAJOR_VERSION_LINKED,
               sdlConfigs.SDL_MINOR_VERSION_LINKED,
               sdlConfigs.SDL_PATCH_VERSION_LINKED);
  txMsg(&info);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, ie::DEFAULT_GL_MAJOR_VERSION);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, ie::DEFAULT_GL_MINOR_VERSION);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  if (ie::SDL_MULTISAMPLING_ON)
  {
    info.fString("SDL Multisampling: %i samples",
    ie::SDL_MULTISAMPLING_SAMPLE_AMOUNT);
    txMsg(&info);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,
                        ie::SDL_MULTISAMPLING_SAMPLE_AMOUNT);
  }
  else
  {
    info.fString("SDL Multisampling: off");
    txMsg(&info);
  }

  mainWindow = SDL_CreateWindow(ie::WINDOW_TITLE.c_str(),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                ie::WINDOW_WIDTH, ie::WINDOW_HEIGHT,
                ie::SDL_MODE);
  
  if (mainWindow != NULL)
  {
    info.fString("SDL window created successfully");
    txMsg(&info);
  }
  else
  {
    warning.fString("SDL window creation failed");
    txMsg(&warning);
    warning.fString("%s", SDL_GetError());
    txMsg(&warning);
  }

  info.fString("SDL Initialized");
  txMsg(&info);
}


bool ie::Engine::initOpenGl(void)
{
  LogMsg info(IE_LOG_INFO_CMD);
  LogMsg warning(IE_LOG_WARNING_CMD);
  info.fString("Initializing OpenGL");
  txMsg(&info);

  mainGlContext = SDL_GL_CreateContext(mainWindow);

  if (mainGlContext != NULL)
  {
    info.fString("OpenGL context created successfully");
    txMsg(&info);
  }
  else
  {
    warning.fString("OpenGL context creation failed");
    txMsg(&warning);
    warning.fString("%s", SDL_GetError());
    txMsg(&warning);
  }

  sdlConfigs.fetchSDLGLConfigs();

  SDL_GL_SetSwapInterval(ie::LOCK_TO_LOCAL_FRAMERATE);

  openGlConfigs.fetchOpenGlConfigs(); 

  info.fString("Engine Requested OpenGL Version = %i.%i",
               DEFAULT_GL_MAJOR_VERSION,
               DEFAULT_GL_MINOR_VERSION);
  txMsg(&info);
  info.fString("SDL Given OpenGL Version = %i.%i",
               sdlConfigs.SDL_GL_MAJOR_VERSION,
               sdlConfigs.SDL_GL_MINOR_VERSION);
  txMsg(&info);
  info.fString("Vendor = %s", openGlConfigs.LOCAL_GL_VENDOR.c_str());
  txMsg(&info);
  info.fString("Renderer = %s", openGlConfigs.LOCAL_GL_RENDERER.c_str());
  txMsg(&info);
  info.fString("Version = %s", openGlConfigs.LOCAL_GL_VERSION.c_str());
  txMsg(&info);
  info.fString("Shading Version = %s",
               openGlConfigs.LOCAL_SHADING_LANGUAGE_VERSION.c_str());
  txMsg(&info);
  if (openGlConfigs.ANISOTROPIC_AVAILABLE)
  {
    info.fString("Anisotropic Filtering: AVAILABLE");
    txMsg(&info);
  }
  else
  {
    info.fString("Anisotropic Filtering: UNAVAILABLE");
    txMsg(&info);
  }

  glViewport(0, 0, ie::WINDOW_WIDTH, ie::WINDOW_HEIGHT);
  if (ie::WIREFRAME_ON) {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);}
  if (ie::DEPTHTEST_ON) {glEnable(GL_DEPTH_TEST);}
  if (ie::CULLFACE_ON) {glEnable(GL_CULL_FACE);}
  glClearColor(ie::DEFAULT_CLEAR_COLOR.r, ie::DEFAULT_CLEAR_COLOR.g,
               ie::DEFAULT_CLEAR_COLOR.b, ie::DEFAULT_CLEAR_COLOR.a);
  glClear(ie::ACTIVEBUFFERS);

  info.fString("OpenGL Initialized");
  txMsg(&info);
}


bool ie::Engine::initAssets(void)
{
  LogMsg info(IE_LOG_INFO_CMD);

  am.connectNexus(&nexus);
  am.init();
  info.fString("Asset Manager Initialized");
  txMsg(&info);

  import.connectNexus(&nexus);
  import.init();
  import.setLoadDestination(&am);
  info.fString("Import Manager Initialized");
  txMsg(&info);

  edit.setLoadDestination(&am);
  info.fString("Creation Manager Initialized");
  txMsg(&info);

  info.fString("Initializing Assets");
  txMsg(&info);

  import.load("TreasureChest.obj");
  import.load("Cursor.obj");
  import.load("Face.obj");

  Material* mat = new Material();
  mat->setName("terrain_grass");

  edit.terrain.generate();
  edit.terrain.setName("Terra");
  edit.terrain.applyPerlin(42.0f, 32.0f, 40.0f);
  edit.terrain.smoothNormals();
  edit.terrain.addMaterial(mat);
  edit.terrain.loadTerrain();

  info.fString("Assets Initialized");
  txMsg(&info);

  return true;
}


bool ie::Engine::initShaders(void)
{
  LogMsg info(IE_LOG_INFO_CMD);
  info.fString("Initializing Shaders");
  txMsg(&info);

  import.load("Static", "ie_staticVShader330c.glsl",
             "ie_staticFShader330c.glsl");

  info.fString("Shaders Initialized");
  txMsg(&info);
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
  ctrl.connectNexus(&nexus);
  ctrl.init();
  ctrl.setWindow(mainWindow);

  LogMsg info(IE_LOG_INFO_CMD);
  info.fString("Controller Initialized");
  txMsg(&info);
}



bool ie::Engine::run(void)
{
  LogMsg info(IE_LOG_INFO_CMD);
  info.fString("Engine Start");
  txMsg(&info);

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


void ie::Engine::rxMsg(Imessage* msg)
{
  switch (msg->type)
  {
    case IE_SYSTEM_MSG:
      handleSystemMsg(static_cast<SystemMsg*>(msg));
    break;
  }
}


void ie::Engine::handleSystemMsg(SystemMsg* msg)
{
  switch (msg->command)
  {
    case IE_SYSTEM_SHUTDOWN_CMD:
      engineRun = false;
    break;
  }
}

bool ie::Engine::quit(void)
{
  LogMsg info(IE_LOG_INFO_CMD);

  //vram.quit();
  import.quit();
  edit.quit();
  am.quit();
  SDL_Quit();
  info.fString("SDL Shutdown");
  txMsg(&info);
  info.fString("End Program");
  txMsg(&info);
  info.fString("End Log");
  txMsg(&info);
  return true;
}
