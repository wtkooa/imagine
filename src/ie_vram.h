#pragma once

#include <map>
#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES // Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

#include "ie_assets.h"
#include "ie_definitions.h"
#include "ie_messages.h"

namespace ie
{
  
  class VramManager
  {
    public:
    void receiveMessage(CreateVboMessage); 

    RenderMemoryMessage sendRenderMemoryMessage(std::string);

    void quit(void);
    private:
    void loadTexture(TextureAsset);
    void loadVbo(void);
    std::map<std::string, VboPair> vboIdPairs;
    VboPair vPair;
    VboPair vnPair;
    VboPair vtnPair;
    std::map<unsigned int, std::vector<VboRenderUnitLocation>> vboMemoryMap; 
    std::vector<VFormat> vboV;
    std::vector<VNFormat> vboVN;
    std::vector<VTNFormat> vboVTN;
  };

}
