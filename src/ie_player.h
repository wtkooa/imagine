#ifndef IE_PLAYER_H
#define IE_PLAYER_H

//___|"ie_player.h"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <string>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "ie_assets.h"
#include "ie_controller.h"
#include "ie_messages.h"

namespace ie
{

  class Player
  {
    public:
    Player();
    void update(void);
    void firstPersonUpdate(void);

    //SENDING MESSAGES
    ie::PlayerStatusToCameraMessage sendPlayerStatusToCameraMessage(void);
    ie::AssetManagerInstructions sendAssetManagerInstructionsMessage(void);

    //RECEIVING MESSAGES
    void receiveMessage(ie::TimeStatusMessage);
    void receiveMessage(ie::ControllerStatusMessage);

    //SETTERS AND GETTERS
    void setPlayerPosition(glm::vec3);
    void setPlayerRotation(glm::vec3);


    private:
    glm::vec3* translEventVec;
    glm::vec2* rotateEventVec;    
    Controller* controller;
    std::string mode;
    std::string linkedEntityName;
    glm::vec3 playerPosition;
    glm::vec3 playerRotation;
    glm::vec3 upVector;
    float frameDelta;
    float moveSpeed;
    float turnSpeed;

    std::vector<AssetManagerInstruction> instructions;

  };
}

#endif
