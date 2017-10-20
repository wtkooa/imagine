//___|"ie_nodes.cpp"|___________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_nodes.h"

#include <iostream>
#include <map>
#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ie_assets.h"
#include "ie_config.h"
#include "ie_const.h"
#include "ie_messages.h"
#include "ie_vram.h"

//___|BASE GRAPH NODE IMPLEMENTATION|___________________________________________

std::map<unsigned int, ie::ModelAsset>* ie::GraphNode::models = NULL;
std::map<std::string, unsigned int>* ie::GraphNode::modelNameIdMap = NULL; 
std::map<unsigned int, ie::TerrainAsset>* ie::GraphNode::terrains = NULL;
std::map<std::string, unsigned int>* ie::GraphNode::terrainNameIdMap = NULL; 
std::map<unsigned int, ie::LightAsset>* ie::GraphNode::lights = NULL;
std::map<std::string, unsigned int>* ie::GraphNode::lightNameIdMap = NULL;
std::map<unsigned int, ie::MaterialAsset>* ie::GraphNode::materials = NULL;
std::map<std::string, unsigned int>* ie::GraphNode::materialNameIdMap = NULL; 
std::map<unsigned int, ie::RenderUnit>* ie::GraphNode::rus = NULL;
ie::SortTreeNode* ie::GraphNode::sortTreeRoot = NULL;
ie::PhysicsSort* ie::GraphNode::physicsTreeRoot = NULL;

ie::GraphNode::GraphNode()
{
  translation = glm::vec3(0.0f);
  rotation = glm::vec3(0.0f);
  scale = glm::vec3(1.0f);
  mtwMatrix = glm::mat4();
  parentNode = NULL;
  type = NONE;
}

ie::GraphNode::~GraphNode()
{
  for (auto it = children.begin(); it != children.end(); it++)
  {
    delete *it;
  }
}

void ie::GraphNode::addChild(GraphNode* child)
{
  child->parentNode = this; 
  children.push_back(child);
}

void ie::GraphNode::physics(void)
{
  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->physics();
  }
}
void ie::GraphNode::update(void)
{
  glm::mat4 transl = glm::translate(glm::mat4(), translation);
  glm::mat4 scal = glm::scale(glm::mat4(), scale);
  glm::mat4 rotX = glm::rotate(glm::mat4(), rotation.x,
                               glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 rotY = glm::rotate(glm::mat4(), rotation.y,
                               glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 rotZ = glm::rotate(glm::mat4(), rotation.z,
                               glm::vec3(0.0f, 0.0f, 1.0f));
  glm::mat4 rotate = rotX * rotY * rotZ;
  mtwMatrix = transl * rotate * scal;
  if (parentNode)
  {
    mtwMatrix = parentNode->mtwMatrix * mtwMatrix;
  }
  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->update();
  }
}

void ie::GraphNode::render(void)
{
  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->render();
  }
}

void ie::GraphNode::setSortTreeRoot(SortTreeNode* root)
{
  sortTreeRoot = root; 
}

void ie::GraphNode::setPhysicsTreeRoot(PhysicsSort* root)
{
  physicsTreeRoot = root; 
}

void ie::GraphNode::sendToPhysics(PhysicsPointers physicsUnit)
{
  physicsTreeRoot->sort(physicsUnit);
}

void ie::GraphNode::receiveMessage(ie::AssetStatusMessage msg)
{
  models = msg.models;
  modelNameIdMap = msg.modelNameIdMap;
  terrains = msg.terrains;
  terrainNameIdMap = msg.terrainNameIdMap;
  lights = msg.lights;
  lightNameIdMap = msg.lightNameIdMap;
  materials = msg.materials;
  materialNameIdMap = msg.materialNameIdMap;
  rus = msg.rus;
}

//______________________________________________________________________________

//___|ENTITY GRAPH NODE IMPLEMENTATION|_________________________________________

