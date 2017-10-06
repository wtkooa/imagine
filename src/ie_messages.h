#ifndef IE_MESSAGES_H
#define IE_MESSAGES_H

//___|"ie_messages.h"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________
#include <map>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "ie_assets.h"
#include "ie_memory.h"

namespace ie
{

  class AssetStatusToVramMessage
  {
    public:
    std::map<unsigned int, ModelAsset>* models;
    std::map<unsigned int, TextureAsset>* textures; 
    std::map<unsigned int, MaterialAsset>* materials;
    std::map<unsigned int, TerrainAsset>* terrains;
    std::vector<glm::vec4>* vHeap;
    std::vector<glm::vec3>* tHeap;
    std::vector<glm::vec3>* nHeap;
    std::vector<glm::vec3>* cHeap;
    std::vector<glm::uvec2>* bHeap;
    std::vector<glm::ivec4>* iHeap;
  };


  class AssetStatusToRenderMessage 
  {
    public:
    std::map<unsigned int, Entity>* entities;
    std::map<unsigned int, MaterialAsset>* materials;
    std::map<unsigned int, ModelAsset>* models;
    std::map<GLuint, ShaderProgramAsset>* shaders;
    std::map<std::string, GLuint>* shaderNameIdMap;
    std::map<unsigned int, LightAsset>* lights;
    std::map<std::string, unsigned int>* lightNameIdMap;
    std::map<unsigned int, TerrainAsset>* terrains;
    std::map<std::string, unsigned int>* terrainNameIdMap;
    std::vector<StaticQuickListElement>* staticVList;
    std::vector<StaticQuickListElement>* staticVNList;
    std::vector<StaticQuickListElement>* staticVTNList;
    std::vector<unsigned int>* terrainVTNCBList;
  };


  class VramStatusToRenderMessage
  {
    public:
    std::map<unsigned int, std::vector<StaticRenderUnitLocation>>* staticMemoryMap;
    std::map<unsigned int, TerrainRenderUnitLocation>* terrainIndexMemoryMap;
    VboPair* vPair;
    VboPair* vnPair;
    VboPair* vtnPair;
    VboPair* vtncbPair;
    VboPair* terrainIndexPair;
  }; 


  class CameraStatusToRenderMessage
  {
    public:
    glm::vec3 cameraPos;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
  };


  class TimeStatusToCameraMessage
  {
    public:
    float frameDelta;
  };

}

#endif
