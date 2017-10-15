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
#include <SDL2/SDL.h>

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
  translEventVec = glm::vec3(0.0f, 0.0f, 0.0f);
  rotateEventVec = glm::vec2(0.0f, 0.0f);

  setGrabMode(SDL_TRUE);
}

void ie::Player::update(void)
{
  unsigned int entityId = (*entityNameIdMap)[linkedEntityName];
  entity = &(*entities)[entityId];

  if (mode == "firstperson")
  {
    firstPersonUpdate();
  }

}

void ie::Player::firstPersonUpdate(void)
{
  float refinedMoveSpeed = 0;
  if (SDL_GetWindowGrab(window) == SDL_TRUE)
  {
    short directionality = std::abs(translEventVec.x) +
                           std::abs(translEventVec.z);
    if (directionality == 1)
    {
      refinedMoveSpeed = moveSpeed;
    }
    else if (directionality == 2)
    {
      refinedMoveSpeed = moveSpeed * (1 / std::sqrt(2));
    }

    float delta = frameDelta * float(refinedMoveSpeed / ie::MSECS_PER_SEC); 
    
    playerPosition += translEventVec.z * delta * playerRotation;
    playerPosition += translEventVec.x * delta * glm::normalize(
                                                   glm::cross(playerRotation,
                                                   upVector));

    glm::mat3 yRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(rotateEventVec.x * turnSpeed),
                                  upVector));
    playerRotation = yRotate * playerRotation; 

    glm::mat4 tmat = glm::translate(glm::mat4(), playerPosition);
    (*entity).translationMatrix = tmat;
  }
}


void ie::Player::setGrabMode(SDL_bool mode)
{
  SDL_SetWindowGrab(window, mode);
  SDL_SetRelativeMouseMode(mode);
}


void ie::Player::toggleGrabMode(void)
{
  if (SDL_GetWindowGrab(window) == SDL_FALSE) 
  {
    std::cout << "GrabMode On" << std::endl;
    setGrabMode(SDL_TRUE);
  }
  else
  {
    std::cout << "GrabMode Off" << std::endl;
    setGrabMode(SDL_FALSE);
  } 
}

//___|SENDING MESSAGES|_________________________________________________________

ie::PlayerStatusToCameraMessage ie::Player::sendPlayerStatusToCameraMessage(void)
{
  ie::PlayerStatusToCameraMessage msg;
  msg.playerPosition = playerPosition;
  msg.playerRotation = playerRotation;
  msg.translEventVec = translEventVec;
  msg.rotateEventVec = rotateEventVec;
  rotateEventVec = glm::vec2(0.0f, 0.0f); //Needs to be reset after sending message
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

void ie::Player::receiveMessage(ie::AssetStatusToPlayerMessage msg)
{
  entities = msg.entities;
  entityNameIdMap = msg.entityNameIdMap;
}


void ie::Player::receiveMessage(ie::TimeStatusMessage msg)
{
  frameDelta = msg.frameDelta;
}

//______________________________________________________________________________

//___|GETTERS AND SETTERS|______________________________________________________

void ie::Player::setWindow(SDL_Window* win) {window = win;}
void ie::Player::setPlayerEntity(std::string entityName) {linkedEntityName = entityName;}
void ie::Player::setPlayerPosition(glm::vec3 pos) {playerPosition = pos;}
void ie::Player::setPlayerRotation(glm::vec3 rot) {playerRotation = rot;}

//______________________________________________________________________________
