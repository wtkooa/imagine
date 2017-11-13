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
void ie::AssetManager::load(Mesh* mesh)
{
  auto result = meshNameIdMap.find(mesh->getName());

  bool isAvailable = result == meshNameIdMap.end();
  
  if (isAvailable)
  {
    mesh->setAssetId(assignAssetId()); 
    meshAssets[mesh->getAssetId()] = mesh;
    meshNameIdMap[mesh->getName()] = mesh->getAssetId();
  }
  else
  {
    std::cout << "Warning: Mesh name (" << mesh->getName() <<
    ") already exists." << std::endl; 
    delete mesh;
  }
}


//Materials
void ie::AssetManager::load(Material* material)
{
  auto result = materialNameIdMap.find(material->getName());

  bool isAvailable = result == materialNameIdMap.end();
  
  if (isAvailable)
  {
    material->setAssetId(assignAssetId()); 
    materialAssets[material->getAssetId()] = material;
    materialNameIdMap[material->getName()] = material->getAssetId();
  }
  else
  {
    std::cout << "Warning: Material name (" << material->getName() <<
    ") already exists." << std::endl; 
    delete material;
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


//Textures
void ie::AssetManager::load(Texture* texture)
{
  auto result = textureNameIdMap.find(texture->getName());

  bool isAvailable = result == textureNameIdMap.end();
  
  if (isAvailable)
  {
    texture->setAssetId(assignAssetId()); 
    textureAssets[texture->getAssetId()] = texture;
    textureNameIdMap[texture->getName()] = texture->getAssetId();
  }
  else
  {
    std::cout << "Warning: Texture name (" << texture->getName() <<
    ") already exists." << std::endl; 
    delete texture;
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
