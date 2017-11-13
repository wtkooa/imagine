#ifndef IE_WAVEFRONT_H
#define IE_WAVEFRONT_H

//___|"ie_wavefront.h"|_________________________________________________________
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

#include "ie_asset_manager.h"
#include "ie_material.h"
#include "ie_mesh.h"
#include "ie_texture.h"

namespace ie
{

  class WavefrontLoader
  {
    public:
    WavefrontLoader();

    void reset(void);
    void quit(void);

    void setLoadDestination(AssetManager*);

    void load(std::string);
    void load(std::string, std::string);
    
    private:
    AssetManager* manager;
    void loadObj(std::string, std::string);
    void loadMtl(std::string, std::string);

    void mtllib(std::string, std::vector<std::string>);
    void loadObject(void);
    void loadRenderUnit(void);
    void procRenderUnitBuffer(void);
    void object(std::vector<std::string>, std::string, std::string);
    void position(std::vector<std::string>);
    void mapping(std::vector<std::string>);
    void normal(std::vector<std::string>);
    void usemtl(std::vector<std::string>);
    void face(std::vector<std::string>);

    void loadMaterial(void);
    void newmtl(std::vector<std::string>, std::string, std::string);
    void shininess(std::vector<std::string>);
    void ambient(std::vector<std::string>);
    void diffuse(std::vector<std::string>);
    void specular(std::vector<std::string>);
    void emission(std::vector<std::string>);
    Texture* loadTexture(std::vector<std::string>);
    void map_Kd(std::vector<std::string>);
    void map_bump(std::vector<std::string>);

    Mesh* workingMesh;
    RenderUnit* workingRenderUnit;
    RenderUnit* bufferRenderUnit;
    Material* workingMaterial;
    Texture* workingTexture;
  };

}

#endif
