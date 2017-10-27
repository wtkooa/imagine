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
  renderTree = new RenderTreeNode();
  SortTypeNode* entityType = new SortTypeNode(); 
  SortStaticTypeNode* staticType = new SortStaticTypeNode();
  RenderBucket* terrainBucket = new RenderBucket();
  RenderBucket* materialedBucket = new RenderBucket();
  RenderBucket* texturedBucket = new RenderBucket();
  RenderBucket* playerBucket = new RenderBucket();
  RenderBucket* cameraBucket = new RenderBucket();
  PhysicsTreeNode* physicsTree = new PhysicsTreeNode();
  PhysicsBucket* terrainPhyBucket = new PhysicsBucket();
  PhysicsBucket* playerPhyBucket = new PhysicsBucket();
  PhysicsBucket* staticPhyBucket = new PhysicsBucket();
  
  //Setting Node Status
  ie::RenderState terrainState;
  terrainState.shader = TERRAIN_SHADER;
  terrainBucket->setRenderState(terrainState);
  terrainBucket->type = TERRAIN_RENDER;
  ie::RenderState materialedState;
  materialedState.shader = STATIC_SHADER;
  materialedBucket->setRenderState(materialedState);
  materialedBucket->type = MATERIAL_RENDER;
  ie::RenderState texturedState;
  texturedState.shader = STATIC_SHADER;
  texturedBucket->setRenderState(texturedState);
  texturedBucket->type = TEXTURE_RENDER;
  playerBucket->type = PLAYER_RENDER;
  cameraBucket->type = CAMERA_RENDER;

  terrainPhyBucket->type = TERRAIN_NODE;
  playerPhyBucket->type = PLAYER_NODE;
  staticPhyBucket->type = STATIC_NODE;
  
  //Linking Nodes
  root->setSortTreeRoot(renderTree);
  root->setPhysicsTreeNodeRoot(physicsTree);

  renderTree->child = entityType;
  entityType->toStatic = staticType;
  entityType->toTerrain = terrainBucket;
  entityType->toPlayer = playerBucket;
  entityType->toCamera = cameraBucket;
  staticType->toMaterialed = materialedBucket;
  staticType->toTextured = texturedBucket;
  playerBucket->nextBucket = cameraBucket;
  cameraBucket->nextBucket = terrainBucket;
  terrainBucket->nextBucket = materialedBucket;
  materialedBucket->nextBucket = texturedBucket;
  firstBucket = playerBucket;

  physicsTree->toTerrain = terrainPhyBucket;
  physicsTree->toPlayer = playerPhyBucket;
  physicsTree->toStatic = staticPhyBucket;
  terrainPhyBucket->nextBucket = staticPhyBucket;
  staticPhyBucket->nextBucket = playerPhyBucket;
  firstPhyBucket = terrainPhyBucket;
}


void ie::SceneGraph::receiveMessage(ie::AssetStatusMessage msg)
{
  root->receiveMessage(msg);
  renderTree->receiveMessage(msg);
}

ie::GraphStatusMessage ie::SceneGraph::sendGraphStatusMessage(void)
{
  ie::GraphStatusMessage msg;
  msg.bucket = firstBucket;
  msg.root = root;
  msg.phyBucket = firstPhyBucket; 
  return msg;
}

void ie::SceneGraph::physics(void) {root->physics();}
void ie::SceneGraph::update(void) {root->update();}
void ie::SceneGraph::render(void) {root->render();}
void ie::SceneGraph::updateAspectRatio(float ratio)
{
  root->setAspectRatio(ratio);
}
