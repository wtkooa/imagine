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
#include "ie_creation.h"
#include "ie_import.h"
#include "ie_material.h"
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
  log.info("Initializing the Imagine Engine v%i.%i.%i",
           IE_MAJOR_VERSION, IE_MINOR_VERSION, IE_PATCH_VERSION); 

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

  log.info("Initializing SDL");

  int result = SDL_Init(ie::REQUIRED_SDL_MODULES);
  bool sdlInitSuccess = (result == 0);

  if (sdlInitSuccess)
  {
    log.info("SDL modules successfully initialized");
  }
  else
  {
    log.warning("SDL modeules failed to initialize");
    log.warning("%s", SDL_GetError());
  }

  sdlConfigs.fetchSDLConfigs();
  
  log.info("SDL Compiled Version = %i.%i.%i",
           sdlConfigs.SDL_MAJOR_VERSION_COMPILED,
           sdlConfigs.SDL_MINOR_VERSION_COMPILED,
           sdlConfigs.SDL_PATCH_VERSION_COMPILED);
  log.info("SDL Linked Version = %i.%i.%i",
           sdlConfigs.SDL_MAJOR_VERSION_LINKED,
           sdlConfigs.SDL_MINOR_VERSION_LINKED,
           sdlConfigs.SDL_PATCH_VERSION_LINKED);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, ie::DEFAULT_GL_MAJOR_VERSION);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, ie::DEFAULT_GL_MINOR_VERSION);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  if (ie::SDL_MULTISAMPLING_ON)
  {
    log.info("SDL Multisampling: %i samples",
    ie::SDL_MULTISAMPLING_SAMPLE_AMOUNT);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,
                        ie::SDL_MULTISAMPLING_SAMPLE_AMOUNT);
  }
  else
  {
    log.info("SDL Multisampling: off");
  }

  mainWindow = SDL_CreateWindow(ie::WINDOW_TITLE.c_str(),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                ie::WINDOW_WIDTH, ie::WINDOW_HEIGHT,
                ie::SDL_MODE);
  
  if (mainWindow != NULL)
  {
    log.info("SDL window created successfully");
  }
  else
  {
    log.warning("SDL window creation failed");
    log.warning("%s", SDL_GetError());
  }

  log.info("SDL Initialized");
}


bool ie::Engine::initOpenGl(void)
{
  log.info("Initializing OpenGL");

  mainGlContext = SDL_GL_CreateContext(mainWindow);

  if (mainGlContext != NULL)
  {
    log.info("OpenGL context created successfully");
  }
  else
  {
    log.warning("OpenGL context creation failed");
    log.warning("%s", SDL_GetError());
  }

  sdlConfigs.fetchSDLGLConfigs();

  SDL_GL_SetSwapInterval(ie::LOCK_TO_LOCAL_FRAMERATE);

  openGlConfigs.fetchOpenGlConfigs(); 

  log.info("Engine Requested OpenGL Version = %i.%i", DEFAULT_GL_MAJOR_VERSION,
                                               DEFAULT_GL_MINOR_VERSION);
  log.info("SDL Given OpenGL Version = %i.%i", sdlConfigs.SDL_GL_MAJOR_VERSION,
                                           sdlConfigs.SDL_GL_MINOR_VERSION);
  log.info("Vendor = %s", openGlConfigs.LOCAL_GL_VENDOR.c_str());
  log.info("Renderer = %s", openGlConfigs.LOCAL_GL_RENDERER.c_str());
  log.info("Version = %s", openGlConfigs.LOCAL_GL_VERSION.c_str());
  log.info("Shading Version = %s", openGlConfigs.LOCAL_SHADING_LANGUAGE_VERSION.c_str());
  if (openGlConfigs.ANISOTROPIC_AVAILABLE)
  {
    log.info("Anisotropic Filtering: AVAILABLE");
  }
  else
  {
    log.info("Anisotropic Filtering: UNAVAILABLE");
  }

  glViewport(0, 0, ie::WINDOW_WIDTH, ie::WINDOW_HEIGHT);
  if (ie::WIREFRAME_ON) {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);}
  if (ie::DEPTHTEST_ON) {glEnable(GL_DEPTH_TEST);}
  if (ie::CULLFACE_ON) {glEnable(GL_CULL_FACE);}
  glClearColor(ie::DEFAULT_CLEAR_COLOR.r, ie::DEFAULT_CLEAR_COLOR.g,
               ie::DEFAULT_CLEAR_COLOR.b, ie::DEFAULT_CLEAR_COLOR.a);
  glClear(ie::ACTIVEBUFFERS);

  log.info("OpenGL Initialized");
}


bool ie::Engine::initAssets(void)
{

  am.setLog(&log);
  log.info("Asset Manager Initialized");

  local.setLog(&log);
  local.setLoadDestination(&am);
  log.info("Import Manager Initialized");

  editor.setLog(&log);
  editor.setLoadDestination(&am);
  log.info("Creation Manager Initialized");


  log.info("Initializing Assets");

  local.load("TreasureChest.obj");
  local.load("Cursor.obj");
  local.load("Face.obj");

  Material* mat = new Material();
  mat->setName("terrain_grass");

  editor.terrainEditor.generateTerrain();
  editor.terrainEditor.setName("Terra");
  editor.terrainEditor.applyPerlin(42.0f, 32.0f, 40.0f);
  editor.terrainEditor.smoothNormals();
  editor.terrainEditor.addMaterial(mat);
  editor.terrainEditor.loadTerrain();

  log.info("Assets Initialized");

  return true;
}


bool ie::Engine::initShaders(void)
{
  log.info("Initializing Shaders");

  local.load("Static", "ie_staticVShader330c.glsl",
             "ie_staticFShader330c.glsl");

  log.info("Shaders Initialized");
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
  ctrl.setLog(&log);
  ctrl.setWindow(mainWindow);
  ctrl.setEngineOnOffSwitch(&engineRun);
  log.info("Controller Initialized");
}



bool ie::Engine::run(void)
{
  engineRun = true;
  log.info("Engine Start");
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

bool ie::Engine::quit(void)
{
  //vram.quit();
  local.quit();
  editor.quit();
  am.quit();
  SDL_Quit();
  log.info("SDL Shutdown");
  log.info("End Program");
  log.info("End Log");
  return true;
}
