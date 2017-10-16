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
#include "ie_messages.h"
#include "ie_nodes.h"

namespace ie
{

  class SceneGraph
  {
    public:
    SceneGraph();
    GraphNode* root;
    SortTreeNode* sortTree;
    SortBucket* firstBucket;
    void update(void);
    void receiveMessage(ie::AssetStatusToScenegraphMessage);
    ie::GraphStatusToRenderMessage sendGraphStatusToRenderMessage(void);
    private:
  };

}

#endif
