#ifndef IE_RENDER_ENGINE_H
#define IE_RENDER_ENGINE_H

//___|"ie_render_engine.h"|_____________________________________________________
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
#include "ie_vram.h"
#include "ie_messages.h"

namespace ie
{

  class RenderEngine
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
    std::map<unsigned int, RenderUnit>* rus;
    std::map<GLuint, ShaderAsset>* shaders;
    std::map<std::string, unsigned int>* shaderNameIdMap;
    std::map<unsigned int, LightAsset>* lights;
    std::map<std::string, unsigned int>* lightNameIdMap;
    std::map<unsigned int, TerrainAsset>* terrains;
    std::map<std::string, unsigned int>* terrainNameIdMap;
    std::map<unsigned int, TextureAsset>* textures;
    std::map<std::string, unsigned int>* textureNameIdMap;
    std::map<unsigned int, StaticQuickListElement>* staticVList;
    std::map<unsigned int, StaticQuickListElement>* staticVNList;
    std::map<unsigned int, StaticQuickListElement>* staticVTNList;
    std::set<unsigned int>* terrainVTNCBList;

    //DATA FROM VRAM MANAGER
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
