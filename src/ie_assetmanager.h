#ifndef IE_ASSETMANAGER_H
#define IE_ASSETMANAGER_H

//___|"ie_assetmanager.h"|______________________________________________________
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
    AssetStatusToRenderMessage sendAssetStatusToRenderMessage();
    
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
    bool releaseAllShaderPrograms(void);
    bool releaseShaderProgram(std::string);
    bool quit(void);

    private:
    //Entities
    std::map<unsigned int, Entity> entities;
    std::map<std::string, unsigned int> entityNameIdMap;
    std::vector<unsigned int> availableEntityIds; 
    unsigned int getNewEntityId(void);

    //MODELS
    std::map<unsigned int, ModelAsset> modelAssets;
    std::map<std::string, unsigned int> modelNameIdMap;
    std::vector<unsigned int> availableModelIds; 
    unsigned int getNewModelAssetId(void);

    //MATERIALS
    std::map<unsigned int, MaterialAsset> materialAssets;
    std::map<std::string, unsigned int> materialNameIdMap;
    std::vector<unsigned int> availableMaterialIds; 
    unsigned int getNewMaterialAssetId(void);

    //TEXTURES
    std::map<unsigned int, TextureAsset> textureAssets;
    std::map<std::string, GLuint> textureNameIdMap;

    //SHADERS
    std::map<GLuint, ShaderProgramAsset> shaderProgramAssets;
    std::map<std::string, GLuint> shaderNameIdMap;

    //LIGHTS
    std::map<unsigned int, LightAsset> lightAssets;
    std::map<std::string, unsigned int> lightNameIdMap;
    std::vector<unsigned int> availableLightIds;
    unsigned int getNewLightAssetId(void);

    //TERRAIN
    std::map<unsigned int, TerrainAsset> terrainAssets;
    std::map<std::string, unsigned int> terrainNameIdMap;
    std::vector<unsigned int> availableTerrainIds; 
    unsigned int getNewTerrainAssetId(void);

    //VERTEX DATA HEAP
    std::vector<glm::vec4> vertexHeap;
    std::vector<glm::vec3> textureCoordinateHeap;
    std::vector<glm::vec3> normalVectorHeap;
    std::vector<glm::vec3> colorHeap;
    std::vector<glm::vec2> blendHeap;
    std::vector<glm::ivec4> indexHeap;
    unsigned int pushVertexData(std::vector<glm::vec4>);
    unsigned int pushTextureCoordinateData(std::vector<glm::vec3>);
    unsigned int pushNormalVectorData(std::vector<glm::vec3>);
    unsigned int pushColorData(std::vector<glm::vec3>);
    unsigned int pushBlendData(std::vector<glm::uvec2>);
    unsigned int pushIndexData(std::vector<glm::ivec4>);

    //QUICK LISTS
    std::vector<StaticQuickListElement> staticVList;
    std::vector<StaticQuickListElement> staticVNList;
    std::vector<StaticQuickListElement> staticVTNList;
  };

}

#endif
