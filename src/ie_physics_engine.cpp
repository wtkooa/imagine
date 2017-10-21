//___|"ie_physics_engine.cpp"|__________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_physics_engine.h"

#include <cmath>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "ie_assets.h"
#include "ie_config.h"
#include "ie_const.h"
#include "ie_controller.h"
#include "ie_messages.h"
#include "ie_nodes.h"

void ie::PhysicsEngine::receiveMessage(ie::TimeStatusMessage msg)
{
  frameDelta = msg.frameDelta;  
}

void ie::PhysicsEngine::receiveMessage(ie::GraphStatusMessage msg)
{
  firstBucket = msg.phyBucket;
}

void ie::PhysicsEngine::receiveMessage(ie::AssetStatusMessage msg)
{
  models = msg.models;
  modelNameIdMap = msg.modelNameIdMap;
  terrains = msg.terrains;
  terrainNameIdMap = msg.terrainNameIdMap;
  lights = msg.lights;
  lightNameIdMap = msg.lightNameIdMap;
  materials = msg.materials;
  materialNameIdMap = msg.materialNameIdMap;
  rus = msg.rus;
  iHeap = msg.iHeap;
  vHeap = msg.vHeap;
}

void ie::PhysicsEngine::setController(ie::Controller* ctrl) {control = ctrl;}

void ie::PhysicsEngine::update(void)
{
  
  PhysicsBucket* currentBucket = firstBucket;
  while (currentBucket != NULL)
  {
    if (currentBucket->type == TERRAIN)
    {
      updateTerrains(currentBucket);
    }
    else if (currentBucket->type == PLAYER)
    {
      updatePlayers(currentBucket);
    }
    else if (currentBucket->type == STATIC)
    {
      //std::cout << "static"  << std::endl;
    }
    currentBucket = currentBucket->getNextBucket();
  }
  firstBucket->clear();
  collidableTerrain.clear();
}

void ie::PhysicsEngine::updateTerrains(PhysicsBucket* terrains)
{
  std::vector<PhysicsPointers>* terrainList = &terrains->physicsUnits;

  for (auto it = terrainList->begin(); it != terrainList->end(); it++)
  {
    PhysicsPointers terrainPointers = (*it);
    EntityNode* terrain = terrainPointers.entity;
    updateTerrain(terrain);
  }
}

void ie::PhysicsEngine::updateTerrain(EntityNode* terrain)
{
  if (terrain->collidable == true)
  {
    collidableTerrain.push_back(terrain);
  }
}

void ie::PhysicsEngine::updatePlayers(PhysicsBucket* players)
{
  std::vector<PhysicsPointers>* playerList = &players->physicsUnits;

  for (auto it = playerList->begin(); it != playerList->end(); it++)
  {
    PhysicsPointers playerPointers = (*it);  
    PlayerNode* player = playerPointers.player;

    if (control->mode == FIRST_PERSON)
    {
      updatePlayerFirstPerson(player);
    }
  }
}

void ie::PhysicsEngine::updatePlayerFirstPerson(PlayerNode* player)
{
  CameraNode* camera = player->linkedCamera;
  EntityNode* entity = player->linkedEntity;

  if (entity->hidden == false) {entity->hidden = true;}
  
  float refinedMoveSpeed = 0;
  if (control->getGrabMode())
  {
    short directionality = std::abs(control->translEventVec.x) +
                           std::abs(control->translEventVec.z);
    if (directionality == 1)
    {
      refinedMoveSpeed = player->moveSpeed;
    }
    else if (directionality == 2)
    {
      refinedMoveSpeed = player->moveSpeed * (1 / std::sqrt(2));
    }

    float delta = frameDelta * float(refinedMoveSpeed / ie::MSECS_PER_SEC); 
    
    player->translation += control->translEventVec.z * delta * player->rotation;

    player->translation += control->translEventVec.x * delta * glm::normalize(
                                                   glm::cross(player->rotation,
                                                   player->upVector));

    glm::mat3 yRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(control->rotateEventVec.x * player->turnSpeed),
                                  player->upVector));
    player->rotation = yRotate * player->rotation; 

    glm::mat4 entityMatrix = glm::translate(glm::mat4(), player->translation);
    entity->mtwMatrix = entityMatrix;

    camera->translation = player->translation + camera->offset;

    yRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(control->rotateEventVec.x * camera->lookSpeed),
                                  camera->upVector));
    glm::vec3 newXAxisVector = glm::normalize(glm::cross(player->rotation, player->upVector));
    glm::mat3 xRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(control->rotateEventVec.y * camera->lookSpeed),
                                  newXAxisVector)); 
    glm::mat3 rotation = xRotate * yRotate;
    camera->lookVector = rotation * camera->lookVector;

    float xOffsetAngle = glm::angle(ie::UP_VECTOR, camera->lookVector);
    if (xOffsetAngle > glm::radians(170.0f))
    {
      float correctionAngle = xOffsetAngle - glm::radians(170.0f);
      glm::mat3 correctionRotation = glm::mat3(glm::rotate(glm::mat4(),
                                               correctionAngle,
                                               newXAxisVector));
      camera->lookVector = correctionRotation * camera->lookVector;
    }
    else if (xOffsetAngle < glm::radians(10.0f))
    {
      float correctionAngle = glm::radians(10.0f) - xOffsetAngle;
      glm::mat3 correctionRotation = glm::mat3(glm::rotate(glm::mat4(),
                                               -correctionAngle,
                                               newXAxisVector));
      camera->lookVector = correctionRotation * camera->lookVector;
    }

    camera->viewMatrix = glm::lookAt(camera->translation,
                         (camera->lookVector + camera->translation),
                         camera->upVector);

    control->clearRotateEventVec();

    updatePlayerTerrainInteraction(player);
  }
}

