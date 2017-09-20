#pragma once
#define GL_GLEXT_PROTOTYPES // Needs to be defined for some GL funcs to work.
#include <map>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include "ie_wavefront.h"



class VRAMManager
{
  public:
  VRAMManager();
  bool genVBO(modelResource*, int);
  bool releaseMem(void);  
  GLuint vboID;
  int vboSizeBytes;
  void* positionStart;
  GLuint positionDim;
  GLuint textureDim;
  void* textureStart;
  GLuint normalDim;
  void* normalStart;
  GLuint vboStride;
};


