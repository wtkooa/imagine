#ifndef IE_VRAM_H
#define IE_VRAM_H

//___|"ie_vram_manager.h"|______________________________________________________
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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <SDL2/SDL.h>

#include "ie_assets.h"
#include "ie_vram.h"
#include "ie_messages.h"

namespace ie
{
  
  class VramManager
  {
    public:
    //BUILDING CPU SIDE VBOS
    void createVbos(void);
    void createStaticVbos(void);
    void createTerrainVbos(void);
    
    //SENDING DATA TO VIDEO HARDWARE
    void loadVbos(void);
    void createAndLoadVbos(void);
    void loadTexture(TextureAsset*);

    //RECEIVING MESSAGES
    void receiveMessage(AssetStatusToVramMessage); 

    //SENDING MESSAGES
    VramStatusToRenderMessage sendVramStatusToRenderMessage();

    //RELEASING VIDEO HARDWARE MEMORY
    void quit(void);

    //GETTERS AND SETTERS
    void setGlContext(SDL_GLContext);

    private:
    //DATA FOR INIT
    SDL_GLContext mainGlContext;

    //VBO ID PAIRS
    VboPair vPair;
    VboPair vnPair;
    VboPair vtnPair;
    VboPair vtncbPair;
    VboPair terrainIndexPair;

    //DATA FROM ASSET MANAGER
    std::map<unsigned int, ModelAsset>* models;
    std::map<unsigned int, TextureAsset>* textures; 
    std::map<unsigned int, MaterialAsset>* materials;
    std::map<unsigned int, TerrainAsset>* terrains;
    std::vector<glm::vec4>* vHeap;
    std::vector<glm::vec3>* tHeap;
    std::vector<glm::vec3>* nHeap;
    std::vector<glm::vec3>* cHeap;
    std::vector<glm::uvec2>* bHeap;
    std::vector<glm::ivec4>* iHeap;

    //CPU SIDE VBO DATA
    std::vector<VFormat> vboV;
    std::vector<VNFormat> vboVN;
    std::vector<VTNFormat> vboVTN;
    std::vector<VTNCBFormat> vboVTNCB;
    std::vector<unsigned int> vboTerrainIndex;
  };

}

#endif
