//___|"ie_vram.cpp"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_vram.h"

#include <iostream>
#include <map>
#include <string>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

#include "ie_assets.h"
#include "ie_memory.h"
#include "ie_messages.h"


//___|RECEIVING MESSAGES|_______________________________________________________

void ie::VramManager::receiveMessage(ie::AssetStatusToVramMessage msg)
{
  models = msg.models;
  textures = msg.textures;
  materials = msg.materials;
  terrains = msg.terrains;
  vHeap = msg.vHeap;
  tHeap = msg.tHeap;
  nHeap = msg.nHeap;
  cHeap = msg.cHeap;
  bHeap = msg.bHeap;
  iHeap = msg.iHeap;
}

//______________________________________________________________________________

//___|SENDING MESSAGES|_________________________________________________________

ie::VramStatusToRenderMessage ie::VramManager::sendVramStatusToRenderMessage()
{
  ie::VramStatusToRenderMessage msg;
  msg.vPair = &vPair;
  msg.vnPair = &vnPair;
  msg.vtnPair = &vtnPair;
  msg.vtncbPair = &vtncbPair;
  msg.terrainIndexPair = &terrainIndexPair;
  msg.staticMemoryMap = &(staticMemoryMap);
  msg.terrainIndexMemoryMap = &terrainIndexMemoryMap;
  return msg;
}

//______________________________________________________________________________

//___|BUILDING AND MANAGING CPU SIDE VBOS|______________________________________

void ie::VramManager::createVbos(void)
{
  createStaticVbos();
  createTerrainVbos();
}


//CREATING STATIC VBOS AND MEMORY MAPS
void ie::VramManager::createStaticVbos()
{
  vPair.genBuffers();
  vnPair.genBuffers();
  vtnPair.genBuffers();

  for (auto modIt = models->begin(); modIt != models->end(); modIt++)
  {
    unsigned int modelId = modIt->first;
    ie::ModelAsset model = modIt->second;
    if (model.tobeVramLoaded == false) {continue;} 
    (modIt->second).vramLoaded = true;
    (modIt->second).tobeVramLoaded = false;
    short renderUnitAmount = model.renderUnits.size();
    std::vector<StaticRenderUnitLocation> ruLocations;
    for (short ru = 0; ru < renderUnitAmount; ru++)
    {
      ie::VboDataFormat dataFormat = model.renderUnits[ru].dataFormat;
      unsigned int materialId = model.renderUnits[ru].materialId;
      MaterialAsset* material = &(*materials)[materialId];
      GLuint textureId = (*material).diffuseMapId;
      TextureAsset* textureAsset = &(*textures)[textureId];
      StaticRenderUnitLocation ruLocation;
      ruLocation.renderUnit = ru;
      ruLocation.format = dataFormat;
      unsigned int indexAmount = model.renderUnits[ru].vertexAmount;
      ruLocation.indexAmount = indexAmount;
      switch (dataFormat)
      {
        case V:
          ruLocation.location = vboV.size(); 
          ruLocations.push_back(ruLocation);

          for (unsigned int n = 0; n < indexAmount; n++)
          {
            ie::VFormat vboDataUnit;
            unsigned int baseIndex = model.renderUnits[ru].indexOffset;
            glm::ivec4 faceElement = (*iHeap)[baseIndex + n];
            unsigned int vertexIndex = faceElement.x;
            glm::vec4 vertex = (*vHeap)[vertexIndex];
            vboDataUnit.vertex = glm::vec3(vertex.x, vertex.y, vertex.z);
            vboV.push_back(vboDataUnit);
          }
          break;
        case VN:
          ruLocation.location = vboVN.size(); 
          ruLocations.push_back(ruLocation);

          for (unsigned int n = 0; n < indexAmount; n++)
          {
            ie::VNFormat vboDataUnit;
            unsigned int baseIndex = model.renderUnits[ru].indexOffset;
            glm::ivec4 faceElement = (*iHeap)[baseIndex + n];
            unsigned int vertexIndex = faceElement.x;
            unsigned int normalIndex = faceElement.z;
            glm::vec4 vertex = (*vHeap)[vertexIndex];
            glm::vec3 normal = (*nHeap)[normalIndex];
            vboDataUnit.vertex = glm::vec3(vertex.x, vertex.y, vertex.z);
            vboDataUnit.normal = glm::vec3(normal.x, normal.y, normal.z);
            vboVN.push_back(vboDataUnit);
          }
          break;
        case VTN:
          ruLocation.location = vboVTN.size(); 
          ruLocations.push_back(ruLocation);

          for (unsigned int n = 0; n < indexAmount; n++)
          {
            ie::VTNFormat vboDataUnit;
            unsigned int baseIndex = model.renderUnits[ru].indexOffset;
            glm::ivec4 faceElement = (*iHeap)[baseIndex + n];
            unsigned int vertexIndex = faceElement.x;
            unsigned int textureIndex = faceElement.y;
            unsigned int normalIndex = faceElement.z;
            glm::vec4 vertex = (*vHeap)[vertexIndex];
            glm::vec3 texture = (*tHeap)[textureIndex];
            glm::vec3 normal = (*nHeap)[normalIndex];
            vboDataUnit.vertex = glm::vec3(vertex.x, vertex.y, vertex.z);
            vboDataUnit.texture = glm::vec2(texture.x, texture.y);
            vboDataUnit.normal = glm::vec3(normal.x, normal.y, normal.z);
            vboVTN.push_back(vboDataUnit);
            if ((*textureAsset).tobeVramLoaded == true)
            {
              loadTexture((*textureAsset));
              (*textureAsset).tobeVramLoaded = false;
              (*textureAsset).vramLoaded = true;
            }
          }
        break;
      }
    }
    staticMemoryMap[model.modelId] = ruLocations;
  }
}


