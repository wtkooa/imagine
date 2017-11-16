#ifndef IE_TERRAIN_GENERATOR_H
#define IE_TERRAIN_GENERATOR_H

//___|"ie_terrain_generator.h"|_________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_asset_manager.h"
#include "ie_log.h"
#include "ie_texture.h"

namespace ie
{

  class TerrainGenerator
  {
    public:
    TerrainGenerator();

    void reset(void);
    void quit(void);

    void setLog(Log*);
    void setLoadDestination(AssetManager*);

    void generateTerrain(void);
    void generateTerrain(short, short);

    void setName(std::string);
    void addMaterial(Material*);
    void loadTerrain(void);

    void applyPerlin(float, float, float);
    void calcFaceNormals(void);
    void smoothNormals(void);

    private:
    Log* log;
    AssetManager* manager;

    Terrain* workingTerrain;
    Mesh* workingMesh;
    RenderUnit* workingRenderUnit;

  };

}

#endif
