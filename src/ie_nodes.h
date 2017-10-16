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


  //SCENEGRAPH NODES
  class GraphNode
  {
    public:
    GraphNode();
    virtual ~GraphNode();

    void addChild(GraphNode*);
    void update(void);
    virtual void render(void);
    void setParentNode(GraphNode*);
    void setSortTreeRoot(SortTreeNode*);
    glm::mat4 getTransformationMatrix(void);
    void setTranslation(glm::vec3);
    void receiveMessage(ie::AssetStatusToScenegraphMessage);


    protected:
    glm::mat4 transformationMatrix;
    std::vector<GraphNode*> children;
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;

    //DATA FROM THE ASSET MANAGER
    static SortTreeNode* sortTreeRoot;
    static std::map<unsigned int, ModelAsset>* models;
    static std::map<std::string, unsigned int>* modelNameIdMap;
    static std::map<unsigned int, TerrainAsset>* terrains;
    static std::map<std::string, unsigned int>* terrainNameIdMap;
    static std::map<unsigned int, LightAsset>* lights;
    static std::map<std::string, unsigned int>* lightNameIdMap;
    static std::map<unsigned int, MaterialAsset>* materials;
    static std::map<std::string, unsigned int>* materialNameIdMap;
    static std::map<unsigned int, RenderUnit>* rus;

    private:
    GraphNode* parentNode;

  };


  class EntityNode : public GraphNode
  {
    public:
    EntityNode();
    EntityNode(std::string, std::string, EntityType);
    virtual void render(void);
    EntityType getType(void);
    unsigned int getAssetId(void);
    bool usesGlobalAmbient;
    bool usesLightAmbient;
    bool usesLightDiffuse;
    bool usesLightSpecular;
    bool usesLightFalloff;

    protected:
    std::string name;
    EntityType type;
    unsigned int assetId;
    bool hidden;

    private:
  };

  //SORTING BUCKET TREE NODES

  class RenderPointers
  {
    public:
    EntityNode* entity;
    RenderUnit* ru;
    TerrainAsset* ta;
  };


  class RenderInstructions
  {
    public:
    std::string renderer;
    std::string shader;
  };

  class SortTreeNode
  {
    public:
    void addChild(SortTreeNode*);
    virtual void sort(EntityNode*);
    virtual void sort(RenderPointers);

    void receiveMessage(ie::AssetStatusToScenegraphMessage);

    protected:
    std::vector<SortTreeNode*> children;

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

    private:
  };

  class SortEntityTypeNode : public SortTreeNode
  {
    public:
    void sort(EntityNode*);
    void addToTerrainChild(SortTreeNode*);
    void addToStaticChild(SortTreeNode*);

    private:
    SortTreeNode* toTerrain;
    SortTreeNode* toStatic;
  };

  class SortStaticTypeNode : public SortTreeNode
  {
    public:
    void sort(EntityNode*);
    void addToStaticMaterialedChild(SortTreeNode*);
    void addToStaticTexturedChild(SortTreeNode*);
    
    private:
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
    void setNextBucket(SortBucket*);
    SortBucket* getNextBucket(void);
    void setRenderInstructions(RenderInstructions);
    RenderInstructions* getRenderInstructions(void);
    std::vector<RenderPointers>* getRenderUnits(void);

    protected:
    RenderInstructions instructions;
    std::vector<RenderPointers> renderUnits;
    SortBucket* link;
  };
}

#endif
