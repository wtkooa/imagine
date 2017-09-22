#pragma once

#include <map>
#include <vector>

#include <glm/glm.hpp>

#include "ie_assets.h"

namespace ie
{

  class CreateVboMessage
  {
    public:
    std::map<unsigned int, ModelAsset>* models;
    std::map<unsigned int, TextureAsset>* textures; 
    std::vector<glm::vec4>* vHeap;
    std::vector<glm::vec3>* tHeap;
    std::vector<glm::vec3>* nHeap;
    std::vector<glm::ivec4>* iHeap;
  };

}
