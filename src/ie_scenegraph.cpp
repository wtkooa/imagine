//___|"ie_scenegraph.cpp"|______________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_scenegraph.h"

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
  scale = glm::vec3(0.0f);
  transformation = glm::mat4();
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
  transformation = transl * rotate * scal;
  if (parentNode)
  {
    transformation = parentNode->transformation * transl * rotate * scal;
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

void ie::GraphNode::receiveMessage(ie::AssetStatusToScenegraphMessage msg)
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

void ie::SortTreeNode::receiveMessage(ie::AssetStatusToScenegraphMessage msg)
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
    toTerrain->sort(entity);
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

//Implement Bucket

//______________________________________________________________________________

//___|SCENEGRAPH IMPLEMENTATION|________________________________________________

ie::SceneGraph::SceneGraph()
{
  root = new GraphNode();
  sortTree = new SortTreeNode();
  root->setSortTreeRoot(sortTree);

  SortEntityTypeNode* entityType = new SortEntityTypeNode(); 
  sortTree->addChild(entityType);

  SortStaticTypeNode* staticType = new SortStaticTypeNode();
  entityType->addToStaticChild(staticType);
}

void ie::SceneGraph::receiveMessage(ie::AssetStatusToScenegraphMessage msg)
{
  root->receiveMessage(msg);
  sortTree->receiveMessage(msg);
}

void ie::SceneGraph::update(void)
{
  root->update();
}


