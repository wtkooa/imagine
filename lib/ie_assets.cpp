#include "ie_assets.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>

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

unsigned int ie::AssetManager::getNewTextureAssetId(void)
{
  if (availableTextureIds.empty())
  {
    return textureAssets.size();  
  }
  else
  {
    unsigned int id = availableTextureIds[0];
    availableTextureIds.erase(availableTextureIds.begin());
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

void ie::AssetManager::unwrapWavefrontObjectFilePackage(
                       ie::WavefrontObjectFilePackage filePackage)
{
  unsigned int vertexHeapOffset = pushVertexData(filePackage.v);
  unsigned int textureCoordinateHeapOffset = pushTextureCoordinateData(filePackage.vt);
  unsigned int normalVectorHeapOffset = pushNormalVectorData(filePackage.vn);

  for (auto n = filePackage.objects.begin(); n != filePackage.objects.end(); n++)
  {
    ie::WavefrontObjectPackage package = ie::WavefrontObjectPackage();
  } 
}

void ie::AssetManager::unwrapWavefrontObjectPackage(
                       ie::WavefrontObjectPackage package)
{
}
