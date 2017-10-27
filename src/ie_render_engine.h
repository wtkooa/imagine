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
#include "ie_messages.h"
#include "ie_nodes.h"
#include "ie_scenegraph.h"
#include "ie_vram.h"

namespace ie
{

  class RenderEngine
  {
    public:
    //RENDERING
    void render(void);
    void renderMaterialedEntities(std::vector<NodePacket>*);
    void renderTexturedEntities(std::vector<NodePacket>*);
    void renderTerrainEntities(std::vector<NodePacket>*);
    void updateCamera(CameraNode*);
    void setState(RenderState*);

    //RECEIVING MESSAGES
    void receiveMessage(AssetStatusMessage);
    void receiveMessage(VramStatusToRenderMessage);
    void receiveMessage(GraphStatusMessage);

    //SETTERS AND GETTERS
    void setShader(std::string, ShaderType);

    private:
    //DATA FROM ASSET MANAGER
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

    //DATA FROM SCENEGRAPH
    RenderBucket* firstBucket;
    
    //SHADERS
    ShaderAsset* staticShader;
    ShaderAsset* terrainShader;
    ShaderAsset* currentShader;
  };

}

#endif
