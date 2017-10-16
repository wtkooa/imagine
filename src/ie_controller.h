#ifndef IE_CONTROLLER_H
#define IE_CONTROLLER_H

//___|"ie_controller.h"|________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <SDL2/SDL.h>

#include "ie_messages.h"

namespace ie
{

  class Controller
  {
    public:
    Controller();
    void toggleGrabMode(void);
    void setWindow(SDL_Window*);
    void clearRotateEventVec(void);
    bool getGrabMode(void);
    ie::ControllerStatusMessage sendControllerStatusMessage(void);

    //EVENT VECTORS
    glm::vec3 translEventVec;
    glm::vec2 rotateEventVec;    

    private:
    void setGrabMode(SDL_bool);
    SDL_Window * window;
  };

}

#endif
