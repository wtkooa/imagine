#ifndef IE_NODES_H
#define IE_NODES_H

//___|"ie_nodes.h"|_____________________________________________________________
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

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ie_assets.h"
#include "ie_messages.h"

namespace ie
{

  //FORWARD DECLARATIONS
  class SortTreeNode;
  class PhysicsSort;
  class EntityNode;
  class PlayerNode;
  class CameraNode;

  enum NodeType{NONE, PLAYER, CAMERA, STATIC, TERRAIN, WATER};
  
  class PhysicsPointers
  {
    public:
    NodeType type;
    EntityNode* entity;
    PlayerNode* player;
  };

  class RenderPointers
  {
    public:
    NodeType type;
    EntityNode* entity;
    RenderUnit* ru;
    TerrainAsset* ta;
    PlayerNode* player;
    CameraNode* camera;
  };

  //SCENEGRAPH NODES
  class GraphNode
  {
    public:
    GraphNode();
    virtual ~GraphNode();

    virtual void physics(void);
    virtual void update(void);
    virtual void render(void);

    void sendToPhysics(PhysicsPointers);

    void addChild(GraphNode*);
    void setSortTreeRoot(SortTreeNode*);
    void setPhysicsTreeRoot(PhysicsSort*);
    void setAspectRatio(float);
    void receiveMessage(ie::AssetStatusMessage);

    NodeType type;
    std::vector<GraphNode*> children;
    glm::mat4 mtwMatrix;
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;
    GraphNode* parentNode;

    protected:
    static SortTreeNode* sortTreeRoot;
    static PhysicsSort* physicsTreeRoot;
    static float aspectRatio;

    //DATA FROM THE ASSET MANAGER
    static std::map<unsigned int, ModelAsset>* models;
    static std::map<std::string, unsigned int>* modelNameIdMap;
    static std::map<unsigned int, TerrainAsset>* terrains;
    static std::map<std::string, unsigned int>* terrainNameIdMap;
    static std::map<unsigned int, LightAsset>* lights;
    static std::map<std::string, unsigned int>* lightNameIdMap;
    static std::map<unsigned int, MaterialAsset>* materials;
    static std::map<std::string, unsigned int>* materialNameIdMap;
    static std::map<unsigned int, RenderUnit>* rus;
  };


  class EntityNode : public GraphNode
  {
    public:
    EntityNode();
    EntityNode(std::string, std::string, NodeType);

    virtual void physics(void);
    virtual void render(void);

    bool usesGlobalAmbient;
    bool usesLightAmbient;
    bool usesLightDiffuse;
    bool usesLightSpecular;
    bool usesLightFalloff;

    bool usesPhysics;
    bool collidable;

    std::string name;
    unsigned int assetId;
    bool hidden;
  };

  class CameraNode : public GraphNode
  {
    public:
    CameraNode();
    void update(void);
    void render(void);

    glm::vec3 firstPersonOffset;
    glm::vec3 thirdPersonOffset;
    glm::vec3 upVector;
    glm::vec3 lookVector;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    float distance;
    float lookSpeed;
    float fieldOfView;
    float currentAspectRatio;
    float nearPlane;
    float farPlane;

  };

  class PlayerNode : public GraphNode
  {
    public:
    PlayerNode();

    void update(void);
    void physics(void);
    void render(void);

    glm::vec3 upVector;
    float moveSpeed;
    float turnSpeed;
    EntityNode* linkedEntity;
    CameraNode* linkedCamera;
  };


  //SORTING BUCKET TREE NODES


  class RenderInstructions
  {
    public:
    std::string renderer;
    std::string shader;
    std::string data;
  };

  class SortTreeNode
  {
    public:
    void addChild(SortTreeNode*);
    virtual void sort(RenderPointers);

    void receiveMessage(ie::AssetStatusMessage);

    std::vector<SortTreeNode*> children;

    protected:

    //DATA FROM THE ASSET MANAGER
    static std::map<unsigned int, ModelAsset>* models;
    static std::map<std::string, unsigned int>* modelNameIdMap;
    static std::map<unsigned int, TerrainAsset>* terrains;
    static std::map<std::string, unsigned int>* terrainNameIdMap;
    static std::map<unsigned int, LightAsset>* lights;
    static std::map<std::string, unsigned int>* lightNameIdMap;
    static std::map<unsigned int, MaterialAsset>* materials;
    static std::map<std::string, unsigned int>* materialNameIdMap;
    static std::map<unsigned int, RenderUnit>* rus;
  };

  class SortTypeNode : public SortTreeNode
  {
    public:
    virtual void sort(RenderPointers);
    SortTreeNode* toPlayer;
    SortTreeNode* toCamera;
    SortTreeNode* toTerrain;
    SortTreeNode* toStatic;
  };

  class SortStaticTypeNode : public SortTreeNode
  {
    public:
    virtual void sort(RenderPointers);
    
    SortTreeNode* toMaterialed;
    SortTreeNode* toTextured;
  };


  //SORT TREE BUCKETS
  class SortBucket : public SortTreeNode
  {
    public:
    SortBucket();

    virtual void sort(RenderPointers);
    void clear();

    SortBucket* getNextBucket(void);
    void setRenderInstructions(RenderInstructions);
    RenderInstructions* getRenderInstructions(void);
    std::vector<RenderPointers>* getRenderUnits(void);

    RenderInstructions instructions;
    std::vector<RenderPointers> renderUnits;
    SortBucket* nextBucket;
  };

  //PHYSICES BUCKETS
  class PhysicsBucket
  {
    public:
    PhysicsBucket();
    void sort(PhysicsPointers);
    PhysicsBucket* getNextBucket(void);
    void clear(void);
    NodeType type;
    std::vector<PhysicsPointers> physicsUnits;
    PhysicsBucket* nextBucket;
  };

  class PhysicsSort
  {
    public:
    void sort(PhysicsPointers);
    PhysicsBucket* toTerrain;
    PhysicsBucket* toPlayer;
    PhysicsBucket* toStatic;

  };

}

#endif
