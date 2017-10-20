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

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

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

void ie::PhysicsEngine::setController(ie::Controller* ctrl) {control = ctrl;}

void ie::PhysicsEngine::update(void)
{
  
  PhysicsBucket* currentBucket = firstBucket;
  while (currentBucket != NULL)
  {
    if (currentBucket->type == TERRAIN)
    {
     //std::cout << "Terrain"  << std::endl; 
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
    (*player).rotation = yRotate * player->rotation; 

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
  }
}
