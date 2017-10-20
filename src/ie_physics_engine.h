#ifndef IE_PHYSICS_ENGINE_H
#define IE_PHYSICS_ENGINE_H

//___|"ie_physics_engine.h"|____________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <vector>

#include "ie_controller.h"
#include "ie_messages.h"
#include "ie_nodes.h"

namespace ie
{

  class PhysicsEngine
  {
    public:

    void update(void);
    void updatePlayers(PhysicsBucket*);
    void updatePlayerFirstPerson(PlayerNode*);

    void setController(Controller*);

    void receiveMessage(ie::TimeStatusMessage);
    void receiveMessage(ie::ControllerStatusMessage);
    void receiveMessage(ie::GraphStatusMessage);

    private:
    float frameDelta;
    PhysicsBucket* firstBucket;
    Controller* control;
    std::vector<PlayerNode*> players;

  };
}

#endif
