//___|"ie_player.cpp"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_player.h"

#include <iostream>
#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "ie_assets.h"
#include "ie_config.h"
#include "ie_const.h"
#include "ie_messages.h"

ie::Player::Player()
{
  moveSpeed = ie::DEFAULT_PLAYER_MOVESPEED;
  turnSpeed = ie::DEFAULT_PLAYER_TURNSPEED; 
  upVector = ie::UP_VECTOR;
  mode = "firstperson";

}

void ie::Player::update(void)
{
  if (mode == "firstperson")
  {
    firstPersonUpdate();
  }
}

void ie::Player::firstPersonUpdate(void)
{
  float refinedMoveSpeed = 0;
  if (controller->getGrabMode())
  {

    short directionality = std::abs(translEventVec->x) +
                           std::abs(translEventVec->z);
    if (directionality == 1)
    {
      refinedMoveSpeed = moveSpeed;
    }
    else if (directionality == 2)
    {
      refinedMoveSpeed = moveSpeed * (1 / std::sqrt(2));
    }

    float delta = frameDelta * float(refinedMoveSpeed / ie::MSECS_PER_SEC); 
    
    playerPosition += translEventVec->z * delta * playerRotation;
    playerPosition += translEventVec->x * delta * glm::normalize(
                                                   glm::cross(playerRotation,
                                                   upVector));

    glm::mat3 yRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(rotateEventVec->x * turnSpeed),
                                  upVector));
    playerRotation = yRotate * playerRotation; 

  }
}



//___|SENDING MESSAGES|_________________________________________________________

ie::PlayerStatusToCameraMessage ie::Player::sendPlayerStatusToCameraMessage(void)
{
  ie::PlayerStatusToCameraMessage msg;
  msg.playerPosition = playerPosition;
  msg.playerRotation = playerRotation;
  msg.translEventVec = *translEventVec;
  msg.rotateEventVec = *rotateEventVec;
  controller->clearRotateEventVec(); //Needs to be reset after sending message
  msg.mode = mode;
  return msg;
}

ie::AssetManagerInstructions ie::Player::sendAssetManagerInstructionsMessage(void)
{
  ie::AssetManagerInstructions msg;
  msg.instructions = instructions;
  instructions.clear();
  return msg;
}

//______________________________________________________________________________

//___|RECEIVING MESSAGES|_______________________________________________________

void ie::Player::receiveMessage(ie::TimeStatusMessage msg)
{
  frameDelta = msg.frameDelta;
}

void ie::Player::receiveMessage(ie::ControllerStatusMessage msg)
{
  controller = msg.controller;
  translEventVec = msg.translEventVec;
  rotateEventVec = msg.rotateEventVec;
}

//______________________________________________________________________________

//___|GETTERS AND SETTERS|______________________________________________________

void ie::Player::setPlayerPosition(glm::vec3 pos) {playerPosition = pos;}
void ie::Player::setPlayerRotation(glm::vec3 rot) {playerRotation = rot;}

//______________________________________________________________________________
