#ifndef IE_ASSET_MANAGER_H
#define IE_ASSET_MANAGER_H

//___|"ie_asset_manager.h"|_____________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "ie_material.h"
#include "ie_mesh.h"
#include "ie_messages.h"
#include "ie_shader.h"
#include "ie_terrain.h"
#include "ie_texture.h"

namespace ie
{

  class AssetManager 
  {
    public:
    AssetManager();

    //RELEASING ASSETS

    private:
    //ASSIGNING AND MANAGING ASSET IDS
    unsigned int newAssetId;
    std::list<unsigned int> usedAssetIds; 
    unsigned int assignAssetId(void);
    
    //MESHES
    std::map<unsigned int, Mesh*> meshAssets;
    std::map<std::string, unsigned int> meshNameIdMap;

    //MATERIALS
    std::map<unsigned int, Material*> materialAssets;
    std::map<std::string, unsigned int> materialNameIdMap;

    //TEXTURES
    std::map<unsigned int, Texture*> textureAssets;
    std::map<std::string, unsigned int> textureNameIdMap;

    //SHADERS
    std::map<GLuint, Shader*> shaderAssets;
    std::map<std::string, unsigned int> shaderNameIdMap;

    //TERRAIN
    std::map<unsigned int, Terrain*> terrainAssets;
    std::map<std::string, unsigned int> terrainNameIdMap;
  };

}

#endif
