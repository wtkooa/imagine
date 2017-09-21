#pragma once


#define GL_GLEXT_PROTOTYPES // Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

namespace ie
{

  class VramManager
  {
    public:
    VramManager();
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

}
