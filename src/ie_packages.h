#pragma once

#include <map>
#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be define for some GL func to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>

namespace ie
{
  
  class GlslUniformPackage
  {
    public:
    std::string name;
    std::string type;
    GLint location;
  };

  class GlslSourcePackage
  {
    public:
    std::string filename;
    std::string filepath;
    std::string sourceCode;
  };
  
  class ShaderProgramPackage
  {
    public:
    std::string name;
    std::string vShaderFilename;
    std::string vShaderFilepath;
    std::string fShaderFilename;
    std::string fShaderFilepath;
    GLuint programId;
    GLuint vertexShaderId;
    GLuint fragmentShaderId;
    std::map<std::string, GlslUniformPackage> uniforms;
  };

  enum TextureType {DIFFUSE_MAP, BUMP_MAP, ALPHA_MAP,
                    AMBIENT_MAP, SPECULAR_MAP, HIGHLIGHT_MAP};

  class WavefrontTexturePackage
  {
    public:
    std::string filename;
    TextureType type;
  };

  class WavefrontMaterialPackage
  {
    public:
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
    std::vector<WavefrontTexturePackage> texturePackages;
  };


  class WavefrontMaterialFilePackage
  {
    public:
    std::vector<WavefrontMaterialPackage> materials;
  };


  class WavefrontObject
  {
    public:
    std::string filename;
    std::string filepath;
    std::map<int, std::string> objectGroups;
    std::map<int, std::string> materialGroups;
    std::map<int, int> smoothGroups;
    std::vector<glm::vec4> v;
    std::vector<glm::vec3> vt;
    std::vector<glm::vec3> vn;
    std::vector<glm::ivec4> f;  
  };


  class WavefrontObjectPackage : public WavefrontObject
  {
    public:
    std::string name;
    std::vector<WavefrontMaterialFilePackage> materialPackages;
  };


  class WavefrontObjectFilePackage : public WavefrontObject
  {
    public:
    std::map<int, std::string> objects;
    std::vector<WavefrontMaterialFilePackage> materialFilePackages;
  };

}