ie::EntityNode::EntityNode()
{
  name = "None";
  type = NONE;
  assetId = 0;
  hidden = false;
  usesGlobalAmbient = true;
  usesLightAmbient = true;
  usesLightDiffuse = true;
  usesLightSpecular = true;
  usesLightFalloff = true;
  usesPhysics = true;
  collidable = true;
}

ie::EntityNode::EntityNode(std::string entityName,
                           std::string assetName,
                           NodeType assetType)
{
  name = entityName;
  type = assetType;
  if (type == STATIC)
  {
    assetId = (*modelNameIdMap)[assetName];
  }
  else if (type == TERRAIN)
  {
    assetId = (*terrainNameIdMap)[assetName];
  }
  hidden = false;
  usesGlobalAmbient = true;
  usesLightAmbient = true;
  usesLightDiffuse = true;
  usesLightSpecular = true;
  usesLightFalloff = true;
  usesPhysics = true;
  collidable = true;
}

void ie::EntityNode::physics(void)
{
  if (usesPhysics == true)
  {
    PhysicsPointers unit;
    unit.type = type;
    unit.entity = this;
    sendToPhysics(unit);
  }
}

void ie::EntityNode::render(void)
{
  if (hidden == false)
  {
    RenderPointers unit;
    unit.type = type;
    unit.entity = this;
    sortTreeRoot->sort(unit);
  }
  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->render();
  }
}

//______________________________________________________________________________

//___|PLAYER NODE IMPLEMENTATION|_______________________________________________

ie::PlayerNode::PlayerNode()
{
  type = PLAYER;
  upVector = ie::UP_VECTOR;
  moveSpeed = ie::DEFAULT_PLAYER_MOVESPEED;
  turnSpeed = ie::DEFAULT_PLAYER_TURNSPEED;
  rotation = glm::vec3(0.0f, 0.0f, -1.0f);
}

void ie::PlayerNode::physics(void)
{
  PhysicsPointers unit;
  unit.type = PLAYER;
  unit.player = this;
  sendToPhysics(unit);
}

void ie::PlayerNode::render(void)
{
  RenderPointers unit;
  unit.type = PLAYER;
  unit.player = this;
  sortTreeRoot->sort(unit);
  linkedEntity->render();
  linkedCamera->render();
}

//______________________________________________________________________________

//___|CAMERA NODE IMPLEMENTATION|_______________________________________________

ie::CameraNode::CameraNode()
{
  type = CAMERA;
  upVector = ie::UP_VECTOR;
  lookSpeed = ie::DEFAULT_CAMERA_LOOKSPEED;
  offset = glm::vec3(0.0f, 2.0f, 0.0f);
  lookVector = glm::vec3(0.0f, 0.0f, -1.0f);
  projectionMatrix = glm::perspective(ie::FIELD_OF_VIEW,
                                      ie::ASPECT_RATIO,
                                      ie::NEAR_PLANE, ie::FAR_PLANE);
}

void ie::CameraNode::render(void)
{
  RenderPointers unit;
  unit.type = CAMERA;
  unit.camera = this;
  sortTreeRoot->sort(unit);
}

//______________________________________________________________________________

//___|SORTING BUCKET TREE IMPLEMENTATION|_______________________________________

std::map<unsigned int, ie::ModelAsset>* ie::SortTreeNode::models = NULL;
std::map<std::string, unsigned int>* ie::SortTreeNode::modelNameIdMap = NULL; 
std::map<unsigned int, ie::TerrainAsset>* ie::SortTreeNode::terrains = NULL;
std::map<std::string, unsigned int>* ie::SortTreeNode::terrainNameIdMap = NULL; 
std::map<unsigned int, ie::LightAsset>* ie::SortTreeNode::lights = NULL;
std::map<std::string, unsigned int>* ie::SortTreeNode::lightNameIdMap = NULL;
std::map<unsigned int, ie::MaterialAsset>* ie::SortTreeNode::materials = NULL;
std::map<std::string, unsigned int>* ie::SortTreeNode::materialNameIdMap = NULL; 
std::map<unsigned int, ie::RenderUnit>* ie::SortTreeNode::rus = NULL;


