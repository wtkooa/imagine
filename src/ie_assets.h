#pragma once

#include <string>
#include <map>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>

#include "ie_definitions.h"
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
    std::map<std::string, GlslUniformPackage> uniforms;
  };

  class RenderUnit
  {
    public:
    unsigned int materialId;
    VboDataFormat dataFormat;
    std::string shaderProgram;
    unsigned int indexOffset;
    unsigned int vertexAmount;
    bool hidden;
  };

  class ModelAsset
  {
    public:
    unsigned int modelId;
    std::string name;
    std::string filename; 
    std::string filepath;
    std::vector<RenderUnit> renderUnits;
    glm::mat4 translationMatrix;
    glm::mat4 rotationMatrix;
    bool copy;
    bool hidden;
    bool tobeVramLoaded;
    bool vramLoaded;
  };


  class MaterialAsset
  {
    public:
    unsigned int materialId; 
    std::string name;
    std::string filename;
    std::string filepath;
    float shininess;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emission;
    float opticalDensity;
    float dissolve;
    int illum;
    bool containsTexture;
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
    std::string filepath;
    std::string name;
    TextureType textureType; 
    bool tobeVramLoaded;
    bool vramLoaded;
  };

  union handle 
  {
    std::map<unsigned int, ModelAsset>* models;
    std::map<unsigned int, MaterialAsset>* materials;
    std::map<unsigned int, TextureAsset>* textures;
    std::map<std::string, ShaderProgramAsset>* shaders;

    ModelAsset* model;
    MaterialAsset* material;
    TextureAsset* texture;
    ShaderProgramAsset* shader;
    RenderUnit* ru;
    GlslUniformPackage* uniform;

  };

}
