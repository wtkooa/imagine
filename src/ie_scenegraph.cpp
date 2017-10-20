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
  SortTypeNode* entityType = new SortTypeNode(); 
  SortStaticTypeNode* staticType = new SortStaticTypeNode();
  SortBucket* terrainBucket = new SortBucket();
  SortBucket* materialedBucket = new SortBucket();
  SortBucket* texturedBucket = new SortBucket();
  SortBucket* playerBucket = new SortBucket();
  SortBucket* cameraBucket = new SortBucket();
  PhysicsSort* physicsSort = new PhysicsSort();
  PhysicsBucket* terrainPhyBucket = new PhysicsBucket();
  PhysicsBucket* playerPhyBucket = new PhysicsBucket();
  PhysicsBucket* staticPhyBucket = new PhysicsBucket();
  
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
  ie::RenderInstructions playerInstructions;
  playerInstructions.data = "player";
  playerBucket->setRenderInstructions(playerInstructions);
  ie::RenderInstructions cameraInstructions;
  cameraInstructions.data = "camera";
  cameraBucket->setRenderInstructions(cameraInstructions);

  terrainPhyBucket->type = TERRAIN;
  playerPhyBucket->type = PLAYER;
  staticPhyBucket->type = STATIC;
  
  //Linking Nodes
  root->setSortTreeRoot(sortTree);
  root->setPhysicsTreeRoot(physicsSort);

  sortTree->addChild(entityType);
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

  physicsSort->toTerrain = terrainPhyBucket;
  physicsSort->toPlayer = playerPhyBucket;
  physicsSort->toStatic = staticPhyBucket;
  terrainPhyBucket->nextBucket = staticPhyBucket;
  staticPhyBucket->nextBucket = playerPhyBucket;
  firstPhyBucket = terrainPhyBucket;
}


void ie::SceneGraph::receiveMessage(ie::AssetStatusMessage msg)
{
  root->receiveMessage(msg);
  sortTree->receiveMessage(msg);
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