void ie::PhysicsEngine::updatePlayerTerrainInteraction(PlayerNode* player)
{
  glm::vec3 playerTransl = player->translation;
  for (auto it = collidableTerrain.begin(); it != collidableTerrain.end(); it++)
  {
    EntityNode* terrain = (*it);
    TerrainAsset* terrainAsset = &(*terrains)[terrain->assetId];
    short dim = terrainAsset->dim;
    float unitSize = terrainAsset->unitSize;
    float terrainSize = unitSize * (dim - 1);
    glm::vec3 startTransl = terrain->translation;
    glm::vec3 endTransl((startTransl.x + terrainSize) , startTransl.y,
                          (startTransl.z + terrainSize));
    bool withinX = (playerTransl.x > startTransl.x) && (playerTransl.x < endTransl.x);
    bool withinZ = (playerTransl.z > startTransl.z) && (playerTransl.z < endTransl.z);
    if (withinX && withinZ)
    {
      glm::vec2 terrainRelPos(playerTransl.x - startTransl.x,
                              playerTransl.z - startTransl.z);
      glm::vec2 terrainRelFloorPos(floor(terrainRelPos.x / unitSize),
                                   floor(terrainRelPos.y /unitSize));
      glm::vec2 terrainRelModPos(fmod(terrainRelPos.x, unitSize),
                                 fmod(terrainRelPos.y, unitSize));
      short tri;
      if (terrainRelModPos.x <= terrainRelModPos.y)
      {
        tri = 0;
      }
      else
      {
        tri = 1;
      }
      unsigned int n = (terrainRelFloorPos.y * (dim - 1)) + terrainRelFloorPos.x;
      n = (n * 2) + tri; 
      unsigned int indexHeapOffset = terrainAsset->indexHeapOffset + n; 
      unsigned int vertexHeapOffset = terrainAsset->vertexHeapOffset;
      glm::ivec4 index = (*iHeap)[indexHeapOffset]; 
      glm::vec4 v1 = (*vHeap)[vertexHeapOffset + index.x];
      glm::vec4 v2 = (*vHeap)[vertexHeapOffset + index.y];
      glm::vec4 v3 = (*vHeap)[vertexHeapOffset + index.z];
      float height = calcBarycentric(v1, v2, v3, terrainRelPos);
      player->translation.y = height;
    }
  }
  
}


float ie::PhysicsEngine::calcBarycentric(glm::vec4 v1, glm::vec4 v2,
                                         glm::vec4 v3, glm::vec2 p1)
{
  float denom = (v2.z - v3.z) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.z - v3.z);
  float lamda1 = ((v2.z - v3.z) * (p1.x - v3.x) + (v3.x - v2.x) * (p1.y - v3.z)) / denom;
  float lamda2 = ((v3.z - v1.z) * (p1.x - v3.x) + (v1.x - v3.x) * (p1.y - v3.z)) / denom;
  float lamda3 = 1.0f - lamda1 - lamda2;
  return lamda1 * v1.y + lamda2 * v2.y + lamda3 * v3.y;
}

