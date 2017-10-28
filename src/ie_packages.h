#ifndef IE_PACKAGES_H
#define IE_PACKAGES_H

//___|"ie_packages.h"|__________________________________________________________
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

#define GL_GLEXT_PROTOTYPES //Needs to be define for some GL func to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/mat4x2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace ie
{
  
  enum TextureType {DIFFUSE_MAP, BUMP_MAP, ALPHA_MAP,
                    AMBIENT_MAP, SPECULAR_MAP, HIGHLIGHT_MAP};


  class TexturePackage
  {
    public:
    std::string filename;
    std::string filepath;
    TextureType type;
    bool mipmapped;
    bool anisotropy;
    bool repeating;
  };


  class TerrainPackage
  {
    public:
    std::string name;
    short dim;
    float unitSize;
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::uvec2> blends;
    std::vector<glm::ivec4> indices;
    std::vector<TexturePackage> textures;
    float shininess;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emission;
  };

  class LightPackage
  {
    public:
    std::string name;
    glm::vec3 posVector;
    glm::vec3 globalAmbient;
    glm::vec3 lightAmbient;
    glm::vec3 lightSpecular;
    glm::vec3 lightDiffuse;
    float constantFalloff;
    float linearFalloff;
    float quadraticFalloff;
  };


  class GlslUniformPackage
  {
    public:
    std::string name;
    std::string type;
    unsigned int arraySize;
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
    std::vector<TexturePackage> texturePackages;
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

#endif
