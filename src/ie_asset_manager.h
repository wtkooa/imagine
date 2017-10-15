#ifndef IE_ASSET_MANAGER_H
#define IE_ASSET_MANAGER_H

//___|"ie_asset_manager.h"|_____________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "ie_assets.h"
#include "ie_messages.h"
#include "ie_packages.h"

namespace ie
{

  class AssetManager 
  {
    public:

    AssetManager();

    //FRAME UPDATE AND INSTRUCTIONS
    void update(void);

    //UNWRAPPING PACKAGES
    void unwrapPackage(ie::WavefrontObjectFilePackage);
    void unwrapPackage(ie::WavefrontMaterialFilePackage);
    GLuint unwrapPackage(ie::WavefrontMaterialPackage);
    GLuint unwrapPackage(ie::TexturePackage);
    void unwrapPackage(ie::ShaderProgramPackage);
    void unwrapPackage(ie::LightPackage);
    void unwrapPackage(ie::TerrainPackage);

    //SENDING MESSAGES
    AssetStatusToVramMessage sendAssetStatusToVramMessage(void); 
    AssetStatusToRenderMessage sendAssetStatusToRenderMessage(void);
    AssetStatusToPlayerMessage sendAssetStatusToPlayerMessage(void);

    //RECEIVING MESSAGES
    void receiveMessage(ie::AssetManagerInstructions);
    void receiveMessage(ie::AssetManagerInstruction);
    
    //CREATING ENTITIES
    void createEntity(std::string, std::string, EntityType);

    //CREATING QUICK RENDER LISTS
    void createQuickLists(void);
    void createStaticQuickLists(void);
    void createTerrainQuickLists(void);

    //ACCESSING MANANGED ASSETS
    handle getHandle(std::string);

    //RELEASING ASSETS
    bool releaseAllTextures(void);
    bool releaseTexture(std::string);
    bool releaseTexture(GLuint);
    bool releaseAllShaders(void);
    bool releaseShader(std::string);
    bool quit(void);

    private:

    //ASSIGNING AND MANAGING ASSET IDS
    unsigned int newAssetId;
    std::list<unsigned int> usedAssetIds; 
    unsigned int assignAssetId(void);
    
    //ENTITIES
    std::map<unsigned int, Entity> entities;
    std::map<std::string, unsigned int> entityNameIdMap;

    //MODELS
    std::map<unsigned int, ModelAsset> modelAssets;
    std::map<std::string, unsigned int> modelNameIdMap;

    //RENDER UNITS
    std::map<unsigned int, RenderUnit> renderUnits;

    //MATERIALS
    std::map<unsigned int, MaterialAsset> materialAssets;
    std::map<std::string, unsigned int> materialNameIdMap;

    //TEXTURES
    std::map<unsigned int, TextureAsset> textureAssets;
    std::map<std::string, unsigned int> textureNameIdMap;

    //SHADERS
    std::map<GLuint, ShaderAsset> shaderAssets;
    std::map<std::string, unsigned int> shaderNameIdMap;

    //LIGHTS
    std::map<unsigned int, LightAsset> lightAssets;
    std::map<std::string, unsigned int> lightNameIdMap;

    //TERRAIN
    std::map<unsigned int, TerrainAsset> terrainAssets;
    std::map<std::string, unsigned int> terrainNameIdMap;

    //VERTEX DATA HEAP
    std::vector<glm::vec4> vertexHeap;
    std::vector<glm::vec3> textureCoordinateHeap;
    std::vector<glm::vec3> normalVectorHeap;
    std::vector<glm::vec3> colorHeap;
    std::vector<glm::uvec2> blendHeap;
    std::vector<glm::ivec4> indexHeap;
    template <class T, class S>
    unsigned int pushToHeap(T*, S*);

    //QUICK LISTS
    std::map<unsigned int, StaticQuickListElement> staticVList;
    std::map<unsigned int, StaticQuickListElement> staticVNList;
    std::map<unsigned int, StaticQuickListElement> staticVTNList;
    std::set<unsigned int> terrainVTNCBList;

    //INSTRUCTION LIST
    std::vector<AssetManagerInstruction> instructions;
  };

}

#endif
