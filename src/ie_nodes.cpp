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

//___|SCENEGRAPH BASE NODE|_____________________________________________________

std::map<unsigned int, ie::ModelAsset>* ie::GraphNode::models = NULL;
std::map<std::string, unsigned int>* ie::GraphNode::modelNameIdMap = NULL; 
std::map<unsigned int, ie::TerrainAsset>* ie::GraphNode::terrains = NULL;
std::map<std::string, unsigned int>* ie::GraphNode::terrainNameIdMap = NULL; 
std::map<unsigned int, ie::LightAsset>* ie::GraphNode::lights = NULL;
std::map<std::string, unsigned int>* ie::GraphNode::lightNameIdMap = NULL;
std::map<unsigned int, ie::MaterialAsset>* ie::GraphNode::materials = NULL;
std::map<std::string, unsigned int>* ie::GraphNode::materialNameIdMap = NULL; 
std::map<unsigned int, ie::RenderUnit>* ie::GraphNode::rus = NULL;
ie::RenderTreeNode* ie::GraphNode::renderTreeRoot = NULL;
ie::PhysicsTreeNode* ie::GraphNode::physicsTreeRoot = NULL;
float ie::GraphNode::aspectRatio = ie::ASPECT_RATIO;

ie::GraphNode::GraphNode()
{
  parentNode = NULL;
  type = NONE_NODE;
  mtwMatrix = glm::mat4();
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

void ie::GraphNode::setSortTreeRoot(RenderTreeNode* root)
{
  renderTreeRoot = root; 
}

void ie::GraphNode::setPhysicsTreeNodeRoot(PhysicsTreeNode* root)
{
  physicsTreeRoot = root; 
}

void ie::GraphNode::setAspectRatio(float ratio)
{
  aspectRatio = ratio;
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

//___|BASE ENTITY GRAPH NODE|___________________________________________________

ie::EntityNode::EntityNode()
{
  name = "none";
  hidden = false;
  translation = glm::vec3(0.0f);
  rotation = glm::vec3(0.0f);
  scale = glm::vec3(1.0f);
}


void ie::EntityNode::update(void)
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

//______________________________________________________________________________

//___|TERRAIN NODE|_____________________________________________________________

ie::TerrainNode::TerrainNode(std::string asset, std::string n)
{
  type = TERRAIN_NODE;
  name = n;
  assetId = (*terrainNameIdMap)[asset];
  usesGlobalAmbient = true;
  usesLightAmbient = true;
  usesLightDiffuse = true;
  usesLightSpecular = true;
  usesLightFalloff = true;
  collidable = true;
}


void ie::TerrainNode::physics(void)
{
  if (collidable == true)
  {
    NodePacket packet;
    packet.type = type;
    packet.node.terrain = this;
    packet.asset.ta = &(*terrains)[assetId];
    physicsTreeRoot->sort(packet);
  }
  
  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->physics();
  }
}


void ie::TerrainNode::render(void)
{
  if (hidden == false)
  {
    NodePacket packet;
    packet.type = type;
    packet.node.terrain = this;
    packet.asset.ta = &(*terrains)[assetId];
    renderTreeRoot->sort(packet);
  }

  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->render();
  }
}

//______________________________________________________________________________

//___|STATIC NODE|______________________________________________________________

ie::StaticNode::StaticNode(std::string asset, std::string n)
{
  type = STATIC_NODE;
  name = n;
  assetId = (*modelNameIdMap)[asset];
  usesGlobalAmbient = true;
  usesLightAmbient = true;
  usesLightDiffuse = true;
  usesLightSpecular = true;
  usesLightFalloff = true;
  usesCullFace = true;
  usesPhysics = false;
}


void ie::StaticNode::render(void)
{
  if (hidden == false)
  {
    NodePacket packet;
    packet.type = type;
    packet.node.stat = this;
    renderTreeRoot->sort(packet);
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
  type = PLAYER_NODE;
  upVector = ie::UP_VECTOR;
  moveSpeed = ie::DEFAULT_PLAYER_MOVESPEED;
  turnSpeed = ie::DEFAULT_PLAYER_TURNSPEED;
  translation = glm::vec3(0.0f);
  rotation = glm::vec3(0.0f, 0.0f, -1.0f);
}

void ie::PlayerNode::physics(void)
{
  NodePacket packet;
  packet.type = type;
  packet.node.player = this;
  physicsTreeRoot->sort(packet);

  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->physics();
  }
}

void ie::PlayerNode::update(void)
{
  linkedCamera->update();
  
  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->update();
  }
}

void ie::PlayerNode::render(void)
{
  NodePacket packet;
  packet.type = type;
  packet.node.player = this;
  renderTreeRoot->sort(packet);
  linkedEntity->render();
  linkedCamera->render();

  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->render();
  }
}


//______________________________________________________________________________

//___|CAMERA NODE IMPLEMENTATION|_______________________________________________

ie::CameraNode::CameraNode()
{
  type = CAMERA_NODE;
  upVector = ie::UP_VECTOR;
  lookSpeed = ie::DEFAULT_CAMERA_LOOKSPEED;
  firstPersonOffset = glm::vec3(0.0f, 2.0f, 0.0f);
  thirdPersonOffset = glm::vec3(0.0f, 0.0f, 1.0f);
  translation = glm::vec3(0.0f);
  distance = 10.0f;
  lookVector = glm::vec3(0.0f, 0.0f, -1.0f);
  fieldOfView = ie::FIELD_OF_VIEW;
  currentAspectRatio = aspectRatio;
  nearPlane = ie::NEAR_PLANE;
  farPlane = ie::FAR_PLANE;
  projectionMatrix = glm::perspective(fieldOfView,
                                      currentAspectRatio,
                                      nearPlane, farPlane);
}

