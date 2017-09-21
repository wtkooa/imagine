#include "vram.h"

#include <map>
#include <string>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>


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


