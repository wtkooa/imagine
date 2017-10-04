#ifndef IE_RENDER_H
#define IE_RENDER_H

//___|"ie_render.h"|____________________________________________________________
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

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "ie_assets.h"
#include "ie_memory.h"
#include "ie_messages.h"

namespace ie
{

  class RenderManager
  {
    public:
    //RENDERING
    void render(void);
    void renderMaterialedEntities(void);
    void renderTexturedEntities(void);
    void renderTerrainEntities(void);

    //RECEIVING MESSAGES
    void receiveMessage(AssetStatusToRenderMessage);
    void receiveMessage(VramStatusToRenderMessage);
    void receiveMessage(CameraStatusToRenderMessage);

    private:
    //DATA FROM ASSET MANAGER
    std::map<unsigned int, Entity>* entities;
    std::map<unsigned int, MaterialAsset>* materials;
    std::map<unsigned int, ModelAsset>* models;
    std::map<GLuint, ShaderProgramAsset>* shaders;
    std::map<std::string, GLuint>* shaderNameIdMap;
    std::map<unsigned int, LightAsset>* lights;
    std::map<std::string, unsigned int>* lightNameIdMap;
    std::map<unsigned int, TerrainAsset>* terrains;
    std::map<std::string, unsigned int>* terrainNameIdMap;
    std::vector<StaticQuickListElement>* staticVList;
    std::vector<StaticQuickListElement>* staticVNList;
    std::vector<StaticQuickListElement>* staticVTNList;
    std::vector<unsigned int>* terrainVTNCBList;


    //DATA FROM VRAM MANAGER
    std::map<unsigned int, std::vector<StaticRenderUnitLocation>>* staticMemoryMap;
    std::map<unsigned int, TerrainRenderUnitLocation>* terrainIndexMemoryMap;
    VboPair* vPair;
    VboPair* vnPair;
    VboPair* vtnPair;
    VboPair* vtncbPair;
    VboPair* terrainIndexPair;

    //DATA FROM CAMERA
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec3 cameraPos;

  };

}

#endif
