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
#include "ie_definitions.h"

namespace ie
{

  class CreateVboMessage
  {
    public:
    std::map<unsigned int, ModelAsset>* models;
    std::map<unsigned int, TextureAsset>* textures; 
    std::vector<glm::vec4>* vHeap;
    std::vector<glm::vec3>* tHeap;
    std::vector<glm::vec3>* nHeap;
    std::vector<glm::ivec4>* iHeap;
  };

  class RenderAssetMessage 
  {
    public:
    ShaderProgramAsset* shaderProgram;
    std::vector<QuickListElement>* quickList;
    std::map<unsigned int, MaterialAsset>* materials;
    std::map<unsigned int, ModelAsset>* models;
  };

  class RenderMemoryMessage
  {
    public:
    std::map<unsigned int, std::vector<VboRenderUnitLocation>>* memMap;
    VboPair* vboPair;    
    std::string formatType;
  }; 

  class RenderCameraMessage
  {
    public:
    glm::vec3 cameraPos;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
  };

  class RenderLightMessage
  {
    public:
    glm::vec3 globalAmbient;
    glm::vec3 posVector;
    glm::vec3 lightAmbient;
    glm::vec3 lightSpecular;
    glm::vec3 lightDiffuse;
    float constantFalloff;
    float linearFalloff;
    float quadraticFalloff;
  };

}

#endif
