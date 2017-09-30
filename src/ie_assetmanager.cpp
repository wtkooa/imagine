//___|"ie_assetmanager.cpp"|____________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_assetmanager.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "ie_assets.h"
#include "ie_definitions.h"
#include "ie_messages.h"
#include "ie_packages.h"
#include "ie_utils.h"

//___|ASSIGNING AND MANAGING ASSET IDS|_________________________________________

unsigned int ie::AssetManager::getNewModelAssetId(void)
{
  if (availableModelIds.empty())
  {
    return modelAssets.size();  
  }
  else
  {
    unsigned int id = availableModelIds[0];
    availableModelIds.erase(availableModelIds.begin());
    return id; 
  }
}


unsigned int ie::AssetManager::getNewMaterialAssetId(void)
{
  if (availableMaterialIds.empty())
  {
    return materialAssets.size();  
  }
  else
  {
    unsigned int id = availableMaterialIds[0];
    availableMaterialIds.erase(availableMaterialIds.begin());
    return id; 
  }
}

//______________________________________________________________________________

//___|LOADING VERTEX HEAP DATA|_________________________________________________

unsigned int ie::AssetManager::pushVertexData(std::vector<glm::vec4> v)
{
  unsigned int offset = vertexHeap.size();
  vertexHeap.insert(vertexHeap.end(), v.begin(), v.end());
  return offset;
}


unsigned int ie::AssetManager::pushTextureCoordinateData(std::vector<glm::vec3> vt)
{
  unsigned int offset = textureCoordinateHeap.size();
  textureCoordinateHeap.insert(textureCoordinateHeap.end(),
                               vt.begin(), vt.end());
  return offset;
}


unsigned int ie::AssetManager::pushNormalVectorData(std::vector<glm::vec3> vn)
{
  unsigned int offset = normalVectorHeap.size();
  normalVectorHeap.insert(normalVectorHeap.end(), vn.begin(), vn.end());
  return offset;
}


unsigned int ie::AssetManager::pushIndexData(std::vector<glm::ivec4> f)
{
  unsigned int offset = indexHeap.size();
  indexHeap.insert(indexHeap.end(), f.begin(), f.end());
  return offset;
}

//______________________________________________________________________________

//___|UNWRAPPING INCOMING PACKAGES|_____________________________________________

//WAVEFRONT OBJ FILE PACKAGES
void ie::AssetManager::unwrapPackage(ie::WavefrontObjectFilePackage filePackage)
{
  unsigned int vertexHeapOffset = pushVertexData(filePackage.v);
  unsigned int textureCoordinateHeapOffset = pushTextureCoordinateData(filePackage.vt);
  unsigned int normalVectorHeapOffset = pushNormalVectorData(filePackage.vn);
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
  unsigned int indexOffset = pushIndexData(filePackage.f);

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
    asset.modelId = getNewModelAssetId();
    modelNameIdMap[asset.name] = asset.modelId;
    asset.filename = filePackage.filename;
    asset.filepath = filePackage.filepath;
    asset.translationMatrix = glm::mat4();
    asset.rotationMatrix = glm::mat4();
    asset.copy = false;
    asset.hidden = false;
    asset.tobeVramLoaded = true;
    asset.vramLoaded = false;
    unsigned int objectIndexBegin = it->first;
    unsigned int objectIndexEnd = 0;
    auto nextIt = it;
    std::advance(nextIt, 1);
    if (nextIt != filePackage.objects.end())
    {
      objectIndexEnd = nextIt->first;
    }
    else
    {
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
        renderUnit.materialId = materialNameIdMap[usingMaterialName];
        renderUnit.shaderProgram = "static";
        renderUnit.indexOffset = indexOffset + groupIndexBegin;
        renderUnit.vertexAmount = groupVertexAmount;
        renderUnit.hidden = false;
        asset.renderUnits.push_back(renderUnit);
      }
    }
    modelAssets[asset.modelId] = asset;
  } 
}


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
  asset.materialId = getNewMaterialAssetId();
  asset.filename = package.filename;
  asset.filepath = package.filepath;
  asset.shininess = package.shininess;
  asset.ambient = package.ambient;
  asset.diffuse = package.diffuse;
  asset.specular = package.specular;
  asset.emission = package.emission;
  asset.opticalDensity = package.opticalDensity;
  asset.dissolve = package.dissolve;
  materialNameIdMap[asset.name] = asset.materialId;
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
    WavefrontTexturePackage tex = package.texturePackages[ntex];
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
  materialAssets[asset.materialId] = asset;
}