//CREATING TERRAIN VBOS AND MEMORY MAPS
void ie::VramManager::createTerrainVbos(void)
{
  vtncbPair.genBuffers();
  terrainIndexPair.genBuffers();

  for (auto terIt = terrains->begin(); terIt != terrains->end(); terIt++)
  {
    TerrainAsset* terrain = &terIt->second; 
    short dim = (*terrain).dim;
    float unitSize = (*terrain).unitSize;
    unsigned int vHeapA = dim * dim;
    unsigned int vHeapO = (*terrain).vertexHeapOffset;
    unsigned int nHeapO = (*terrain).normalHeapOffset;
    unsigned int cHeapO = (*terrain).colorHeapOffset;
    unsigned int bHeapO = (*terrain).blendHeapOffset;
    unsigned int iHeapO = (*terrain).indexHeapOffset;
    unsigned int iHeapA = (*terrain).indexHeapAmount;
    std::vector<GLuint>* textureIds = &(*terrain).textureIds;
    terrain->tobeVramLoaded = false;
    terrain->vramLoaded = true;

    for (int nTex = 0; nTex < (*textureIds).size(); nTex++)
    {
      loadTexture((*textures)[(*textureIds)[nTex]]);
    }

    unsigned int vboVTNCBOffset = vboVTNCB.size();
    unsigned int indexOffset = vboTerrainIndex.size();
    ie::TerrainRenderUnitLocation vboVTNCBMapUnit;
    ie::TerrainRenderUnitLocation indexMapUnit;
    vboVTNCBMapUnit.location = vboVTNCBOffset;
    vboVTNCBMapUnit.indexAmount = vHeapA;
    indexMapUnit.location = indexOffset;
    indexMapUnit.indexAmount = iHeapA * 3;
    terrainMemoryMap[(*terrain).id] = vboVTNCBMapUnit;
    terrainIndexMemoryMap[(*terrain).id] = indexMapUnit;

    std::vector<glm::vec2> tCoords;
    for (short z = 0; z < dim; z++)
    {
      for (short x = 0; x < dim; x++)
      {
        glm::vec2 coords(x, z);
        tCoords.push_back(coords);
      }
    }

    for (unsigned int n = 0; n < vHeapA; n++)
    {
      ie::VTNCBFormat vboDataUnit;
      glm::vec4 vertex4 = (*vHeap)[vHeapO + n];
      vboDataUnit.vertex = glm::vec3(vertex4.x, vertex4.y, vertex4.z);
      vboDataUnit.texture = tCoords[n];
      vboDataUnit.normal = (*nHeap)[nHeapO + n];
      vboDataUnit.color = (*cHeap)[cHeapO + n];
      vboDataUnit.blend = (*bHeap)[bHeapO + n];
      vboVTNCB.push_back(vboDataUnit);
    }

    for (unsigned int n = 0; n < iHeapA; n++)
    {
      glm::ivec4 index4 = (*iHeap)[iHeapO + n];
      vboTerrainIndex.push_back(index4.x);
      vboTerrainIndex.push_back(index4.y);
      vboTerrainIndex.push_back(index4.z);
    }
  }
}

