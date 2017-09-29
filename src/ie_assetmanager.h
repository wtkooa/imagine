#pragma once

#include <map>
#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>

#include "ie_assets.h"
#include "ie_messages.h"
#include "ie_packages.h"

namespace ie
{

  class AssetManager 
  {
    public:
    void unwrapPackage(ie::WavefrontObjectFilePackage);
    GLuint unwrapPackage(ie::WavefrontObjectPackage);
    void unwrapPackage(ie::WavefrontMaterialFilePackage);
    GLuint unwrapPackage(ie::WavefrontMaterialPackage);
    GLuint unwrapPackage(ie::WavefrontTexturePackage);
    void unwrapPackage(ie::ShaderProgramPackage);

    CreateVboMessage sendCreateVboMessage(void); 
    RenderAssetMessage sendRenderAssetMessage(std::string, std::string);

    bool releaseAllTextures(void);
    bool releaseTexture(std::string);
    bool releaseTexture(GLuint);
    bool releaseAllShaderPrograms(void);
    bool releaseShaderProgram(std::string);
    
    void createQuickLists(void);

    handle getHandle(std::string);

    bool quit(void);

    private:
    std::map<unsigned int, ModelAsset> modelAssets;
    std::map<std::string, unsigned int> modelNameIdMap;
    std::vector<unsigned int> availableModelIds; 
    unsigned int getNewModelAssetId(void);

    std::map<unsigned int, MaterialAsset> materialAssets;
    std::map<std::string, unsigned int> materialNameIdMap;
    std::vector<unsigned int> availableMaterialIds; 
    unsigned int getNewMaterialAssetId(void);

    std::map<unsigned int, TextureAsset> textureAssets;
    std::map<std::string, GLuint> textureNameIdMap;

    std::map<std::string, ShaderProgramAsset> shaderProgramAssets;

    std::vector<glm::vec4> vertexHeap;
    unsigned int pushVertexData(std::vector<glm::vec4>);
    std::vector<glm::vec3> textureCoordinateHeap;
    unsigned int pushTextureCoordinateData(std::vector<glm::vec3>);
    std::vector<glm::vec3> normalVectorHeap;
    unsigned int pushNormalVectorData(std::vector<glm::vec3>);
    std::vector<glm::ivec4> indexHeap;
    unsigned int pushIndexData(std::vector<glm::ivec4>);

    std::map<std::string, std::vector<QuickListElement>> quickLists; 
  };

}
