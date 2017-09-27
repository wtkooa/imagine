#include "ie_assetmanager.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

#include "ie_assets.h"
#include "ie_definitions.h"
#include "ie_messages.h"
#include "ie_packages.h"

//Asset Manager
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

unsigned int ie::AssetManager::pushVertexData(std::vector<glm::vec4> v)
{
  unsigned int offset = vertexHeap.size();
  vertexHeap.insert(vertexHeap.end(), v.begin(), v.end());
  return offset;
}

unsigned int ie::AssetManager::pushTextureCoordinateData(std::vector<glm::vec3> vt)
{
  unsigned int offset = textureCoordinateHeap.size();
  textureCoordinateHeap.insert(textureCoordinateHeap.end(), vt.begin(), vt.end());
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

  for (auto it = filePackage.objects.begin(); it != filePackage.objects.end(); it++)
  {
    ie::ModelAsset asset;
    asset.name = it->second;
    bool modelNameTaken = modelNameIdMap.count(asset.name) == 1;
    if (modelNameTaken)
    {
      std::cout << "Warning: Model with name '" << asset.name << "' already exists." << std::endl;
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

GLuint ie::AssetManager::unwrapPackage(
                       ie::WavefrontObjectPackage package)
{
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

GLuint ie::AssetManager::unwrapPackage(
                         ie::WavefrontMaterialPackage package)
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

GLuint ie::AssetManager::unwrapPackage(
                         ie::WavefrontTexturePackage package)
{
  GLuint id;
  ie::TextureAsset asset;
  asset.name = package.filename;
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
  for (auto texIt = textureAssets.begin(); texIt != textureAssets.end(); texIt++)
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
