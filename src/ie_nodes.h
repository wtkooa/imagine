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

#include "ie_enum.h"
#include "ie_messages.h"

namespace ie
{

  //FORWARD DECLARATIONS
  class RenderTreeNode;
  class PhysicsTreeNode;
  class EntityNode;
  class TerrainNode;
  class StaticNode;
  class PlayerNode;
  class CameraNode;


  union NodePointer
  {
    StaticNode* stat;
    TerrainNode* terrain;
    PlayerNode* player;
    CameraNode* camera;
  };


  union AssetPointer
  {
    TerrainAsset* ta;
    RenderUnit* ru;
  };


  class NodePacket
  {
    public:
    IEenum type;
    NodePointer node;
    AssetPointer asset;
  };


  //SCENEGRAPH BASE NODE
  class GraphNode
  {
    public:
    GraphNode();
    virtual ~GraphNode();

    virtual void physics(void);
    virtual void update(void);
    virtual void render(void);

    void addChild(GraphNode*);

    void setSortTreeRoot(RenderTreeNode*);
    void setPhysicsTreeNodeRoot(PhysicsTreeNode*);
    void setAspectRatio(float);

    void receiveMessage(ie::AssetStatusMessage);

    IEenum type;
    std::vector<GraphNode*> children;
    GraphNode* parentNode;
    glm::mat4 mtwMatrix;

    protected:
    static RenderTreeNode* renderTreeRoot;
    static PhysicsTreeNode* physicsTreeRoot;
    static float aspectRatio;

    //Data from the asset manager
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


  //ENTITY BASE NODE
  class EntityNode : public GraphNode
  {
    public:
    EntityNode();

    virtual void update(void);

    std::string name;
    
    bool hidden;

    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;
  };


  class TerrainNode : public EntityNode
  {
    public:
    TerrainNode(std::string, std::string);

    virtual void physics(void);
    virtual void render(void);

    unsigned int assetId;

    bool usesGlobalAmbient;
    bool usesLightAmbient;
    bool usesLightDiffuse;
    bool usesLightSpecular;
    bool usesLightFalloff;

    bool collidable;
  };


  class StaticNode : public EntityNode
  {
    public:
    StaticNode(std::string, std::string);

    virtual void render(void);
    
    unsigned int assetId;

    bool usesGlobalAmbient;
    bool usesLightAmbient;
    bool usesLightDiffuse;
    bool usesLightSpecular;
    bool usesLightFalloff;
     
    bool usesCullFace;
    bool usesPhysics;
  };


  class CameraNode : public GraphNode
  {
    public:
    CameraNode();

    void update(void);
    void render(void);

    glm::vec3 translation;
    glm::vec3 firstPersonOffset;
    glm::vec3 thirdPersonOffset;
    float distance;
    glm::vec3 upVector;
    glm::vec3 lookVector;
    glm::mat4 viewMatrix;
    float lookSpeed;

    glm::mat4 projectionMatrix;
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

    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 upVector;
    float moveSpeed;
    float turnSpeed;

    StaticNode* linkedEntity;
    CameraNode* linkedCamera;
  };


  //RENDER TREE NODES
  class RenderTreeNode
  {
    public:
    virtual void sort(NodePacket);

    void receiveMessage(ie::AssetStatusMessage);

    RenderTreeNode* child;

    protected:
    //Data from the asset manager
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


  class SortTypeNode : public RenderTreeNode
  {
    public:
    virtual void sort(NodePacket);
    RenderTreeNode* toPlayer;
    RenderTreeNode* toCamera;
    RenderTreeNode* toTerrain;
    RenderTreeNode* toStatic;
  };


  class SortStaticTypeNode : public RenderTreeNode
  {
    public:
    virtual void sort(NodePacket);
    RenderTreeNode* toMaterialed;
    RenderTreeNode* toTextured;
  };


  class SortCullFaceNode : public RenderTreeNode
  {
    public:
    virtual void sort(NodePacket);
    RenderTreeNode* toCulled;
    RenderTreeNode* toUnCulled;
  };


  //RENDER TREE BUCKETS
  class RenderState
  {
    public:
    RenderState();
    IEenum shader;
    bool cullFace;
  };


  class RenderBucket : public RenderTreeNode
  {
    public:
    RenderBucket();

    virtual void sort(NodePacket);
    void clear();

    IEenum type;

    RenderBucket* getNextBucket(void);
    void setRenderState(RenderState);
    RenderState* getRenderState(void);
    std::vector<NodePacket>* getPackets(void);

    IEenum state;
    std::vector<NodePacket> packets;
    RenderBucket* nextBucket;
  };


  //PHYSICS TREE BUCKETS
  class PhysicsBucket
  {
    public:
    PhysicsBucket();

    void sort(NodePacket);
    PhysicsBucket* getNextBucket(void);
    void clear(void);

    IEenum type;
    std::vector<NodePacket> packets;
    PhysicsBucket* nextBucket;
  };


  //PHYSICS TREE NODES
  class PhysicsTreeNode
  {
    public:
    void sort(NodePacket);
    PhysicsBucket* toTerrain;
    PhysicsBucket* toPlayer;
    PhysicsBucket* toStatic;
  };

}

#endif
