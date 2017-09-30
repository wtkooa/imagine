#ifndef IE_VRAM_H
#define IE_VRAM_H

//___|"ie_vram.h"|______________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

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
    //RECIEVING MESSAGES
    void receiveMessage(CreateVboMessage); 

    //SENDING MESSAGES
    RenderMemoryMessage sendRenderMemoryMessage(std::string);

    //RELEASING VIDEO HARDWARE MEMORY
    void quit(void);

    private:
    //VBO ID PAIRS
    VboPair vPair;
    VboPair vnPair;
    VboPair vtnPair;

    //CPU SIDE VBO DATA
    std::vector<VFormat> vboV;
    std::vector<VNFormat> vboVN;
    std::vector<VTNFormat> vboVTN;

    //VBO MEMORY MAPS
    std::map<unsigned int, std::vector<VboRenderUnitLocation>> vboMemoryMap; 

    //LOADING DATA TO VIDEO HARDWARE
    void loadTexture(TextureAsset);
    void loadVbo(void);
  };

}

#endif
