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
    GLuint unwrapPackage(ie::WavefrontTexturePackage);
    void unwrapPackage(ie::ShaderProgramPackage);

    //SENDING MESSAGES
    CreateVboMessage sendCreateVboMessage(void); 
    RenderAssetMessage sendRenderAssetMessage(std::string, std::string);
    
    //CREATING QUICK RENDER LISTS
    void createQuickLists(void);

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
    std::map<std::string, ShaderProgramAsset> shaderProgramAssets;

    //VERTEX DATA HEAP
    std::vector<glm::vec4> vertexHeap;
    std::vector<glm::vec3> textureCoordinateHeap;
    std::vector<glm::vec3> normalVectorHeap;
    std::vector<glm::ivec4> indexHeap;
    unsigned int pushVertexData(std::vector<glm::vec4>);
    unsigned int pushTextureCoordinateData(std::vector<glm::vec3>);
    unsigned int pushNormalVectorData(std::vector<glm::vec3>);
    unsigned int pushIndexData(std::vector<glm::ivec4>);

    //QUICK LISTS
    std::map<std::string, std::vector<QuickListElement>> quickLists; 
  };

}

#endif
