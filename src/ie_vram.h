#pragma once

#include <map>
#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES // Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

#include "ie_messages.h"

namespace ie
{
  
  class VFormat
  {
    public:
    glm::vec4 vertex;
  };

  class VNFormat
  {
    public:
    glm::vec4 vertex;
    glm::vec3 normal;
  };

  class VTNFormat
  {
    public:
    glm::vec4 vertex;
    glm::vec3 texture;
    glm::vec3 normal;
  };

  class VboPair
  {
    public:
    VboPair();
    GLuint readVbo;
    GLuint writeVbo;
    void swap(void);
    void release(void);
  };

  class VboRenderUnitLocation 
  {
    public:
    short renderUnit;
    unsigned int location;
    unsigned int indexSize;
  };

  class VramManager
  {
    public:
    VramManager();
    void recieveMessage(CreateVboMessage); 

    void quit(void);
    private:
    void loadTexture(TextureAsset);
    std::map<std::string, VboPair> vboIdPairs;
    std::vector<VFormat> vboV;
    std::map<unsigned int, VboRenderUnitLocation> vboVMemoryMap; 
    std::vector<VNFormat> vboVN;
    std::map<unsigned int, VboRenderUnitLocation> vboVNMemoryMap; 
    std::vector<VTNFormat> vboVTN;
    std::map<unsigned int, VboRenderUnitLocation> vboVTNMemoryMap; 
  };

}

/*
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
*/