void ie::SortTreeNode::addChild(SortTreeNode* child)
{
  children.push_back(child);
}

void ie::SortTreeNode::sort(RenderPointers unit)
{
  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->sort(unit);
  }
}

void ie::SortTreeNode::receiveMessage(ie::AssetStatusMessage msg)
{
  models = msg.models;
  modelNameIdMap = msg.modelNameIdMap;
  terrains = msg.terrains;
  terrainNameIdMap = msg.terrainNameIdMap;
  lights = msg.lights;
  lightNameIdMap = msg.lightNameIdMap;
  materials = msg.materials;
  materialNameIdMap = msg.materialNameIdMap;
  rus = msg.rus;
}


void ie::SortTypeNode::sort(RenderPointers unit)
{
  if (unit.type == TERRAIN)
  {
    unit.ta = &(*terrains)[unit.entity->assetId];
    toTerrain->sort(unit);
  }
  else if (unit.type == STATIC)
  {
    toStatic->sort(unit);
  }
  else if (unit.type == PLAYER)
  {
    toPlayer->sort(unit);
  }
  else if (unit.type == CAMERA)
  {
    toCamera->sort(unit);
  }
}

void ie::SortStaticTypeNode::sort(RenderPointers unit)
{
  ModelAsset* model = &(*models)[unit.entity->assetId];
  std::vector<unsigned int>* ruList = &model->renderUnits;
  for (auto it = ruList->begin(); it != ruList->end(); it++)
  {
    RenderUnit* ru = &(*rus)[*it];
    RenderPointers rps;
    rps.entity = unit.entity;
    rps.ru = ru;
    if (ru->dataFormat == VN)
    {
      toMaterialed->sort(rps);
    }
    else if (ru->dataFormat == VTN)
    {
      toTextured->sort(rps);
    }
  }
}


ie::SortBucket::SortBucket()
{
  nextBucket = NULL;
}


void ie::SortBucket::sort(RenderPointers rps)
{
  renderUnits.push_back(rps);
}


void ie::SortBucket::clear(void)
{
  renderUnits.clear();
  if (nextBucket != NULL)
  {
    nextBucket->clear();
  }
}


ie::SortBucket* ie::SortBucket::getNextBucket(void) {return nextBucket;}
void ie::SortBucket::setRenderInstructions(ie::RenderInstructions instruc)
{
  instructions = instruc;
}
ie::RenderInstructions* ie::SortBucket::getRenderInstructions(void)
{
  return &instructions;
}
std::vector<ie::RenderPointers>* ie::SortBucket::getRenderUnits(void)
{
  return &renderUnits;
}

//______________________________________________________________________________

//___|PHYSICS BUCKETS|__________________________________________________________

void ie::PhysicsSort::sort(ie::PhysicsPointers physicsUnit)
{
  if (physicsUnit.type == TERRAIN)
  {
    toTerrain->sort(physicsUnit);
  }
  else if (physicsUnit.type == PLAYER)
  {
    toPlayer->sort(physicsUnit);
  }
  else if (physicsUnit.type == STATIC)
  {
    toStatic->sort(physicsUnit);
  }
}

ie::PhysicsBucket::PhysicsBucket()
{
  nextBucket = NULL;
}

void ie::PhysicsBucket::clear(void)
{
  physicsUnits.clear();
  if (nextBucket != NULL)
  {
    nextBucket->clear();
  }
}

void ie::PhysicsBucket::sort(ie::PhysicsPointers physicsUnit)
{
  physicsUnits.push_back(physicsUnit);
}

ie::PhysicsBucket* ie::PhysicsBucket::getNextBucket(void)
{
  return nextBucket;
}
