#ifndef IE_TERRAIN_EDITOR_H
#define IE_TERRAIN_EDITOR_H

//___|"ie_terrain_editor.h"|____________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_asset_manager.h"
#include "ie_communication.h"
#include "ie_log.h"
#include "ie_messages.h"
#include "ie_texture.h"

namespace ie
{

  class TerrainEditor : public Icommunication
  {
    public:
    TerrainEditor();

    void reset(void);
    void init(void);
    void quit(void);

    void setLoadDestination(AssetManager*);

    void rxMsg(Imessage*);

    void generate(void);
    void generate(short, short);

    void setName(std::string);
    void addMaterial(Material*);
    void loadTerrain(void);

    void applyPerlin(float, float, float);
    void calcFaceNormals(void);
    void smoothNormals(void);

    private:
    AssetManager* manager;

    Terrain* workingTerrain;
    Mesh* workingMesh;
    RenderUnit* workingRenderUnit;

  };

}

#endif
