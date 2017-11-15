#ifndef IE_CREATION__H
#define IE_CREATION__H

//___|"ie_creation.h"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_asset_manager.h"
#include "ie_terrain_generator.h"

namespace ie
{

  class CreationManager
  {
    public:
    CreationManager();

    void reset(void);
    void quit(void);

    void setLoadDestination(AssetManager*);

    TerrainGenerator terrainEditor;

    private:
    AssetManager* manager; 

  };

}

#endif
