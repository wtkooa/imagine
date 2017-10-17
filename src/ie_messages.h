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
#include <set>
#include <string>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "ie_assets.h"
#include "ie_vram.h"

namespace ie
{

  class AssetStatusMessage
  {
    public:
    std::map<unsigned int, RenderUnit>* rus;
    std::map<unsigned int, ModelAsset>* models;
    std::map<std::string, unsigned int>* modelNameIdMap;
    std::map<unsigned int, TextureAsset>* textures; 
    std::map<std::string, unsigned int>* textureNameIdMap;
    std::map<unsigned int, MaterialAsset>* materials;
    std::map<std::string, unsigned int>* materialNameIdMap;
    std::map<unsigned int, TerrainAsset>* terrains;
    std::map<std::string, unsigned int>* terrainNameIdMap;
    std::map<GLuint, ShaderAsset>* shaders;
    std::map<std::string, GLuint>* shaderNameIdMap;
    std::map<unsigned int, LightAsset>* lights;
    std::map<std::string, unsigned int>* lightNameIdMap;
    std::vector<glm::vec4>* vHeap;
    std::vector<glm::vec3>* tHeap;
    std::vector<glm::vec3>* nHeap;
    std::vector<glm::vec3>* cHeap;
    std::vector<glm::uvec2>* bHeap;
    std::vector<glm::ivec4>* iHeap;
  };

  class VramStatusToRenderMessage
  {
    public:
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


  class TimeStatusMessage
  {
    public:
    float frameDelta;
  };

  class PlayerStatusToCameraMessage
  {
    public:
    glm::vec3 playerPosition;
    glm::vec3 playerRotation;
    glm::vec3 translEventVec;
    glm::vec2 rotateEventVec;    
    std::string mode;
  };

  class AssetManagerInstruction 
  {
    public:
    std::string command;
    unsigned int id;
  };

  class AssetManagerInstructions
  {
    public:
    std::vector<AssetManagerInstruction> instructions;
  };


  class SortBucket; //Forward Declaration
  class GraphStatusToRenderMessage
  {
    public:
    SortBucket* bucket;
  };

  class Controller; //Forward Declaration
  class ControllerStatusMessage
  {
    public:
    Controller* controller;
    glm::vec3* translEventVec;
    glm::vec2* rotateEventVec;    
  };

}

#endif
