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

namespace ie
{

  enum ControlMode {FIRST_PERSON, THIRD_PERSON, THIRD_PERSON_RPG, FLYAROUND,
                    EDITING};

  class Controller
  {
    public:
    Controller();
    void toggleGrabMode(void);
    void togglePlayerMode(void);
    void setWindow(SDL_Window*);
    void clearRotateEventVec(void);
    bool getGrabMode(void);

    ControlMode mode;

    //EVENT VECTORS
    glm::vec3 translEventVec;
    glm::vec2 rotateEventVec;    
    int scrollEvent;

    private:
    void setGrabMode(SDL_bool);
    SDL_Window * window;
  };

}

#endif
