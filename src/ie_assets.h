#ifndef IE_ASSETS_H
#define IE_ASSETS_H

//___|"ie_assets.h"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <string>
#include <map>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "ie_vram.h"
#include "ie_packages.h"

namespace ie
{


  enum EntityType{NONE, STATIC, TERRAIN, WATER};
  
  class Entity
  {
    public:
    unsigned int assetId;
    std::string name;
    EntityType type;
    unsigned int modelId;
    unsigned int terrainId;
    bool hidden;
    glm::mat4 translationMatrix;
    glm::mat4 rotationMatrix;
    glm::mat4 scaleMatrix;
    bool usesGlobalAmbient;
    bool usesLightAmbient;
    bool usesLightDiffuse;
    bool usesLightSpecular;
    bool usesLightFalloff;
  };

  class StaticQuickListElement
  {
    public:
    std::vector<short> renderUnitList;
  };


  class ShaderAsset
  {
    public:
    std::string name;
    unsigned int assetId;
    GLuint programId;
    GLuint vertexShaderId;
    GLuint fragmentShaderId;
    std::map<std::string, GlslUniformPackage> uniforms;
  };


  class RenderUnit
  {
    public:
    unsigned int assetId;
    unsigned int material;
    VboDataFormat dataFormat;
    std::string shaderProgram;
    unsigned int heapIndexOffset;
    unsigned int vertexAmount;
    unsigned int vramLocation;
    bool hidden;
  };


  class ModelAsset
  {
    public:
    unsigned int assetId;
    std::string name;
    std::string filename; 
    std::string filepath;
    std::vector<RenderUnit> renderUnits;
    bool tobeVramLoaded;
    bool vramLoaded;
  };


  class MaterialAsset
  {
    public:
    unsigned int assetId; 
    std::string name;
    std::string filename;
    std::string filepath;
    float shininess;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emission;
    bool usesGlobalAmbient;
    bool usesLightAmbient;
    bool usesLightDiffuse;
    bool usesLightSpecular;
    bool usesLightFalloff;
    float opticalDensity;
    float dissolve;
    int illum;
    bool containsTexture;
    unsigned int ambientMapId;
    unsigned int diffuseMapId; 
    unsigned int specularMapId;
    unsigned int highlightMapId;
    unsigned int alphaMapId;
    unsigned int bumpMapId;
  };


  class TextureAsset
  {
    public:
    unsigned int assetId;
    GLuint textureId;
    std::string filename;
    std::string filepath;
    std::string name;
    TextureType textureType; 
    bool tobeVramLoaded;
    bool vramLoaded;
  };


  class LightAsset
  {
    public:
    unsigned int assetId;
    std::string name;
    glm::vec3 posVector;
    glm::vec3 globalAmbient;
    glm::vec3 lightAmbient;
    glm::vec3 lightSpecular;
    glm::vec3 lightDiffuse;
    float constantFalloff;
    float linearFalloff;
    float quadraticFalloff;
  };

  class TerrainAsset
  {
    public:
    unsigned int assetId;
    std::string name;
    short dim;
    float unitSize;
    std::vector<unsigned int> textureIds;
    unsigned int vertexHeapOffset;
    unsigned int normalHeapOffset;
    unsigned int colorHeapOffset;
    unsigned int blendHeapOffset;
    unsigned int indexHeapOffset;
    unsigned int indexHeapAmount;
    unsigned int vramArrayLocation;
    unsigned int vramVertexAmount;
    unsigned int vramIndexLocation;
    unsigned int vramIndexAmount;
    bool tobeVramLoaded;
    bool vramLoaded;
    float shininess;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emission;
  };

  union handle 
  {
    std::map<unsigned int, Entity>* entities;
    std::map<unsigned int, ModelAsset>* models;
    std::map<unsigned int, MaterialAsset>* materials;
    std::map<unsigned int, TextureAsset>* textures;
    std::map<GLuint, ShaderAsset>* shaders;
    std::map<unsigned int, LightAsset>* lights;
    std::map<unsigned int, TerrainAsset>* terrains;

    Entity* entity;
    ModelAsset* model;
    MaterialAsset* material;
    TextureAsset* texture;
    ShaderAsset* shader;
    RenderUnit* ru;
    GlslUniformPackage* uniform;
    LightAsset* light;
    TerrainAsset* terrain;
  };

}

#endif
