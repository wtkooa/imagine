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

#include "ie_asset.h"
#include "ie_material.h"
#include "ie_mesh.h"

namespace ie
{

  class Terrain : public Asset
  {
    public:
    Terrain();

    void setMesh(Mesh*);
    void setDim(unsigned int);
    void setUnitSize(unsigned int);
    void addMaterial(Material*);
    Mesh* getMesh(void);
    unsigned int getDim(void);
    unsigned int getUnitSize(void);
    Material* getMaterial(unsigned int);
    unsigned int getMaterialAmount(void);

    virtual void reset(void);

    private:
    Mesh* mesh;
    short dim;
    float unitSize;
    std::vector<Material*> materials;
  };

}

#endif
