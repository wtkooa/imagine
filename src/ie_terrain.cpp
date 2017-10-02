//___|"ie_terrain.cpp"|_________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_terrain.h"

#include <iostream>

#include <glm/gtc/noise.hpp>
#include <glm/mat4x2.hpp>
#include <glm/vec3.hpp>

#include "ie_const.h"
#include "ie_packages.h"

ie::TerrainGenerator::TerrainGenerator(void)
{
  name = "Terrain";
  dim = 100;
  generateTerrain();
}
ie::TerrainGenerator::TerrainGenerator(short d)
{
  name = "Terrain";
  generateTerrain(d);
}


void ie::TerrainGenerator::clear(void)
{
  name = "Terrain";
  dim = 100;
  vertices.clear();
  colors.clear();
  blends.clear();
  indices.clear();
  textures.clear();
}


void ie::TerrainGenerator::generateTerrain() {generateTerrain(dim);}
void ie::TerrainGenerator::generateTerrain(short d)
{
  clear();
  dim = d;
  for (short z = 0; z < dim; z++)
  {
    for (short x = 0; x < dim; x++)
    {
      glm::vec4 vert(float(x), 0.0f, float(z), 1.0f);
      glm::vec3 color(0.6f, 0.6f, 0.6f);
      glm::vec3 normal(0.0f, 1.0f, 0.0f);
      glm::uvec2 blend(0, 0);
      vertices.push_back(vert);
      colors.push_back(color);
      normals.push_back(normal);
      blends.push_back(blend);
    }
  }

  for (short z = 0; z < (dim - 1); z++)
  {
    for (short x = 0; x < (dim - 1); x++)
    {
      unsigned int n = (z * dim) + x;
      unsigned int v1 = n;
      unsigned int v2 = (n + dim);
      unsigned int v3 = (n + dim + 1);
      unsigned int v4 = (n + 1);
      indices.push_back(glm::ivec4(v1, v2, v3, 0));
      indices.push_back(glm::ivec4(v3, v4, v1, 0));
    }
  }
}


void ie::TerrainGenerator::applyPerlin(float seed, float res, float range)
{
  float hackyOffset = 0.001;
  range /= 2.0f;
  for (short z = 0; z < dim; z++)
  {
    for (short x = 0; x < dim; x++)
    {
      unsigned int n = (z * dim) + x;
      glm::vec3 perlVec((float(x) + hackyOffset) / res,
                         float(seed) + hackyOffset,
                        (float(z) + hackyOffset) / res);
      vertices[n].y = glm::perlin(perlVec) * range;
    }
  }
}

void ie::TerrainGenerator::addTexture(std::string filepath,
                                      std::string filename)
{
  if (textures.size() < ie::MAX_TERRAIN_TEXTURES)
  {
    TexturePackage texture;
    texture.filepath = filepath;
    texture.filename = filename;
    texture.type = BUMP_MAP;
    textures.push_back(texture);
  }
  else
  {
    std::cout << "Warning: Max terrian texture capacity (" <<
    ie::MAX_TERRAIN_TEXTURES << ") already reached." << std::endl;
  }
}


ie::TerrainPackage ie::TerrainGenerator::wrapTerrainPackage(void)
{
  ie::TerrainPackage package;
  package.name = name;
  package.dim = dim;
  package.vertices = vertices;
  package.colors = colors;
  package.blends = blends;
  package.indices = indices;
  package.textures = textures;
  return package;
}


void ie::TerrainGenerator::setName(std::string n) {name = n;}
void ie::TerrainGenerator::setDim(short d) {dim = d;}