#ifndef IE_SCENEGRAPH_H
#define IE_SCENEGRAPH_H

//___|"ie_scenegraph.h"|________________________________________________________
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

namespace ie
{
  
  //SCENEGRAPH NODES
  class GraphNode
  {
    public:
    GraphNode();
    virtual ~GraphNode();

    void addChild(GraphNode*);
    void update(void);
    void render(void);
    void setParentNode(GraphNode*);

    protected:
    glm::mat4 transformation;

    private:
    GraphNode* parentNode;
    std::vector<GraphNode*> children;
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;
  };

  class EntityNode : public GraphNode
  {
    public:
    EntityNode();

    private:
    std::string name;
    EntityType type;
    unsigned int assetId;
    bool hidden;
    bool usesGlobalAmbient;
    bool usesLightAmbient;
    bool usesLightDiffuse;
    bool usesLightSpecular;
    bool usesLightFalloff;
  };


 //SORT TREE NODES
 class SortTreeNode
 {
   public:

   private:
 };


 //SORT TREE BUCKETS

  class SceneGraph
  {
    public:
    GraphNode root;
    SortTreeNode renderTree;
    private:
  };

}

#endif
