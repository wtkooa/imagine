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

#include <cmath>
#include <iostream>

#include <glm/gtc/noise.hpp>
#include <glm/mat4x2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "ie_const.h"
#include "ie_packages.h"

ie::TerrainGenerator::TerrainGenerator(void)
{
  clear();
  generateTerrain();
}
ie::TerrainGenerator::TerrainGenerator(short d)
{
  clear();
  generateTerrain(d, unitSize);
}
ie::TerrainGenerator::TerrainGenerator(short d, float u)
{
  generateTerrain(d, u);
}


void ie::TerrainGenerator::clear(void)
{
  name = "Terrain";
  dim = 100;
  unitSize = 1;
  vertices.clear();
  colors.clear();
  blends.clear();
  indices.clear();
  textures.clear();
  shininess = 1;
  ambient = glm::vec3(1.0f, 1.0f, 1.0f);
  diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
  specular = glm::vec3(0.0f, 0.0f, 0.0f);
  emission = glm::vec3(0.0f, 0.0f, 0.0f);
}


void ie::TerrainGenerator::generateTerrain() {generateTerrain(dim, unitSize);}
void ie::TerrainGenerator::generateTerrain(short d, float u)
{
  clear();
  dim = d;
  unitSize = u;
  for (short z = 0; z < dim; z++)
  {
    for (short x = 0; x < dim; x++)
    {
      float xv = x - (float(dim) / 2.0);
      float zv = z - (float(dim) / 2.0);
      glm::vec4 vert(xv * unitSize, 0.0f, zv * unitSize, 1.0f);
      srand(x*z);
      glm::vec3 color(0.0f , 0.0f, 0.0f);
      glm::vec3 normal(0.0f, 1.0f, 0.0f);
      glm::uvec2 blend(99, 0);
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
  calcFaceNormals();
}


void ie::TerrainGenerator::calcFaceNormals(void)
{
  faceNormals.clear();
  for (short z = 0; z < (dim - 1); z++)
  {
    for (short x = 0; x < (dim - 1); x++)
    {
      unsigned int n = (z * dim) + x;
      glm::vec3 v1(vertices[n].x, vertices[n].y, vertices[n].z);
      glm::vec3 v2(vertices[n+dim].x, vertices[n+dim].y, vertices[n+dim].z);
      glm::vec3 v3(vertices[n+dim+1].x, vertices[n+dim+1].y, vertices[n+dim+1].z);
      glm::vec3 v4(vertices[n+1].x, vertices[n+1].y, vertices[n+1].z);
      glm::vec3 n1 = glm::normalize(cross((v2-v1), (v3-v1)));
      glm::vec3 n2 = glm::normalize(cross((v3-v1), (v4-v1)));
      faceNormals.push_back(n1);
      faceNormals.push_back(n2);
    }
  }
  smoothNormals();
}


void ie::TerrainGenerator::smoothNormals(void)
{
  normals.clear();
  for (unsigned int n = 0; n < dim * dim; n++)
  {
    normals.push_back(glm::vec3());
  }
  
  for (unsigned int face = 0; face < indices.size(); face++)
  {
      unsigned int vert = indices[face].x; 
      normals[vert] += faceNormals[face];
      vert = indices[face].y; 
      normals[vert] += faceNormals[face];
      vert = indices[face].z; 
      normals[vert] += faceNormals[face];
  }

  for (unsigned int n = 0; n < dim * dim; n++)
  {
    normals[n] = glm::normalize(normals[n]);
  }
}


void ie::TerrainGenerator::applyDemoBlends(void)
{
  short octalSize = (dim / 8);
  unsigned int blendAmount = dim * dim;
  for (unsigned int nBlend = 0; nBlend < blendAmount; nBlend++)
  {
    blends[nBlend] = glm::uvec2(0, 0);
  }

  for (short z = 0; z < dim; z++)
  {
    for (short x = 0; x < dim; x++)
    {
      unsigned int n = (z * dim) + x;
      short octal = short(x / octalSize);
      blends[n] = setBlendValue(blends[n], octal, 49); 
    }
  }

  for (short z = 0; z < dim; z++)
  {
    for (short x = 0; x < dim; x++)
    {
      unsigned int n = (z * dim) + x;
      short octal = short(z / octalSize);
      blends[n] = setBlendValue(blends[n], octal, 49); 
    }
  }
}


glm::uvec2 ie::TerrainGenerator::setBlendValue(glm::uvec2 blend,
                                               short tex, short value)
{
  tex = 7 - tex;
  if (tex < 4)
  {
    blend.y += value * std::pow(100, tex);
  }
  else
  {
    tex -= 4;
    blend.x += value * std::pow(100, tex);
  }
  return blend;
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
  package.unitSize = unitSize;
  package.vertices = vertices;
  package.normals = normals;
  package.colors = colors;
  package.blends = blends;
  package.indices = indices;
  package.textures = textures;
  package.shininess = shininess;
  package.ambient = ambient;
  package.diffuse = diffuse;
  package.emission = emission;
  return package;
}


void ie::TerrainGenerator::setName(std::string n) {name = n;}
void ie::TerrainGenerator::setDim(short d) {dim = d;}
void ie::TerrainGenerator::setShininess(float s) {shininess = s;}
void ie::TerrainGenerator::setAmbient(glm::vec3 a) {ambient = a;}
void ie::TerrainGenerator::setDiffuse(glm::vec3 d) {diffuse = d;}
void ie::TerrainGenerator::setSpecular(glm::vec3 s) {specular = s;}
void ie::TerrainGenerator::setEmission(glm::vec3 e) {emission = e;}
