//___|"ie_scenegraph.cpp"|______________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_scenegraph.h"

#include <iostream>
#include <map>
#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ie_assets.h"
#include "ie_messages.h"
#include "ie_nodes.h"
#include "ie_vram.h"



//___|SCENEGRAPH IMPLEMENTATION|________________________________________________

ie::SceneGraph::SceneGraph()
{
  //Generating Nodes
  root = new GraphNode();
  sortTree = new SortTreeNode();
  SortEntityTypeNode* entityType = new SortEntityTypeNode(); 
  SortStaticTypeNode* staticType = new SortStaticTypeNode();
  SortBucket* terrainBucket = new SortBucket();
  SortBucket* materialedBucket = new SortBucket();
  SortBucket* texturedBucket = new SortBucket();;
  firstBucket = terrainBucket;
  
  //Setting Node Status
  ie::RenderInstructions terrainInstructions;
  terrainInstructions.renderer = "terrain";
  terrainInstructions.shader = "terrain";
  terrainBucket->setRenderInstructions(terrainInstructions);
  ie::RenderInstructions materialedInstructions;
  materialedInstructions.renderer = "materialed";
  materialedInstructions.shader = "static";
  materialedBucket->setRenderInstructions(materialedInstructions);
  ie::RenderInstructions texturedInstructions;
  texturedInstructions.renderer = "textured";
  texturedInstructions.shader = "static";
  texturedBucket->setRenderInstructions(texturedInstructions);
  
  //Linking Nodes
  root->setSortTreeRoot(sortTree);

  sortTree->addChild(entityType);
  entityType->addToStaticChild(staticType);
  entityType->addToTerrainChild(terrainBucket);
  staticType->addToStaticMaterialedChild(materialedBucket);
  staticType->addToStaticTexturedChild(texturedBucket);
  terrainBucket->setNextBucket(materialedBucket);
  materialedBucket->setNextBucket(texturedBucket);

}

void ie::SceneGraph::receiveMessage(ie::AssetStatusMessage msg)
{
  root->receiveMessage(msg);
  sortTree->receiveMessage(msg);
}


ie::GraphStatusToRenderMessage ie::SceneGraph::sendGraphStatusToRenderMessage(void)
{
  ie::GraphStatusToRenderMessage msg;
  msg.bucket = firstBucket;
  return msg;
}

void ie::SceneGraph::update(void)
{
  root->update();
  root->render();
}


