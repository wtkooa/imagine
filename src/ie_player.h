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
#include <SDL2/SDL.h>

#include "ie_assets.h"
#include "ie_messages.h"

namespace ie
{

  class Player
  {
    public:
    Player();
    void update(void);
    void toggleGrabMode(void);
    void toggleHideEntity(void);
    void firstPersonUpdate(void);

    //SENDING MESSAGES
    ie::PlayerStatusToCameraMessage sendPlayerStatusToCameraMessage(void);
    ie::AssetManagerInstructions sendAssetManagerInstructionsMessage(void);

    //RECEIVING MESSAGES
    void receiveMessage(ie::AssetStatusToPlayerMessage);
    void receiveMessage(ie::TimeStatusMessage);

    //SETTERS AND GETTERS
    void setWindow(SDL_Window*);
    void setPlayerEntity(std::string);
    void setPlayerPosition(glm::vec3);
    void setPlayerRotation(glm::vec3);

    //EVENT VECTOR
    glm::vec3 translEventVec;
    glm::vec2 rotateEventVec;    

    private:
    void setGrabMode(SDL_bool);

    std::string mode;
    Entity* entity;
    SDL_Window * window;
    std::string linkedEntityName;
    std::map<unsigned int, Entity>* entities;
    std::map<std::string, unsigned int>* entityNameIdMap;
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
