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
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "ie_assets.h"
#include "ie_vram.h"
#include "ie_messages.h"
#include "ie_packages.h"
#include "ie_utils.h"


ie::AssetManager::AssetManager()
{
  newAssetId = 0;
}


//___|CARRYING OUT ACCUMULATED INSTRUCTIONS|____________________________________

void ie::AssetManager::update(void)
{
  for (unsigned int n = 0; n < instructions.size(); n++)
  {
    ie::AssetManagerInstruction instruc = instructions[n];
    instructions.erase(instructions.begin());

    if (instruc.command == "hide entity")
    {
    }
    else if (instruc.command == "unhide entity")
    {
    }
  }
  instructions.clear();
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

//___|LOADING VERTEX HEAP DATA|_________________________________________________

template <class T, class S>
unsigned int ie::AssetManager::pushToHeap(T* heap, S* data)
{
  unsigned int offset = heap->size();
  heap->insert(heap->end(), data->begin(), data->end());
  return offset;
}

//______________________________________________________________________________

//___|UNWRAPPING INCOMING PACKAGES|_____________________________________________

//UNWRAP WAVEFRONT OBJ FILE PACKAGES
void ie::AssetManager::unwrapPackage(ie::WavefrontObjectFilePackage filePackage)
{
  unsigned int vertexHeapOffset = pushToHeap(&vertexHeap, &filePackage.v);

  unsigned int textureCoordinateHeapOffset = pushToHeap(&textureCoordinateHeap,
                                             &filePackage.vt);
  unsigned int normalVectorHeapOffset = pushToHeap(&normalVectorHeap,
                                                   &filePackage.vn);
  unsigned int indexAmount = filePackage.f.size();

  for (int n = 0; n < indexAmount; n++)
  {
    filePackage.f[n].x += vertexHeapOffset - 1;
    if (filePackage.f[n].y != -1)
    {
      filePackage.f[n].y += textureCoordinateHeapOffset - 1;
    }
    if (filePackage.f[n].z != -1)
    {
      filePackage.f[n].z += normalVectorHeapOffset - 1;
    }
  }
  unsigned int indexOffset = pushToHeap(&indexHeap, &filePackage.f);

  unsigned int materialFileAmount = filePackage.materialFilePackages.size();
  for (int nFile = 0; nFile < materialFileAmount; nFile++)
  {
    unwrapPackage(filePackage.materialFilePackages[nFile]);
  }

  for (auto it = filePackage.objects.begin();
       it != filePackage.objects.end(); it++)
  {
    ie::ModelAsset asset;
    asset.name = it->second;
    bool modelNameTaken = modelNameIdMap.count(asset.name) == 1;
    if (modelNameTaken)
    {
      std::cout << "Warning: Model with name '" << asset.name <<
      "' already exists." << std::endl;
      continue;
    }
    asset.assetId = assignAssetId();
    modelNameIdMap[asset.name] = asset.assetId;
    asset.filename = filePackage.filename;
    asset.filepath = filePackage.filepath;
    unsigned int objectIndexBegin = it->first;
    unsigned int objectIndexEnd = 0;
    auto nextIt = it;
    std::advance(nextIt, 1);
    if (nextIt != filePackage.objects.end())
    {
      objectIndexEnd = nextIt->first;
    }
    else {
      objectIndexEnd = indexAmount;
    }
    for (auto mgIt = filePackage.materialGroups.begin();
         mgIt != filePackage.materialGroups.end(); mgIt++)
    {
      unsigned int groupIndexBegin = mgIt->first;
      unsigned int heapSampleIndex = indexOffset + groupIndexBegin;
      glm::ivec4 sampleFaceElement = indexHeap[heapSampleIndex];
      std::string usingMaterialName = mgIt->second;
      if (groupIndexBegin >= objectIndexBegin &&
          groupIndexBegin < objectIndexEnd)
      {
        unsigned int groupIndexEnd = 0;
        auto nextMgIt = mgIt;
        std::advance(nextMgIt, 1);
        if (nextMgIt != filePackage.materialGroups.end())
        {
          groupIndexEnd = nextMgIt->first;
        }
        else
        {
          groupIndexEnd = objectIndexEnd;
        }
        unsigned int groupVertexAmount = groupIndexEnd - groupIndexBegin;
        ie::RenderUnit renderUnit;
        if (sampleFaceElement.y == -1 && sampleFaceElement.z == -1)
        {
          renderUnit.dataFormat = VboDataFormat::V;
        }
        else if (sampleFaceElement.y == -1)
        {
          renderUnit.dataFormat = VboDataFormat::VN;
        }
        else
        {
          renderUnit.dataFormat = VboDataFormat::VTN;
        }
        renderUnit.assetId = assignAssetId();
        renderUnit.material = materialNameIdMap[usingMaterialName];
        renderUnit.heapIndexOffset = indexOffset + groupIndexBegin;
        renderUnit.vertexAmount = groupVertexAmount;
        renderUnit.hidden = false;
        renderUnit.tobeVramLoaded = true;
        renderUnit.vramLoaded = false;
        asset.renderUnits.push_back(renderUnit.assetId);
        renderUnits[renderUnit.assetId] = renderUnit;
      }
    }
    modelAssets[asset.assetId] = asset;
  } 
}


//UNWRAP WAVEFRONT MTL FILE PACKAGES
void ie::AssetManager::unwrapPackage(
                       ie::WavefrontMaterialFilePackage filePackage)
{
  unsigned int materialAmount = filePackage.materials.size();
  for (int nMaterial = 0; nMaterial < materialAmount; nMaterial++)
  {
    unwrapPackage(filePackage.materials[nMaterial]); 
  }
}


GLuint ie::AssetManager::unwrapPackage(ie::WavefrontMaterialPackage package)
{
  MaterialAsset asset;
  asset.name = package.name;
  bool materialNameTaken = materialNameIdMap.count(asset.name) == 1;
  if (materialNameTaken)
  {
    std::cout << "Warning: Material with name '" << asset.name <<
    "' already exists. Engine will use original." << std::endl;
    return 0;
  }
  asset.assetId = assignAssetId();
  asset.filename = package.filename;
  asset.filepath = package.filepath;
  asset.shininess = package.shininess;
  asset.ambient = package.ambient;
  asset.diffuse = package.diffuse;
  asset.specular = package.specular;
  asset.emission = package.emission;
  asset.opticalDensity = package.opticalDensity;
  asset.dissolve = package.dissolve;
  asset.usesGlobalAmbient = true;
  asset.usesLightAmbient = true;
  asset.usesLightDiffuse = true;
  asset.usesLightSpecular = true;
  asset.usesLightFalloff = true;
  materialNameIdMap[asset.name] = asset.assetId;
  unsigned int texturePackageAmount = package.texturePackages.size();
  if (texturePackageAmount > 0)
  {
    asset.containsTexture = true;
  }
  else
  {
    asset.containsTexture = false;
  }
  for (int ntex = 0; ntex < texturePackageAmount; ntex++)
  {
    TexturePackage tex = package.texturePackages[ntex];
    switch (tex.type)
    {
      case DIFFUSE_MAP:
        asset.diffuseMapId = unwrapPackage(tex);
        break;
      case BUMP_MAP:
        asset.bumpMapId = unwrapPackage(tex);
        break;
      case ALPHA_MAP:
        asset.alphaMapId = unwrapPackage(tex);
        break;
      case AMBIENT_MAP:
        asset.ambientMapId = unwrapPackage(tex);
        break;
      case SPECULAR_MAP:
        asset.specularMapId = unwrapPackage(tex);
        break;
      case HIGHLIGHT_MAP:
        asset.highlightMapId = unwrapPackage(tex);
        break;
    }
  }
  materialAssets[asset.assetId] = asset;
}


//UNWRAP TEXTURE PACKAGES
GLuint ie::AssetManager::unwrapPackage(ie::TexturePackage package)
{
  ie::TextureAsset asset;
  std::string filename = package.filename;
  int tokenAmount = ie::countTokens(filename, '/') - 1; 
  std::string name = ie::split(filename, '/', tokenAmount); 
  name = ie::split(name, '.', 0);
  asset.name = name; 
  bool textureNameTaken = textureNameIdMap.count(asset.name) == 1;
  if (textureNameTaken)
  {
    std::cout << "Warning: Texture with name '" << asset.name <<
    "' already exists. Engine will use original." << std::endl;
    return textureNameIdMap[asset.name];
  }
  GLuint textureId;
  glGenTextures(1, &textureId);
  asset.textureId = textureId;
  asset.assetId = assignAssetId();
  asset.filename = package.filename;
  asset.filepath = package.filepath;
  asset.textureType = package.type;
  asset.tobeVramLoaded = true;
  asset.vramLoaded = false;
  asset.mipmapped = package.mipmapped; 
  asset.anisotropy = package.anisotropy;
  asset.repeating = package.repeating;
  textureNameIdMap[asset.name] = asset.assetId;
  textureAssets[asset.assetId] = asset;
  return asset.assetId;
}


//UNWRAP SHADER PACKAGES 
void ie::AssetManager::unwrapPackage(ie::ShaderProgramPackage package)
{
  ie::ShaderAsset asset;
  asset.name = package.name;
  bool shaderProgramNameTaken = shaderNameIdMap.count(package.name) == 1;
  if (shaderProgramNameTaken)
  {
    std::cout << "Warning: shader program with name '" << asset.name <<
    "' already exists. Engine will use original." << std::endl;
    return;
  }
  asset.assetId = assignAssetId();
  asset.programId = package.programId;
  asset.vertexShaderId = package.vertexShaderId;
  asset.fragmentShaderId = package.fragmentShaderId;
  asset.uniforms = package.uniforms;
  shaderNameIdMap[asset.name] = asset.assetId;
  shaderAssets[asset.assetId] = asset;
}


//UNWRAP LIGHT PACKAGES
void ie::AssetManager::unwrapPackage(ie::LightPackage package)
{
  ie::LightAsset asset;
  asset.name = package.name;
  bool lightNameTaken = lightNameIdMap.count(package.name) == 1;
  if (lightNameTaken)
  {
    std::cout << "Warning: light with name '" << asset.name <<
    "' already exists. Engine will use original." << std::endl;
    return;
  }
  asset.assetId = assignAssetId();
  asset.posVector = package.posVector;
  asset.globalAmbient = package.globalAmbient;
  asset.lightAmbient = package.lightAmbient;
  asset.lightSpecular = package.lightSpecular;
  asset.lightDiffuse = package.lightDiffuse;
  asset.constantFalloff = package.constantFalloff;
  asset.linearFalloff = package.linearFalloff;
  asset.quadraticFalloff = package.quadraticFalloff;
  lightNameIdMap[asset.name] = asset.assetId;
  lightAssets[asset.assetId] = asset;
}

//UNWRAP TERRAIN PACKAGES
void ie::AssetManager::unwrapPackage(ie::TerrainPackage package)
{
  TerrainAsset asset;  
  asset.name = package.name;
  bool terrainNameTaken = terrainNameIdMap.count(package.name) == 1;
  if (terrainNameTaken)
  {
    std::cout << "Warning: terrain with name '" << asset.name <<
    "' already exists. Engine will use original." << std::endl;
    return;
  }

  for (short nTex = 0; nTex < package.textures.size(); nTex++)
  {
    asset.textureIds.push_back(unwrapPackage(package.textures[nTex])); 
  }

  asset.assetId = assignAssetId();
  asset.dim = package.dim;
  asset.unitSize = package.unitSize;
  asset.vertexHeapOffset = pushToHeap(&vertexHeap, &package.vertices);
  asset.normalHeapOffset = pushToHeap(&normalVectorHeap, &package.normals);
  asset.colorHeapOffset = pushToHeap(&colorHeap, &package.colors);
  asset.blendHeapOffset = pushToHeap(&blendHeap, &package.blends);
  asset.indexHeapOffset = pushToHeap(&indexHeap, &package.indices);
  asset.indexHeapAmount = package.indices.size();
  asset.tobeVramLoaded = true;
  asset.vramLoaded = false;
  asset.shininess = package.shininess;
  asset.ambient = package.ambient;
  asset.diffuse = package.diffuse;
  asset.specular = package.specular;
  asset.emission = package.emission;
  terrainNameIdMap[asset.name] = asset.assetId;
  terrainAssets[asset.assetId] = asset;
}

//______________________________________________________________________________

//___|SENDING MESSAGES|_________________________________________________________

ie::AssetStatusMessage ie::AssetManager::sendAssetStatusMessage(void)
{
  ie::AssetStatusMessage msg;
  msg.rus = &renderUnits;
  msg.models = &modelAssets;
  msg.modelNameIdMap = &modelNameIdMap;
  msg.textures = &textureAssets;
  msg.textureNameIdMap = &textureNameIdMap;
  msg.materials = &materialAssets;
  msg.materialNameIdMap = &materialNameIdMap;
  msg.terrains = &terrainAssets;
  msg.terrainNameIdMap = &terrainNameIdMap;
  msg.shaders = &shaderAssets;
  msg.shaderNameIdMap = &shaderNameIdMap;
  msg.lights = &lightAssets;
  msg.lightNameIdMap = &lightNameIdMap;
  msg.vHeap = &vertexHeap;
  msg.tHeap = &textureCoordinateHeap;
  msg.nHeap = &normalVectorHeap;
  msg.cHeap = &colorHeap;
  msg.bHeap = &blendHeap;
  msg.iHeap = &indexHeap;
  return msg;
}

//______________________________________________________________________________

//___|RECEIVING MESSAGES|_______________________________________________________

void ie::AssetManager::receiveMessage(ie::AssetManagerInstructions msg)
{
  for (unsigned int n = 0; n < msg.instructions.size(); n++)
  {
    receiveMessage(msg.instructions[n]);
  }
}


void ie::AssetManager::receiveMessage(ie::AssetManagerInstruction instruc)
{
  instructions.push_back(instruc);
}

//______________________________________________________________________________


//___|ACCESSING MANAGED ASSETS|________________________________________________

ie::handle ie::AssetManager::getHandle(std::string line)
{
  //RETURN POINTER TO ENTIRE GROUP OF ONE ASSET TYPE
  int tokenAmount = ie::countTokens(line, '/');  
  std::string token = ie::split(line, '/', 0);
  ie::handle hdl;

  if (token == "models" && tokenAmount == 1)
  {
    hdl.models = &modelAssets;
    return hdl;
  }
  else if (token == "materials" && tokenAmount == 1)
  {
    hdl.materials = &materialAssets;
    return hdl;
  }
  else if (token == "textures" && tokenAmount == 1)
  {
    hdl.textures = &textureAssets;
    return hdl;
  }
  else if (token == "shaders" && tokenAmount == 1)
  {
    hdl.shaders = &shaderAssets;
    return hdl;
  }
  else if (token == "lights" && tokenAmount == 1)
  {
    hdl.lights = &lightAssets;
    return hdl;
  }
  else if (token == "terrains" && tokenAmount == 1)
  {
    hdl.terrains = &terrainAssets;
    return hdl;
  }

  //RETURN POINTER TO A MODEL
  else if (token == "model" && tokenAmount > 1)
  {
    line = ie::popFrontToken(line, '/');
    token = ie::split(line, '/', 0);
    int tokenAmount = ie::countTokens(line, '/');  
    auto it = modelNameIdMap.find(token);
    if (it == modelNameIdMap.end())
    {
      std::cout << "Warning: Model '" << token <<
      "' doesn't exist"  << std::endl;
      hdl.model = 0;
      return hdl;
    }

    unsigned int id = modelNameIdMap[token];
    ModelAsset* ma = &modelAssets[id];
    if (tokenAmount == 1)
    {
      hdl.model = ma;  
      return hdl;
    }
  }

  //RETURN POINTER TO A MATERIAL
  else if (token == "material" && tokenAmount > 1)
  {
    line = ie::popFrontToken(line, '/');
    token = ie::split(line, '/', 0);
    int tokenAmount = ie::countTokens(line, '/');  
    auto it = materialNameIdMap.find(token);
    if (it == materialNameIdMap.end())
    {
      std::cout << "Warning: Material '" << token <<
                     "' doesn't exist"  << std::endl;
      hdl.material = 0;
      return hdl;
    }

    unsigned int id = materialNameIdMap[token];
    MaterialAsset* ma = &materialAssets[id];
    if (tokenAmount == 1)
    {
      hdl.material = ma;  
      return hdl;
    }
    else
    {
      std::cout << "Warning: Unrecognized material path '" << line << std::endl;
      hdl.material = 0;
      return hdl;
    }
  }

  //RETURN POINTER TO A TEXTURE
  else if (token == "texture" && tokenAmount > 1)
  {
    line = ie::popFrontToken(line, '/', 2);
    token = ie::split(line, '/', 0);
    int tokenAmount = ie::countTokens(line, '/');  
    auto it = textureNameIdMap.find(token);
    if (it == textureNameIdMap.end())
    {
      std::cout << "Warning: Texture '" << token <<
                    "' doesn't exist"  << std::endl;
      hdl.texture = 0;
      return hdl;
    }

    unsigned int id = textureNameIdMap[token];
    TextureAsset* ta = &textureAssets[id];
    if (tokenAmount == 1)
    {
      hdl.texture = ta;  
      return hdl;
    }
    else
    {
      std::cout << "Warning: Unrecognized texture path '" << line << std::endl;
      hdl.texture = 0;
      return hdl;
    }
  }

  //RETURN POINTER TO A SHADER
  else if (token == "shader" && tokenAmount > 1)
  {
    line = ie::popFrontToken(line, '/');
    token = ie::split(line, '/', 0);
    int tokenAmount = ie::countTokens(line, '/');  
    auto it = shaderNameIdMap.find(token);
    if (it == shaderNameIdMap.end())
    {
      std::cout << "Warning: Shader '" << token <<
                   "' doesn't exist"  << std::endl;
      hdl.shader = 0;
      return hdl;
    }

    GLuint id = shaderNameIdMap[token];
    ShaderAsset* spa = &shaderAssets[id];
    if (tokenAmount == 1)
    {
      hdl.shader = spa;  
      return hdl;
    }
    
    line = ie::popFrontToken(line, '/');
    token = ie::split(line, '/', 0);
    tokenAmount = ie::countTokens(line, '/');
    if (tokenAmount == 1)
    {
      auto it = (*spa).uniforms.find(token);
      if (it == (*spa).uniforms.end())
      {
        std::cout << "Warning: Uniform '" << token <<
        "' doesn't exist in Shader Program '" << (*spa).name <<
                                               "'" << std::endl;
        hdl.uniform = 0;
        return hdl;
      }
      hdl.uniform = &(*spa).uniforms[token];
      return hdl;
    }

    else
    {
      std::cout << "Warning: Unrecognized shader path '" << line << std::endl;
      hdl.shader = 0;
      return hdl;
    }
  }  

  //RETURN POINTER TO A LIGHT
  else if (token == "light" && tokenAmount > 1)
  {
    line = ie::popFrontToken(line, '/');
    token = ie::split(line, '/', 0);
    int tokenAmount = ie::countTokens(line, '/');  
    auto it = lightNameIdMap.find(token);
    if (it == lightNameIdMap.end())
    {
      std::cout << "Warning: Light '" << token <<
                     "' doesn't exist"  << std::endl;
      hdl.light = 0;
      return hdl;
    }

    unsigned int id = lightNameIdMap[token];
    LightAsset* la = &lightAssets[id];
    if (tokenAmount == 1)
    {
      hdl.light = la;  
      return hdl;
    }
    else
    {
      std::cout << "Warning: Unrecognized light path '" << line << std::endl;
      hdl.light = 0;
      return hdl;
    }
  }

  //RETURN POINTER TO A TERRAIN 
  else if (token == "terrain" && tokenAmount > 1)
  {
    line = ie::popFrontToken(line, '/');
    token = ie::split(line, '/', 0);
    int tokenAmount = ie::countTokens(line, '/');  
    auto it = terrainNameIdMap.find(token);
    if (it == terrainNameIdMap.end())
    {
      std::cout << "Warning: Terrian '" << token <<
                     "' doesn't exist"  << std::endl;
      hdl.terrain = 0;
      return hdl;
    }

    unsigned int id = terrainNameIdMap[token];
    TerrainAsset* ta = &terrainAssets[id];
    if (tokenAmount == 1)
    {
      hdl.terrain = ta;  
      return hdl;
    }
    else
    {
      std::cout << "Warning: Unrecognized terrain path '" << line << std::endl;
      hdl.terrain = 0;
      return hdl;
    }
  }
}

//______________________________________________________________________________

//___|RELEASING ASSETS AND CLEARING MEMORY|_____________________________________

bool ie::AssetManager::releaseAllShaders(void)
{
  for (auto it = shaderAssets.begin();
       it != shaderAssets.end(); it++)
  {
    ShaderAsset asset = it->second;  
    releaseShader(asset.name);
  }
}


bool ie::AssetManager::releaseShader(std::string name)
{
  ShaderAsset* shader = &shaderAssets[shaderNameIdMap[name]];
  glDetachShader((*shader).programId, (*shader).vertexShaderId);
  glDetachShader((*shader).programId, (*shader).fragmentShaderId);
  glDeleteShader((*shader).vertexShaderId);
  glDeleteShader((*shader).fragmentShaderId);
  glUseProgram(0);
  glDeleteProgram((*shader).programId);
}


bool ie::AssetManager::releaseAllTextures(void)
{
  for (auto texIt = textureAssets.begin();
       texIt != textureAssets.end(); texIt++)
  {
    ie::TextureAsset asset = texIt->second;
    releaseTexture(asset.textureId);
  }
}


bool ie::AssetManager::releaseTexture(std::string name)
{
  GLuint textureId = textureAssets[textureNameIdMap[name]].textureId;
  releaseTexture(textureId);  
}


bool ie::AssetManager::releaseTexture(GLuint id)
{
  glDeleteTextures(1, &id);
}


bool ie::AssetManager::quit(void)
{
  releaseAllTextures();
  releaseAllShaders();
}

//_____________________________________________________________________________
