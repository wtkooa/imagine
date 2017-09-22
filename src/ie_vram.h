#pragma once

#include <map>
#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES // Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

#include "ie_assets.h"
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
    VboDataFormat format;
    unsigned int location;
    unsigned int indexAmount;
  };

  class VramManager
  {
    public:
    VramManager();
    void recieveMessage(CreateVboMessage); 

    void quit(void);
    private:
    void loadTexture(TextureAsset);
    void loadVbo(void);
    std::map<std::string, VboPair> vboIdPairs;
    std::map<unsigned int, std::vector<VboRenderUnitLocation>> vboMemoryMap; 
    std::vector<VFormat> vboV;
    std::vector<VNFormat> vboVN;
    std::vector<VTNFormat> vboVTN;
  };

}
