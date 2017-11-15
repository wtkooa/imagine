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

#include "ie_config.h"
#include "ie_enum.h"
#include "ie_material.h"
#include "ie_mesh.h"

ie::Terrain::Terrain()
{
  reset();
}


void ie::Terrain::reset(void)
{
  mesh = NULL;
  dim = 0;
  unitSize = 0;
  materials.clear();
}


void ie::Terrain::setMesh(Mesh* m) {mesh = m;}
ie::Mesh* ie::Terrain::getMesh(void) {return mesh;}


void ie::Terrain::setDim(unsigned int d) {dim = d;}
unsigned int ie::Terrain::getDim(void) {return dim;}


void ie::Terrain::setUnitSize(unsigned int u) {unitSize = u;}
unsigned int ie::Terrain::getUnitSize(void) {return unitSize;}


void ie::Terrain::addMaterial(Material* material)
{
  if (materials.size() < ie::MAX_TERRAIN_MATERIALS)
  {
    materials.push_back(material);
  }
  else
  {
    std::cout << "Warning: Max number of terrain materials already present."  << std::endl;
  }
}
ie::Material* ie::Terrain::getMaterial(unsigned int index) {return materials[index];}
unsigned int ie::Terrain::getMaterialAmount(void) {return materials.size();}
