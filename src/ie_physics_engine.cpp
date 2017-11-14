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

#include "ie_config.h"
#include "ie_const.h"
#include "ie_controller.h"
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

void ie::PhysicsEngine::setController(ie::Controller* control) {ctrl = control;}

void ie::PhysicsEngine::update(void)
{
  
  PhysicsBucket* currentBucket = firstBucket;
  while (currentBucket != NULL)
  {
    if (currentBucket->type == IE_TERRAIN_NODE)
    {
      updateTerrains(currentBucket);
    }
    else if (currentBucket->type == IE_PLAYER_NODE)
    {
      updatePlayers(currentBucket);
    }
    else if (currentBucket->type == IE_STATIC_NODE)
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
  std::vector<NodePacket>* terrainList = &terrains->packets;

  for (auto it = terrainList->begin(); it != terrainList->end(); it++)
  {
    NodePacket packet = (*it);
    TerrainNode* terrain = packet.node.terrain;
    updateTerrain(terrain);
  }
}

void ie::PhysicsEngine::updateTerrain(TerrainNode* terrain)
{
  if (terrain->collidable == true)
  {
    collidableTerrain.push_back(terrain);
  }
}

void ie::PhysicsEngine::updatePlayers(PhysicsBucket* players)
{

  std::vector<NodePacket>* playerList = &players->packets;

  for (auto it = playerList->begin(); it != playerList->end(); it++)
  {
    NodePacket packet = (*it);  
    PlayerNode* player = packet.node.player;

    if (ctrl->mode == IE_FIRST_PERSON_MODE)
    {
      updatePlayerFirstPerson(player);
    }
    else if (ctrl->mode == IE_THIRD_PERSON_MODE)
    {
      updatePlayerThirdPerson(player);
    }
  }
}

void ie::PhysicsEngine::updatePlayerFirstPerson(PlayerNode* player)
{
  if (ctrl->getGrabMode())
  {
    CameraNode* camera = player->linkedCamera;
    StaticNode* entity = player->linkedEntity;

    if (entity->hidden == false) {entity->hidden = true;}
  
    float refinedMoveSpeed = 0;
    short directionality = std::abs(ctrl->translEventVec.x) +
                           std::abs(ctrl->translEventVec.z);
    if (directionality == 1)
    {
      refinedMoveSpeed = player->moveSpeed;
    }
    else if (directionality == 2)
    {
      refinedMoveSpeed = player->moveSpeed * (1 / std::sqrt(2));
    }

    float delta = frameDelta * float(refinedMoveSpeed / ie::MSECS_PER_SEC); 
    
    player->translation += ctrl->translEventVec.z * delta * player->rotation;

    player->translation += ctrl->translEventVec.x * delta * glm::normalize(
                                                   glm::cross(player->rotation,
                                                   player->upVector));

    glm::mat3 yRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(ctrl->rotateEventVec.x * player->turnSpeed),
                                  player->upVector));
    player->rotation = yRotate * player->rotation; 
    entity->rotation = player->rotation;

    camera->translation = player->translation + camera->firstPersonOffset;

    yRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(ctrl->rotateEventVec.x * camera->lookSpeed),
                                  camera->upVector));
    glm::vec3 newXAxisVector = glm::normalize(glm::cross(player->rotation, player->upVector));
    glm::mat3 xRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(ctrl->rotateEventVec.y * camera->lookSpeed),
                                  newXAxisVector)); 
    glm::mat3 rotation = xRotate * yRotate;
    camera->lookVector = rotation * camera->lookVector;
    camera->thirdPersonOffset = rotation * camera->thirdPersonOffset;

    float xOffsetAngle = glm::angle(ie::UP_VECTOR, camera->lookVector);
    if (xOffsetAngle > glm::radians(170.0f))
    {
      float correctionAngle = xOffsetAngle - glm::radians(170.0f);
      glm::mat3 correctionRotation = glm::mat3(glm::rotate(glm::mat4(),
                                               correctionAngle,
                                               newXAxisVector));
      camera->lookVector = correctionRotation * camera->lookVector;
      camera->thirdPersonOffset = correctionRotation * camera->thirdPersonOffset;
    
    }
    else if (xOffsetAngle < glm::radians(10.0f))
    {
      float correctionAngle = glm::radians(10.0f) - xOffsetAngle;
      glm::mat3 correctionRotation = glm::mat3(glm::rotate(glm::mat4(),
                                               -correctionAngle,
                                               newXAxisVector));
      camera->lookVector = correctionRotation * camera->lookVector;
      camera->thirdPersonOffset = correctionRotation * camera->thirdPersonOffset;
    }

    camera->viewMatrix = glm::lookAt(camera->translation,
                         (camera->lookVector + camera->translation),
                         camera->upVector);

    ctrl->clearRotateEventVec();
    ctrl->scrollEvent = 0;

    float height = calcTerrainInteraction(player->translation);
    player->translation.y = height;
    entity->translation = player->translation;
  }
}


