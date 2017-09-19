#pragma once

#include <map>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace ie
{

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
