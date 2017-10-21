#ifndef IE_PHYSICS_ENGINE_H
#define IE_PHYSICS_ENGINE_H

//___|"ie_physics_engine.h"|____________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "ie_controller.h"
#include "ie_messages.h"
#include "ie_nodes.h"

namespace ie
{

  class PhysicsEngine
  {
    public:

    void update(void);
    void updatePlayers(PhysicsBucket*);
    void updateTerrains(PhysicsBucket*);
    void updateTerrain(EntityNode*);
    void updatePlayerFirstPerson(PlayerNode*);
    void updatePlayerTerrainInteraction(PlayerNode*);

    float calcBarycentric(glm::vec4, glm::vec4, glm::vec4, glm::vec2);

    void setController(Controller*);

    void receiveMessage(ie::TimeStatusMessage);
    void receiveMessage(ie::ControllerStatusMessage);
    void receiveMessage(ie::GraphStatusMessage);
    void receiveMessage(ie::AssetStatusMessage);

    private:
    float frameDelta;
    PhysicsBucket* firstBucket;
    Controller* control;
    std::vector<EntityNode*> collidableTerrain;
    
    //DATA FROM THE ASSET MANAGER
    std::map<unsigned int, ModelAsset>* models;
    std::map<std::string, unsigned int>* modelNameIdMap;
    std::map<unsigned int, TerrainAsset>* terrains;
    std::map<std::string, unsigned int>* terrainNameIdMap;
    std::map<unsigned int, LightAsset>* lights;
    std::map<std::string, unsigned int>* lightNameIdMap;
    std::map<unsigned int, MaterialAsset>* materials;
    std::map<std::string, unsigned int>* materialNameIdMap;
    std::map<unsigned int, RenderUnit>* rus;
    std::vector<glm::ivec4>* iHeap;
    std::vector<glm::vec4>* vHeap;

  };
}

#endif
