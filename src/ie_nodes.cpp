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

ie::GraphNode::GraphNode()
{
  translation = glm::vec3(0.0f);
  rotation = glm::vec3(0.0f);
  scale = glm::vec3(1.0f);
  transformationMatrix = glm::mat4();
  parentNode = NULL;
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
  child->setParentNode(this); 
  children.push_back(child);
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
  transformationMatrix = transl * rotate * scal;
  if (parentNode)
  {
    transformationMatrix = parentNode->transformationMatrix * transformationMatrix;
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

void ie::GraphNode::setParentNode(GraphNode* parent)
{
  parentNode = parent;
}

void ie::GraphNode::setSortTreeRoot(SortTreeNode* root)
{
  sortTreeRoot = root; 
}

void ie::GraphNode::setTranslation(glm::vec3 transl)
{
  translation = transl;
}

glm::mat4 ie::GraphNode::getTransformationMatrix(void)
{
  return transformationMatrix;
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
}

ie::EntityNode::EntityNode(std::string entityName,
                           std::string assetName,
                           EntityType assetType)
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
}

void ie::EntityNode::render(void)
{
  if (hidden == false)
  {
    sortTreeRoot->sort(this);
  }
  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->render();
  }
}

ie::EntityType ie::EntityNode::getType(void) {return type;}
unsigned int ie::EntityNode::getAssetId(void) {return assetId;}

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

void ie::SortTreeNode::sort(EntityNode* entity)
{
  for (auto it = children.begin(); it != children.end(); it++)
  {
    (*it)->sort(entity);
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

void ie::SortTreeNode::sort(RenderPointers) {}

void ie::SortEntityTypeNode::sort(EntityNode* entity)
{
  if (entity->getType() == TERRAIN)
  {
    TerrainAsset* terrain = &(*terrains)[entity->getAssetId()];
    RenderPointers rps;
    rps.entity = entity;
    rps.ta = terrain;
    toTerrain->sort(rps);
  }
  else if (entity->getType() == STATIC)
  {
    toStatic->sort(entity);
  }
}

void ie::SortEntityTypeNode::addToTerrainChild(SortTreeNode* node) {toTerrain = node;}
void ie::SortEntityTypeNode::addToStaticChild(SortTreeNode* node) {toStatic = node;}

void ie::SortStaticTypeNode::sort(EntityNode* entity)
{
  ModelAsset* model = &(*models)[entity->getAssetId()];
  std::vector<unsigned int>* ruList = &model->renderUnits;
  for (auto it = ruList->begin(); it != ruList->end(); it++)
  {
    RenderUnit* ru = &(*rus)[*it];
    RenderPointers rps;
    rps.entity = entity;
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

void ie::SortStaticTypeNode::addToStaticMaterialedChild(SortTreeNode* node) {toMaterialed = node;}
void ie::SortStaticTypeNode::addToStaticTexturedChild(SortTreeNode* node) {toTextured = node;}

ie::SortBucket::SortBucket()
{
  link = NULL;
}

void ie::SortBucket::sort(RenderPointers rps)
{
  renderUnits.push_back(rps);
}

void ie::SortBucket::clear(void)
{
  renderUnits.clear();
  if (link != NULL)
  {
    link->clear();
  }
}

void ie::SortBucket::setNextBucket(SortBucket* newLink) {link = newLink;}
ie::SortBucket* ie::SortBucket::getNextBucket(void) {return link;}
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