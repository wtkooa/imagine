#pragma once

#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>

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

  enum VboDataFormat {V, VN, VTN};

  class RenderUnit
  {
    public:
    unsigned int materialId;
    VboDataFormat dataFormat;
    unsigned int shaderProgramId;
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

}