void ie::PhysicsEngine::updatePlayerThirdPerson(PlayerNode* player)
{
  if (ctrl->getGrabMode())
  {
    CameraNode* camera = player->linkedCamera;
    StaticNode* entity = player->linkedEntity;

    if (entity->hidden == true) {entity->hidden = false;}
    
    float refinedMoveSpeed = 0;
    short directionality = std::abs(ctrl->translEventVec.x) +
                           std::abs(ctrl->translEventVec.z);
    if (directionality == 1)
    {
      refinedMoveSpeed = player->moveSpeed;
    }
    else if (directionality == 2)
    {
      refinedMoveSpeed = player->moveSpeed * (1 / std::sqrt(2));
    }

    float delta = frameDelta * float(refinedMoveSpeed / ie::MSECS_PER_SEC); 
    
    player->translation += ctrl->translEventVec.z * delta * player->rotation;

    player->translation += ctrl->translEventVec.x * delta * glm::normalize(
                                                   glm::cross(player->rotation,
                                                   player->upVector));

    glm::mat3 yRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(ctrl->rotateEventVec.x * player->turnSpeed),
                                  player->upVector));
    player->rotation = yRotate * player->rotation; 
    entity->rotation = player->rotation;

    glm::mat4 entityMatrix = glm::translate(glm::mat4(), player->translation);
    entity->mtwMatrix = entityMatrix;
    
    camera->distance += (ctrl->scrollEvent * 0.3);
    if (camera->distance < 1.0f)
    {
      camera->distance = 1.0f;
    }
    else if (camera->distance > 50)
    {
      camera->distance = 50.0f;
    }
    camera->translation = player->translation + (camera->thirdPersonOffset *
                                                 camera->distance) + 
                                                 camera->firstPersonOffset;
    ctrl->scrollEvent = 0;

    yRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(ctrl->rotateEventVec.x * camera->lookSpeed),
                                  camera->upVector));
    glm::vec3 newXAxisVector = glm::normalize(glm::cross(player->rotation, player->upVector));
    glm::mat3 xRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(ctrl->rotateEventVec.y * camera->lookSpeed),
                                  newXAxisVector)); 
    glm::mat3 rotation = xRotate * yRotate;
    camera->thirdPersonOffset = rotation * camera->thirdPersonOffset;
    camera->lookVector = glm::normalize((player->translation + camera->firstPersonOffset) -
                                         camera->translation);

    float xOffsetAngle = glm::angle(ie::UP_VECTOR, camera->lookVector);
    if (xOffsetAngle > glm::radians(170.0f))
    {
      float correctionAngle = xOffsetAngle - glm::radians(170.0f);
      glm::mat3 correctionRotation = glm::mat3(glm::rotate(glm::mat4(),
                                               correctionAngle,
                                               newXAxisVector));
      camera->lookVector = correctionRotation * camera->lookVector;
      camera->thirdPersonOffset = correctionRotation * camera->thirdPersonOffset;
    }
    else if (xOffsetAngle < glm::radians(10.0f))
    {
      float correctionAngle = glm::radians(10.0f) - xOffsetAngle;
      glm::mat3 correctionRotation = glm::mat3(glm::rotate(glm::mat4(),
                                               -correctionAngle,
                                               newXAxisVector));
      camera->lookVector = correctionRotation * camera->lookVector;
      camera->thirdPersonOffset = correctionRotation * camera->thirdPersonOffset;
    }

    camera->viewMatrix = glm::lookAt(camera->translation,
                         (camera->lookVector + camera->translation),
                         camera->upVector);

    ctrl->clearRotateEventVec();

    float height = calcTerrainInteraction(player->translation);
    player->translation.y = height;
    entity->translation = player->translation;
  }
}


float ie::PhysicsEngine::calcTerrainInteraction(glm::vec3 position)
{
  for (auto it = collidableTerrain.begin(); it != collidableTerrain.end(); it++)
  {
    TerrainNode* terrain = (*it);
    TerrainAsset* terrainAsset = &(*terrains)[terrain->assetId];
    short dim = terrainAsset->dim;
    float unitSize = terrainAsset->unitSize;
    float terrainSize = unitSize * (dim - 1);
    glm::vec3 startTransl = terrain->translation;
    glm::vec3 endTransl((startTransl.x + terrainSize) , startTransl.y,
                          (startTransl.z + terrainSize));
    bool withinX = (position.x > startTransl.x) && (position.x < endTransl.x);
    bool withinZ = (position.z > startTransl.z) && (position.z < endTransl.z);
    if (withinX && withinZ)
    {
      glm::vec2 terrainRelPos(position.x - startTransl.x,
                              position.z - startTransl.z);
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
      return height;
    }
    return position.y;
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