//______________________________________________________________________________

//___|LOADING DATA TO VIDEO HARDWARE|___________________________________________

void ie::VramManager::loadTexture(ie::TextureAsset textureAsset)
{ 
  SDL_Surface* surface;
  GLuint textureId = textureAsset.textureOpenglId;
  int mode;
  surface = IMG_Load((textureAsset.filepath +
                      textureAsset.filename).c_str());
  if (!surface) {std::cout << "Warning: Texture '" << textureAsset.filename <<
  "' failed to load to VRAM..." << std::endl;}
  glBindTexture(GL_TEXTURE_2D, textureId);
  if (surface->format->BytesPerPixel == 4) {mode = GL_RGBA;}
  else if (surface->format->BytesPerPixel = 3) {mode = GL_RGB;}
  glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h,
               0, mode, GL_UNSIGNED_BYTE, surface->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.5); 
  glBindTexture(GL_TEXTURE_2D, 0);
  SDL_FreeSurface(surface);
}


void ie::VramManager::loadVbos(void)
{
  short dataSize = sizeof(VFormat);
  glBindBuffer(GL_ARRAY_BUFFER, vPair.readVbo);
  glBufferData(GL_ARRAY_BUFFER, vboV.size() * dataSize,
               vboV.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vPair.writeVbo);
  glBufferData(GL_ARRAY_BUFFER, vboV.size() * dataSize,
               vboV.data(), GL_STATIC_DRAW);

  dataSize = sizeof(VNFormat);
  glBindBuffer(GL_ARRAY_BUFFER, vnPair.readVbo);
  glBufferData(GL_ARRAY_BUFFER, vboVN.size() * dataSize,
               vboVN.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vnPair.writeVbo);
  glBufferData(GL_ARRAY_BUFFER, vboVN.size() * dataSize,
               vboVN.data(), GL_STATIC_DRAW);

  dataSize = sizeof(VTNFormat);
  glBindBuffer(GL_ARRAY_BUFFER, vtnPair.readVbo);
  glBufferData(GL_ARRAY_BUFFER, vboVTN.size() * dataSize,
               vboVTN.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vtnPair.writeVbo);
  glBufferData(GL_ARRAY_BUFFER, vboVTN.size() * dataSize,
               vboVTN.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  dataSize = sizeof(VTNCBFormat);
  glBindBuffer(GL_ARRAY_BUFFER, vtncbPair.readVbo);
  glBufferData(GL_ARRAY_BUFFER, vboVTNCB.size() * dataSize,
               vboVTNCB.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vtncbPair.writeVbo);
  glBufferData(GL_ARRAY_BUFFER, vboVTNCB.size() * dataSize,
               vboVTNCB.data(), GL_STATIC_DRAW);

  dataSize = sizeof(unsigned int);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIndexPair.readVbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vboTerrainIndex.size() * dataSize,
               vboTerrainIndex.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIndexPair.writeVbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vboTerrainIndex.size() * dataSize,
               vboTerrainIndex.data(), GL_STATIC_DRAW);
}


void ie::VramManager::createAndLoadVbos(void)
{
  createVbos();
  loadVbos();
}

//______________________________________________________________________________

//___|RELEASING VIDEO HARDWARE MEMORY|__________________________________________

void ie::VramManager::quit(void)
{
  vPair.release();
  vnPair.release();
  vtnPair.release();
  vtncbPair.release();
  terrainIndexPair.release();
}

//______________________________________________________________________________
