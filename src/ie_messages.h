#ifndef IE_MESSAGES_H
#define IE_MESSAGES_H

//___|"ie_messages.h"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________
#include <map>
#include <set>
#include <string>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "ie_vram.h"

namespace ie
{
  class TimeStatusMessage
  {
    public:
    float frameDelta;
  };
/*
  class RenderBucket; //Forward Declaration
  class GraphNode;
  class PhysicsBucket;
  class GraphStatusMessage
  {
    public:
    RenderBucket* bucket;
    PhysicsBucket* phyBucket;
    GraphNode* root;
    
  };
*/
  class Controller; //Forward Declaration
  class ControllerStatusMessage
  {
    public:
    Controller* controller;
    glm::vec3* translEventVec;
    glm::vec2* rotateEventVec;    
  };

}

#endif
