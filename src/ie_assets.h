#pragma once

#include <map>
#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>

#include "ie_messages.h"
#include "ie_packages.h"

namespace ie
{

  class ShaderProgramAsset
  {
    public:
    std::string name;
    GLuint programId;
    GLuint vertexShaderId;
    GLuint fragmentShaderId;
    std::vector<GlslUniformPackage> uniforms;
  };

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
    void unwrapPackage(ie::WavefrontObjectFilePackage);
    GLuint unwrapPackage(ie::WavefrontObjectPackage);
    void unwrapPackage(ie::WavefrontMaterialFilePackage);
    GLuint unwrapPackage(ie::WavefrontMaterialPackage);
    GLuint unwrapPackage(ie::WavefrontTexturePackage);
    void unwrapPackage(ie::ShaderProgramPackage);
    bool releaseAllShaderPrograms(void);
    bool releaseShaderProgram(std::string);
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
  };

}
