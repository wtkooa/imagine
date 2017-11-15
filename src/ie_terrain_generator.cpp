//___|"ie_terrain_generator.h"|_________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_terrain_generator.h"

#include <iostream>

#include <glm/gtc/noise.hpp>
#include <glm/mat4x2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "ie_config.h"

ie::TerrainGenerator::TerrainGenerator()
{
  reset();
}


void ie::TerrainGenerator::reset(void)
{
  manager = NULL;
  workingTerrain = NULL;
  workingMesh = NULL;
  workingRenderUnit = NULL;
}


void ie::TerrainGenerator::setLoadDestination(AssetManager* am)
{
  manager = am;
}


void ie::TerrainGenerator::generateTerrain()
{
  generateTerrain(ie::DEFAULT_TERRAIN_DIM, ie::DEFAULT_TERRAIN_UNITS); 
}
void ie::TerrainGenerator::generateTerrain(short dim, short unitSize)
{
  workingTerrain = new Terrain();
  workingTerrain->setDim(dim);
  workingTerrain->setUnitSize(unitSize);
  workingMesh = new Mesh();
  workingRenderUnit = new RenderUnit();

  for (short z = 0; z < dim; z++)
  {
    for (short x = 0; x < dim; x++)
    {
      glm::vec3 position(x * unitSize, 0.0f, z * unitSize);
      glm::vec3 color(0.0f , 0.0f, 0.0f);
      glm::vec3 normal(0.0f, 1.0f, 0.0f);
      glm::u8vec4 data1(255, 0, 0, 0);
      glm::u8vec4 data2(0, 0, 0, 0);
      workingRenderUnit->addPositionAttrib(position);
      workingRenderUnit->addUnpackedColorAttrib(color);
      workingRenderUnit->addUnpackedNormalAttrib(normal);
      workingRenderUnit->addUnpackedDataAttrib(1, data1);
      workingRenderUnit->addUnpackedDataAttrib(2, data2);
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
      workingRenderUnit->addIndexAttrib(v1);
      workingRenderUnit->addIndexAttrib(v2);
      workingRenderUnit->addIndexAttrib(v3);
      workingRenderUnit->addIndexAttrib(v3);
      workingRenderUnit->addIndexAttrib(v4);
      workingRenderUnit->addIndexAttrib(v1);
    }
  }
}


void ie::TerrainGenerator::setName(std::string name)
{
  workingTerrain->setName(name);
  workingMesh->setName(name);
}


void ie::TerrainGenerator::addMaterial(Material* material)
{
    workingTerrain->addMaterial(material);
}


void ie::TerrainGenerator::applyPerlin(float seed, float res, float range)
{
  RenderUnit* bufferRenderUnit = new RenderUnit();
  short dim = workingTerrain->getDim();
  
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
      float y = glm::perlin(perlVec) * range;
      glm::vec3 newPosition = glm::vec3((workingRenderUnit->getPositionAttrib(n)).x,
                                        y,
                                        (workingRenderUnit->getPositionAttrib(n)).z);
      bufferRenderUnit->addPositionAttrib(newPosition);
    }
  }

  workingRenderUnit->clearAttrib(IE_POSITION_ATTRIB);
  unsigned int vertexAmount = bufferRenderUnit->getAttribAmount(IE_POSITION_ATTRIB);
  for (unsigned int n = 0; n < vertexAmount; n++)
  {
    glm::vec3 newPosition = bufferRenderUnit->getPositionAttrib(n);
    workingRenderUnit->addPositionAttrib(newPosition);
  }

  delete bufferRenderUnit;
  calcFaceNormals();
}


void ie::TerrainGenerator::calcFaceNormals(void)
{
  workingRenderUnit->clearAttrib(IE_NORMAL_ATTRIB);
  short dim = workingTerrain->getDim();

  for (short z = 0; z < (dim - 1); z++)
  {
    for (short x = 0; x < (dim - 1); x++)
    {
      unsigned int n = (z * dim) + x;
      glm::vec3 v1 = workingRenderUnit->getPositionAttrib(n);
      glm::vec3 v2 = workingRenderUnit->getPositionAttrib(n+dim);
      glm::vec3 v3 = workingRenderUnit->getPositionAttrib(n+dim+1);
      glm::vec3 v4 = workingRenderUnit->getPositionAttrib(n+1);
      glm::vec3 n1 = glm::normalize(cross((v2-v1), (v3-v1)));
      glm::vec3 n2(0.0f, 0.0f, 0.0f);
      glm::vec3 n3 = glm::normalize(cross((v3-v1), (v4-v1)));
      workingRenderUnit->addUnpackedNormalAttrib(n1);
      workingRenderUnit->addUnpackedNormalAttrib(n2);
      workingRenderUnit->addUnpackedNormalAttrib(n3);
      workingRenderUnit->addUnpackedNormalAttrib(n3);
      workingRenderUnit->addUnpackedNormalAttrib(n2);
      workingRenderUnit->addUnpackedNormalAttrib(n1);
    }
  }
}


void ie::TerrainGenerator::smoothNormals(void)
{
  short dim = workingTerrain->getDim();
  std::vector<glm::vec3> normals;

  for (unsigned int n = 0; n < dim * dim; n++)
  {
    normals.push_back(glm::vec3());
  }
  
  unsigned int indexAmount = workingRenderUnit->getAttribAmount(IE_ELEMENT_ATTRIB);
  for (unsigned int face = 0; face < indexAmount; face += 3)
  {
      unsigned int vert = workingRenderUnit->getIndexAttrib(face); 
      normals[vert] += workingRenderUnit->getUnpackedNormalAttrib(face);
      vert = workingRenderUnit->getIndexAttrib(face+1); 
      normals[vert] += workingRenderUnit->getUnpackedNormalAttrib(face); 
      vert = workingRenderUnit->getIndexAttrib(face+2); 
      normals[vert] += workingRenderUnit->getUnpackedNormalAttrib(face);
  }

  workingRenderUnit->clearAttrib(IE_NORMAL_ATTRIB);
  for (unsigned int n = 0; n < dim * dim; n++)
  {
    workingRenderUnit->addUnpackedNormalAttrib(glm::normalize(normals[n]));
  }
}


void ie::TerrainGenerator::loadTerrain(void)
{
  workingMesh->addRenderUnit(workingRenderUnit);
  workingTerrain->setMesh(workingMesh);
  bool loadedTerrain = manager->load(workingTerrain);

  if (loadedTerrain)
  {
    manager->load(workingMesh);

    unsigned int materialAmount = workingTerrain->getMaterialAmount();
    for (unsigned int n = 0; n < materialAmount; n++)
    {
      manager->load(workingTerrain->getMaterial(n));
    }
  }

  workingTerrain = NULL;
  workingMesh = NULL;
  workingRenderUnit = NULL;
}


void ie::TerrainGenerator::quit(void)
{
  if (workingTerrain != NULL) {delete workingTerrain;}
  if (workingMesh != NULL) {delete workingMesh;}
  if (workingRenderUnit != NULL) {delete workingRenderUnit;}
}
