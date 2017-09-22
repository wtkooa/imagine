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
    loadTexture(texIt->second);
  }


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

void ie::VramManager::quit(void)
{
  for (auto vboIt = vboIdPairs.begin(); vboIt != vboIdPairs.end(); vboIt++)
  {
    (vboIt->second).release();
  }
}


/*
//Video Memory Manger
VramManager::VramManager(void)
{
  vboSizeBytes = 0;
  positionStart = (void*)(0);
  positionDim = 3;
  textureStart = (void*)(sizeof(float) * 3);
  textureDim = 3;
  normalStart = (void*)(sizeof(float) * 6);
  normalDim = 3;
  vboStride = sizeof(float) * 9;
  glGenBuffers(1, &vboID);
  glBindBuffer(GL_ARRAY_BUFFER, vboID); 
}

bool VRAMManager::genVBO(modelResource* model, int modelAmount)
{
  int newVBOSizeBytes = 0;
  for (int n = 0; n < modelAmount; n++)
  {
    if (model[n].vboloaded == true || model[n].tobevboloaded == true)
    {
      newVBOSizeBytes += model[n].vertexSizeBytes;
    }
  }
  glDeleteBuffers(1, &vboID);
  glGenBuffers(1, &vboID);
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glBufferData(GL_ARRAY_BUFFER, newVBOSizeBytes, NULL, GL_STATIC_DRAW);
  vboSizeBytes = newVBOSizeBytes;
  size_t vOffset = 0;
  size_t iOffset = 0;
  for (int n = 0; n < modelAmount; n++)
  {
    if (model[n].vboloaded == true || model[n].tobevboloaded == true)
    {
      model[n].vboOffsetBytes = vOffset;
      model[n].vboOffsetIndex = iOffset;
      glBufferSubData(GL_ARRAY_BUFFER,
             vOffset,
              model[n].vertexSizeBytes,
              model[n].vertexData);
      vOffset += (model[n].vertexSizeBytes);
      iOffset += (model[n].vertexAmount);
      model[n].vboloaded = true;
      model[n].tobevboloaded = false;
    } 
  } 
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