void ie::CameraNode::render(void)
{
  NodePacket packet;
  packet.type = type;
  packet.node.camera = this;
  renderTreeRoot->sort(packet);

  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->render();
  }
}

void ie::CameraNode::update(void)
{
  if (currentAspectRatio != aspectRatio)
  {
    currentAspectRatio = aspectRatio;
    projectionMatrix = glm::perspective(fieldOfView,
                                        currentAspectRatio,
                                        nearPlane, farPlane);

    for (auto it = children.begin(); it != children.end(); it++)
    {
      (*it)->update();
    }
  }
}

//______________________________________________________________________________

//___|RENDER TREE|______________________________________________________________

ie::RenderState::RenderState()
{
  cullFace = true;
}

std::map<unsigned int, ie::ModelAsset>* ie::RenderTreeNode::models = NULL;
std::map<std::string, unsigned int>* ie::RenderTreeNode::modelNameIdMap = NULL; 
std::map<unsigned int, ie::TerrainAsset>* ie::RenderTreeNode::terrains = NULL;
std::map<std::string, unsigned int>* ie::RenderTreeNode::terrainNameIdMap = NULL; 
std::map<unsigned int, ie::LightAsset>* ie::RenderTreeNode::lights = NULL;
std::map<std::string, unsigned int>* ie::RenderTreeNode::lightNameIdMap = NULL;
std::map<unsigned int, ie::MaterialAsset>* ie::RenderTreeNode::materials = NULL;
std::map<std::string, unsigned int>* ie::RenderTreeNode::materialNameIdMap = NULL; 
std::map<unsigned int, ie::RenderUnit>* ie::RenderTreeNode::rus = NULL;


void ie::RenderTreeNode::sort(NodePacket packet)
{
    child->sort(packet);
}

void ie::RenderTreeNode::receiveMessage(ie::AssetStatusMessage msg)
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


void ie::SortTypeNode::sort(NodePacket packet)
{
  if (packet.type == TERRAIN_NODE)
  {
    packet.asset.ta = &(*terrains)[packet.node.terrain->assetId];
    toTerrain->sort(packet);
  }
  else if (packet.type == STATIC_NODE)
  {
    toStatic->sort(packet);
  }
  else if (packet.type == PLAYER_NODE)
  {
    toPlayer->sort(packet);
  }
  else if (packet.type == CAMERA_NODE)
  {
    toCamera->sort(packet);
  }
}

void ie::SortStaticTypeNode::sort(NodePacket packet)
{
  ModelAsset* model = &(*models)[packet.node.stat->assetId];
  std::vector<unsigned int>* ruList = &model->renderUnits;
  for (auto it = ruList->begin(); it != ruList->end(); it++)
  {
    RenderUnit* ru = &(*rus)[*it];
    NodePacket np;
    np.node.stat = packet.node.stat;
    np.asset.ru = ru;
    if (ru->dataFormat == VN)
    {
      toMaterialed->sort(np);
    }
    else if (ru->dataFormat == VTN)
    {
      toTextured->sort(np);
    }
  }
}


void ie::SortCullFaceNode::sort(NodePacket packet)
{
  if (packet.node.stat->usesCullFace)
  {
    toCulled->sort(packet);
  }
  else
  {
    toUnCulled->sort(packet);
  }
}


ie::RenderBucket::RenderBucket()
{
  nextBucket = NULL;
  type = NONE_RENDER;
}


void ie::RenderBucket::sort(NodePacket packet)
{
  packets.push_back(packet);
}


void ie::RenderBucket::clear(void)
{
  packets.clear();
  if (nextBucket != NULL)
  {
    nextBucket->clear();
  }
}


ie::RenderBucket* ie::RenderBucket::getNextBucket(void) {return nextBucket;}
void ie::RenderBucket::setRenderState(ie::RenderState s)
{
  state = s;
}
ie::RenderState* ie::RenderBucket::getRenderState(void)
{
  return &state;
}
std::vector<ie::NodePacket>* ie::RenderBucket::getPackets(void)
{
  return &packets;
}

//______________________________________________________________________________

//___|PHYSICS BUCKETS|__________________________________________________________

void ie::PhysicsTreeNode::sort(ie::NodePacket packet)
{
  if (packet.type == TERRAIN_NODE)
  {
    toTerrain->sort(packet);
  }
  else if (packet.type == PLAYER_NODE)
  {
    toPlayer->sort(packet);
  }
  else if (packet.type == STATIC_NODE)
  {
    toStatic->sort(packet);
  }
}

ie::PhysicsBucket::PhysicsBucket()
{
  nextBucket = NULL;
}

void ie::PhysicsBucket::clear(void)
{
  packets.clear();
  if (nextBucket != NULL)
  {
    nextBucket->clear();
  }
}

void ie::PhysicsBucket::sort(ie::NodePacket packet)
{
  packets.push_back(packet);
}

ie::PhysicsBucket* ie::PhysicsBucket::getNextBucket(void)
{
  return nextBucket;
}