GLuint ie::AssetManager::unwrapPackage(ie::WavefrontTexturePackage package)
{
  GLuint id;
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
  glGenTextures(1, &id);
  asset.textureOpenglId = id;
  asset.filename = package.filename;
  asset.filepath = "";
  asset.textureType = package.type;
  asset.tobeVramLoaded = true;
  asset.vramLoaded = false;
  textureNameIdMap[asset.name] = asset.textureOpenglId;
  textureAssets[asset.textureOpenglId] = asset;
  return id;
}


//SHADER PACKAGES 
void ie::AssetManager::unwrapPackage(ie::ShaderProgramPackage package)
{
  ie::ShaderProgramAsset asset;
  asset.name = package.name;
  bool shaderProgramNameTaken = shaderProgramAssets.count(asset.name) == 1;
  if (shaderProgramNameTaken)
  {
    std::cout << "Warning: shader program with name '" << asset.name <<
    "' already exists. Engine will use original." << std::endl;
    return;
  }
  asset.programId = package.programId;
  asset.vertexShaderId = package.vertexShaderId;
  asset.fragmentShaderId = package.fragmentShaderId;
  asset.uniforms = package.uniforms;
  shaderProgramAssets[asset.name] = asset;
}

//______________________________________________________________________________

//___|SENDING MESSAGES|_________________________________________________________

ie::CreateVboMessage ie::AssetManager::sendCreateVboMessage(void)
{
  ie::CreateVboMessage msg;
  msg.models = &modelAssets;
  msg.textures = &textureAssets;
  msg.vHeap = &vertexHeap;
  msg.tHeap = &textureCoordinateHeap;
  msg.nHeap = &normalVectorHeap;
  msg.iHeap = &indexHeap;
  return msg;
}


ie::RenderAssetMessage ie::AssetManager::sendRenderAssetMessage(std::string prog,
                                                                std::string list)
{
  ie::RenderAssetMessage msg;
  msg.shaderProgram = &(shaderProgramAssets[prog]);
  msg.quickList = &(quickLists[list]);
  msg.materials = &materialAssets;
  msg.models = &modelAssets;
  return msg;
}

//______________________________________________________________________________

//___|CREATING AND MANAGING QUICK RENDER LISTS|_________________________________

void ie::AssetManager::createQuickLists(void)
{
  std::vector<QuickListElement> vList;
  std::vector<QuickListElement> vnList;
  std::vector<QuickListElement> vtnList;
  for (auto modIt = modelAssets.begin(); modIt != modelAssets.end(); modIt++)
  {
    ModelAsset model = modIt->second;
    if (model.hidden == false)
    {
      QuickListElement vElement;
      QuickListElement vnElement;
      QuickListElement vtnElement;
      vElement.modelId = model.modelId;
      vnElement.modelId = model.modelId;
      vtnElement.modelId = model.modelId;
      for (int nUnit = 0; nUnit < model.renderUnits.size(); nUnit++)
      {
        RenderUnit ru = model.renderUnits[nUnit];
        if (ru.hidden == false)
        {
          switch (ru.dataFormat)
          {
            case V:
              vElement.renderUnitList.push_back(nUnit);
              break;
            case VN:
              vnElement.renderUnitList.push_back(nUnit);
              break;
            case VTN:
              vtnElement.renderUnitList.push_back(nUnit);
              break;
          }
        }
      }
      if (vElement.renderUnitList.size() > 0) {vList.push_back(vElement);}
      if (vnElement.renderUnitList.size() > 0) {vnList.push_back(vnElement);}
      if (vtnElement.renderUnitList.size() > 0) {vtnList.push_back(vtnElement);}
    }
  }
  quickLists["vList"] = vList;
  quickLists["vnList"] = vnList;
  quickLists["vtnList"] = vtnList;
}

