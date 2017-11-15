//___|"ie_asset_manager.cpp"|___________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_asset_manager.h"

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "ie_material.h"
#include "ie_mesh.h"
#include "ie_shader.h"
#include "ie_terrain.h"
#include "ie_texture.h"

ie::AssetManager::AssetManager()
{
  newAssetId = 1;
}

//______________________________________________________________________________

//___|ASSIGNING AND MANAGING ASSET IDS|_________________________________________

unsigned int ie::AssetManager::assignAssetId(void)
{
  if (usedAssetIds.empty())
  {
    newAssetId++;
    return newAssetId;  
  }
  else
  {
    unsigned int id = usedAssetIds.front();
    usedAssetIds.pop_front();
    return id; 
  }
}

//______________________________________________________________________________

//___|LOADING ASSETS|___________________________________________________________


//Mesh
bool ie::AssetManager::load(Mesh* mesh)
{
  auto result = meshNameIdMap.find(mesh->getName());

  bool isAvailable = result == meshNameIdMap.end();
  
  if (isAvailable)
  {
    mesh->setAssetId(assignAssetId()); 
    meshAssets[mesh->getAssetId()] = mesh;
    meshNameIdMap[mesh->getName()] = mesh->getAssetId();
    return true;
  }
  else
  {
    std::cout << "Warning: Mesh name (" << mesh->getName() <<
    ") already exists." << std::endl; 
    delete mesh;
    return false;
  }
}


bool ie::AssetManager::deleteMesh(std::string name)
{
  auto results = meshNameIdMap.find(name);
  bool wasFound = results != meshNameIdMap.end();

  if (wasFound)
  {
    unsigned int id = results->second;
    meshNameIdMap.erase(name);
    delete meshAssets[id];
    meshAssets.erase(id);
    return true;
  }
  else
  {
    std::cout << "Warning: Mesh (" << name <<
    ") could not be found in manager."  << std::endl; 
    return false;
  }
}
bool ie::AssetManager::deleteMesh(unsigned int id)
{
  auto results = meshAssets.find(id);
  bool wasFound = results != meshAssets.end();

  if (wasFound)
  {
    std::string name = (results->second)->getName();
    meshNameIdMap.erase(name);
    delete meshAssets[id];
    meshAssets.erase(id);
    return true;
  }
  else
  {
    std::cout << "Warning: Mesh with id (" << id <<
    ") could not be found in manager."  << std::endl; 
    return false;
  }
}

//Materials
bool ie::AssetManager::load(Material* material)
{
  auto result = materialNameIdMap.find(material->getName());

  bool isAvailable = result == materialNameIdMap.end();
  
  if (isAvailable)
  {
    material->setAssetId(assignAssetId()); 
    materialAssets[material->getAssetId()] = material;
    materialNameIdMap[material->getName()] = material->getAssetId();
    return true;
  }
  else
  {
    std::cout << "Warning: Material name (" << material->getName() <<
    ") already exists." << std::endl; 
    delete material;
    return false;
  }
}


ie::Material* ie::AssetManager::getMaterial(std::string name)
{
  auto results = materialNameIdMap.find(name);
  bool isAvailable = results != materialNameIdMap.end();

  if (isAvailable)
  {
    return materialAssets[results->second];
  }
  else
  {
    std::cout << "Warning: Material (" << name <<
    ") could not be found in manager."  << std::endl; 
  }
}
ie::Material* ie::AssetManager::getMaterial(unsigned int id)
{
  auto results = materialAssets.find(id);
  bool isAvailable = results != materialAssets.end();

  if (isAvailable)
  {
    return materialAssets[id];
  }
  else
  {
    std::cout << "Warning: Material with id (" << id <<
    ") could not be found in manager."  << std::endl; 
  }
}


bool ie::AssetManager::deleteMaterial(std::string name)
{
  auto results = materialNameIdMap.find(name);
  bool wasFound = results != materialNameIdMap.end();

  if (wasFound)
  {
    unsigned int id = results->second;
    materialNameIdMap.erase(name);
    delete materialAssets[id];
    materialAssets.erase(id);
    return true;
  }
  else
  {
    std::cout << "Warning: Material (" << name <<
    ") could not be found in manager."  << std::endl; 
    return false;
  }
}
bool ie::AssetManager::deleteMaterial(unsigned int id)
{
  auto results = materialAssets.find(id);
  bool wasFound = results != materialAssets.end();

  if (wasFound)
  {
    std::string name = (results->second)->getName();
    materialNameIdMap.erase(name);
    delete materialAssets[id];
    materialAssets.erase(id);
    return true;
  }
  else
  {
    std::cout << "Warning: Material with id (" << id <<
    ") could not be found in manager."  << std::endl; 
    return false;
  }
}


//Textures
bool ie::AssetManager::load(Texture* texture)
{
  auto result = textureNameIdMap.find(texture->getName());

  bool isAvailable = result == textureNameIdMap.end();
  
  if (isAvailable)
  {
    texture->setAssetId(assignAssetId()); 
    textureAssets[texture->getAssetId()] = texture;
    textureNameIdMap[texture->getName()] = texture->getAssetId();
    return true;
  }
  else
  {
    std::cout << "Warning: Texture name (" << texture->getName() <<
    ") already exists." << std::endl; 
    delete texture;
    return false;
  }
}

//Terrain
bool ie::AssetManager::load(Terrain* terrain)
{
  auto result = terrainNameIdMap.find(terrain->getName());

  bool isAvailable = result == terrainNameIdMap.end();
  
  if (isAvailable)
  {
    terrain->setAssetId(assignAssetId()); 
    terrainAssets[terrain->getAssetId()] = terrain;
    terrainNameIdMap[terrain->getName()] = terrain->getAssetId();
    return true;
  }
  else
  {
    std::cout << "Warning: Terrian name (" << terrain->getName() <<
    ") already exists." << std::endl; 
    delete terrain;
    return false;
  }
}

//Shaders
bool ie::AssetManager::load(Shader* shader)
{
  auto result = shaderNameIdMap.find(shader->getName());

  bool isAvailable = result == shaderNameIdMap.end();
  
  if (isAvailable)
  {
    shader->setAssetId(assignAssetId()); 
    shaderAssets[shader->getAssetId()] = shader;
    shaderNameIdMap[shader->getName()] = shader->getAssetId();
    return true;
  }
  else
  {
    std::cout << "Warning: Shader name (" << shader->getName() <<
    ") already exists." << std::endl; 
    delete shader;
    return false;
  }
}
//___|CLEANING UP|______________________________________________________________

void ie::AssetManager::quit(void)
{
  for (auto it = meshAssets.begin(); it != meshAssets.end(); it++)
  {
    delete it->second;
  }
  for (auto it = materialAssets.begin(); it != materialAssets.end(); it++)
  {
    delete it->second;
  }
  for (auto it = textureAssets.begin(); it != textureAssets.end(); it++)
  {
    delete it->second;
  }
}

//______________________________________________________________________________
