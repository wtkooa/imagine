#include "ie_vram.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

#include "ie_assets.h"

ie::VboPair::VboPair()
{
  glGenBuffers(1, &readVbo);
  glGenBuffers(1, &writeVbo);
}

void ie::VboPair::swap(void)
{
  std::swap(readVbo, writeVbo);
}

void ie::VboPair::release(void)
{
  glDeleteBuffers(1, &readVbo);
  glDeleteBuffers(1, &writeVbo);
}

ie::VramManager::VramManager()
{
  ie::VboPair vPair;
  ie::VboPair vnPair;
  ie::VboPair vtnPair;
  vboIdPairs["vPair"] = vPair;
  vboIdPairs["vnPair"] = vnPair;
  vboIdPairs["vtnPair"] = vtnPair;
}

void ie::VramManager::recieveMessage(ie::CreateVboMessage msg)
{
  std::map<unsigned int, ModelAsset>* models = msg.models;
  std::map<unsigned int, TextureAsset>* textures = msg.textures;
  std::vector<glm::vec4>* vHeap = msg.vHeap;
  std::vector<glm::vec3>* tHeap = msg.tHeap;
  std::vector<glm::vec3>* nHeap = msg.nHeap;
  std::vector<glm::ivec4>* iHeap = msg.iHeap;

  for (auto texIt = textures->begin(); texIt != textures->end(); texIt++)
  {
    if ((texIt->second).tobeVramLoaded == true)
    {
      (texIt->second).tobeVramLoaded = false;
      (texIt->second).vramLoaded = true;
      loadTexture(texIt->second);
    }
  }

  for (auto modIt = models->begin(); modIt != models->end(); modIt++)
  {
    unsigned int modelId = modIt->first;
    ie::ModelAsset model = modIt->second;
    if (model.tobeVramLoaded == false) {continue;} 
    (modIt->second).vramLoaded = true;
    (modIt->second).tobeVramLoaded = false;
    short renderUnitAmount = model.renderUnits.size();
    std::vector<VboRenderUnitLocation> ruLocations;
    for (short ru = 0; ru < renderUnitAmount; ru++)
    {
      ie::VboDataFormat dataFormat = model.renderUnits[ru].dataFormat;
      VboRenderUnitLocation ruLocation;
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
            vboDataUnit.vertex = vertex;
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
            vboDataUnit.vertex = vertex;
            vboDataUnit.normal = normal;
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
            vboDataUnit.vertex = vertex;
            vboDataUnit.texture = texture;
            vboDataUnit.normal = normal;
            vboVTN.push_back(vboDataUnit);
          }
        break;
      }
    }
    vboMemoryMap[model.modelId] = ruLocations;
  }
  loadVbo();
}

void ie::VramManager::loadTexture(ie::TextureAsset textureAsset)
{ 
  SDL_Surface* surface;
  GLuint textureId = textureAsset.textureOpenglId;
  int mode;
  surface = IMG_Load(textureAsset.filename.c_str());
  if (!surface) {std::cout << "Warning: Texture '" << textureAsset.filename <<
                 "' failed to load..." << std::endl;}
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  if (surface->format->BytesPerPixel == 4) {mode = GL_RGBA;}
  else if (surface->format->BytesPerPixel = 3) {mode = GL_RGB;}
  glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  SDL_FreeSurface(surface);
}

void ie::VramManager::loadVbo(void)
{
  short dataSize = sizeof(VFormat);
  VboPair pair = vboIdPairs["vPair"];
  glBindBuffer(GL_ARRAY_BUFFER, pair.readVbo);
  glBufferData(GL_ARRAY_BUFFER, vboV.size() * dataSize, vboV.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, pair.writeVbo);
  glBufferData(GL_ARRAY_BUFFER, vboV.size() * dataSize, vboV.data(), GL_STATIC_DRAW);
  dataSize = sizeof(VNFormat);
  pair = vboIdPairs["vnPair"];
  glBindBuffer(GL_ARRAY_BUFFER, pair.readVbo);
  glBufferData(GL_ARRAY_BUFFER, vboVN.size() * dataSize, vboV.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, pair.writeVbo);
  glBufferData(GL_ARRAY_BUFFER, vboVN.size() * dataSize, vboV.data(), GL_STATIC_DRAW);
  dataSize = sizeof(VTNFormat);
  pair = vboIdPairs["vtnPair"];
  glBindBuffer(GL_ARRAY_BUFFER, pair.readVbo);
  glBufferData(GL_ARRAY_BUFFER, vboVTN.size() * dataSize, vboV.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, pair.writeVbo);
  glBufferData(GL_ARRAY_BUFFER, vboVTN.size() * dataSize, vboV.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ie::VramManager::quit(void)
{
  for (auto vboIt = vboIdPairs.begin(); vboIt != vboIdPairs.end(); vboIt++)
  {
    (vboIt->second).release();
  }
}

/*
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, positionDim,
              GL_FLOAT, GL_FALSE,
              vboStride, positionStart);
  glVertexAttribPointer(1, textureDim,
              GL_FLOAT, GL_FALSE,
              vboStride, textureStart);
  glVertexAttribPointer(2, normalDim,
              GL_FLOAT, GL_FALSE,
              vboStride, normalStart);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool VRAMManager::releaseMem(void)
{ 
  glDeleteBuffers(1, &vboID);
}
*/

