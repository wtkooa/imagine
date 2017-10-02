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
  dim = 100;
  vertices.clear();
  colors.clear();
  blends.clear();
  index.clear();
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
      glm::vec3 vert(float(x), 0.0f, float(z));
      glm::vec3 color(0.6f, 0.6f, 0.6f);
      glm::vec3 normal(0.0f, 1.0f, 0.0f);
      glm::uvec2 blend(99000000, 0);
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
      index.push_back(n);
      index.push_back(n + dim);
      index.push_back(n + dim + 1);
      index.push_back(n + dim + 1);
      index.push_back(n + 1);
      index.push_back(n);
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


ie::TerrainPackage ie::TerrainGenerator::wrapTerrainPackage(void)
{
  ie::TerrainPackage package;
  package.name = name;
  package.dim = dim;
  package.vertices = vertices;
  package.colors = colors;
  package.blends = blends;
  package.index = index;
  return package;
}


void ie::TerrainGenerator::setName(std::string n) {name = n;}
void ie::TerrainGenerator::setDim(short d) {dim = d;}
