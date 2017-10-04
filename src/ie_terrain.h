#ifndef IE_TERRAIN_H
#define IE_TERRAIN_H

//___|"ie_terrain.h"|___________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec3.hpp>

#include "ie_packages.h"

namespace ie
{

  class TerrainGenerator
  {
    public:
    TerrainGenerator();
    TerrainGenerator(short);
    TerrainGenerator(short, float);

    void generateTerrain(void);
    void generateTerrain(short, float);

    void applyPerlin(float, float, float);

    void addTexture(std::string, std::string); 

    ie::TerrainPackage wrapTerrainPackage(void);

    void setName(std::string);
    void setDim(short);

    private:
    void clear(void);

    std::string name;
    short dim;
    float unitSize;
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::uvec2> blends;
    std::vector<glm::ivec4> indices;
    std::vector<TexturePackage> textures;
  };

}

#endif