//______________________________________________________________________________

//___|ACCESSING MANAGED ASSETS|________________________________________________

ie::handle ie::AssetManager::getHandle(std::string line)
{
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
    hdl.shaders = &shaderProgramAssets;
    return hdl;
  }
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
    
    line = ie::popFrontToken(line, '/');
    token = ie::split(line, '/', 0);
    tokenAmount = ie::countTokens(line, '/');
    int ru;
    if (tokenAmount == 1)
    {
      try
      {
        ru = std::stoi(token);
      }
      catch(...)
      {
        std::cout << "Warning: stoi conversion of render unit '" << token <<
        "' failed"  << std::endl;
        hdl.model = 0; 
        return hdl;
      }
      if (ru > (*ma).renderUnits.size() - 1)
      {
        std::cout << "Warning: Render unit '" << ru <<
        "' isn't a member of '" << (*ma).name << "'"  << std::endl;
        hdl.model = 0;
        return hdl;
      }
      hdl.ru = &(*ma).renderUnits[ru];
      return hdl;
    }
    else
    {
      std::cout << "Warning: Unrecognized model path '" << line << std::endl;
      hdl.model = 0;
      return hdl;
    }

  }
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
  else if (token == "shader" && tokenAmount > 1)
  {
    line = ie::popFrontToken(line, '/');
    token = ie::split(line, '/', 0);
    int tokenAmount = ie::countTokens(line, '/');  
    auto it = shaderProgramAssets.find(token);
    if (it == shaderProgramAssets.end())
    {
      std::cout << "Warning: Shader '" << token <<
                   "' doesn't exist"  << std::endl;
      hdl.shader = 0;
      return hdl;
    }

    ShaderProgramAsset* spa = &shaderProgramAssets[token];
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
      hdl.model = 0;
      return hdl;
    }
  }
}

//______________________________________________________________________________

//___|RELEASING ASSETS AND CLEARING MEMORY|_____________________________________

bool ie::AssetManager::releaseAllShaderPrograms(void)
{
  for (auto it = shaderProgramAssets.begin();
       it != shaderProgramAssets.end(); it++)
  {
    ShaderProgramAsset asset = it->second;  
    releaseShaderProgram(asset.name);
  }
}


bool ie::AssetManager::releaseShaderProgram(std::string name)
{
  ShaderProgramAsset asset = shaderProgramAssets[name];
  glDetachShader(asset.programId, asset.vertexShaderId);
  glDetachShader(asset.programId, asset.fragmentShaderId);
  glDeleteShader(asset.vertexShaderId);
  glDeleteShader(asset.fragmentShaderId);
  glUseProgram(0);
  glDeleteProgram(asset.programId);
}


bool ie::AssetManager::releaseAllTextures(void)
{
  for (auto texIt = textureAssets.begin();
       texIt != textureAssets.end(); texIt++)
  {
    ie::TextureAsset asset = texIt->second;
    releaseTexture(asset.textureOpenglId);
  }
}


bool ie::AssetManager::releaseTexture(std::string name)
{
  GLuint id = textureNameIdMap[name];
  releaseTexture(id);  
}


bool ie::AssetManager::releaseTexture(GLuint id)
{
  glDeleteTextures(1, &id);
}


bool ie::AssetManager::quit(void)
{
  releaseAllTextures();
  releaseAllShaderPrograms();
}

//_____________________________________________________________________________
