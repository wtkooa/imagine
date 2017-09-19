#pragma once

#include <map>
#include <string>
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>

#include "ie_packages.h"

namespace ie
{

  class RenderUnit
  {
    public:
    unsigned int materialId;
    unsigned int shaderProgramId;
    unsigned int indexOffset;
    unsigned int vertexAmount;
  };


  class ModelAsset
  {
    public:
    unsigned int modelId;
    std::string name;
    std::string filename; 
    std::vector<RenderUnit> renderUnits;
    glm::mat4 translationMatrix;
    glm::mat4 rotationMatrix;
    bool copy;
    bool hidden;
    bool tobeVboLoaded;
    bool vboLoaded;
  };


  class MaterialAsset
  {
    public:
    unsigned int materialId; 
    std::string name;
    std::string filename;
    float shininess;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emission;
    float opticalDensity;
    float dissolve;
    int illum;
    GLuint ambientMapId;
    GLuint diffuseMapId; 
    GLuint specularMapId;
    GLuint highlightMapId;
    GLuint alphaMapId;
    GLuint bumpMapId;
  };


  class TextureAsset
  {
    public:
    unsigned int textureAssetId;
    GLuint textureOpenglId;
    std::string filename;
    std::string name;
    TextureType textureType; 
    bool tobeVramLoaded;
    bool vramLoaded;
  };


  class AssetManager 
  {
    public:
    void unwrapWavefrontObjectFilePackage(ie::WavefrontObjectFilePackage);
    GLuint unwrapWavefrontObjectPackage(ie::WavefrontObjectPackage);
    void unwrapWavefrontMaterialFilePackage(ie::WavefrontMaterialFilePackage);
    GLuint unwrapWavefrontMaterialPackage(ie::WavefrontMaterialPackage);
    GLuint unwrapWavefrontTexturePackage(ie::WavefrontTexturePackage);
    //private:
    std::map<unsigned int, ModelAsset> modelAssets;
    std::map<std::string, unsigned int> modelNameIdMap;
    std::vector<unsigned int> availableModelIds; 
    unsigned int getNewModelAssetId(void);

    std::map<unsigned int, MaterialAsset> materialAssets;
    std::map<std::string, unsigned int> materialNameIdMap;
    std::vector<unsigned int> availableMaterialIds; 
    unsigned int getNewMaterialAssetId(void);

    std::map<unsigned int, TextureAsset> textureAssets;
    std::map<std::string, unsigned int> textureNameIdMap;
    std::vector<unsigned int> availableTextureIds; 
    unsigned int getNewTextureAssetId(void);

    std::vector<glm::vec4> vertexHeap;
    unsigned int pushVertexData(std::vector<glm::vec4>);
    std::vector<glm::vec3> textureCoordinateHeap;
    unsigned int pushTextureCoordinateData(std::vector<glm::vec3>);
    std::vector<glm::vec3> normalVectorHeap;
    unsigned int pushNormalVectorData(std::vector<glm::vec3>);
    std::vector<glm::ivec4> indexHeap;
    unsigned int pushIndexData(std::vector<glm::ivec4>);
  };

}
