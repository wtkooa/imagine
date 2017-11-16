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

#include "ie_log.h"
#include "ie_material.h"
#include "ie_mesh.h"
#include "ie_shader.h"
#include "ie_terrain.h"
#include "ie_texture.h"

ie::AssetManager::AssetManager()
{
  reset();
}


void ie::AssetManager::reset(void)
{
  log = NULL;
  newAssetId = 1;
}


void ie::AssetManager::setLog(Log* l) {log = l;}

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

    log->info("Loaded Mesh '%s'", mesh->getName().c_str());

    return true;
  }
  else
  {
    log->warning("Failed to load Mesh '%s' due to naming conflict",
                 mesh->getName().c_str());
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

    log->info("Deleted Mesh '%s'", name.c_str());

    return true;
  }
  else
  {
    log->warning("Request to delete Mesh '%s' failed; "
                 "Mesh could not be found", name.c_str());
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
    log->warning("Request to delete Mesh with id '%i' failed; "
                 "Mesh could not be found", id);
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

    log->info("Loaded Material '%s'",
              material->getName().c_str());

    return true;
  }
  else
  {
    log->warning("Failed to load Material '%s' due to naming conflict",
                 material->getName().c_str());
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
    log->warning("Request for Material '%s' failed; "
                 "Material not be found", name.c_str());
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
    log->warning("Request for Material with id '%i' failed; "
                 "Material could not be found", id);
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

    log->info("Deleted Material '%s'", name.c_str());

    return true;
  }
  else
  {
    log->warning("Request to delete Material '%s' failed; "
                 "Material could not be found", name.c_str());
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
    log->warning("Request to delete Material with id '%i' failed; "
                 "Material could not be found", id);
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

    log->info("Loaded Texture '%s'",
              texture->getName().c_str());

    return true;
  }
  else
  {
    log->warning("Failed to load Texture '%s' due to naming conflict",
                 texture->getName().c_str());
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

    log->info("Loaded Terrain '%s'",
              terrain->getName().c_str());

    return true;
  }
  else
  {
    log->warning("Failed to load Terrain '%s' due to naming conflict",
                 terrain->getName().c_str());
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

    log->info("Loaded Shader '%s'",
              shader->getName().c_str());

    return true;
  }
  else
  {
    log->warning("Failed to load Shader '%s' due to naming conflict",
                 shader->getName().c_str());
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

  log->info("Asset Manager Shutdown");
}

//______________________________________________________________________________
