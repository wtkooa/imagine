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

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <SDL2/SDL.h>

#include "ie_enum.h"
#include "ie_log.h"

namespace ie
{


  class Controller
  {
    public:
    Controller();

    void reset(void);

    void update(void);

    void setLog(Log*);
    void setEngineOnOffSwitch(bool*);
    void setWindow(SDL_Window*);
    void setMode(IEenum);

    IEenum getMode(void);
    glm::vec3 getTranslEvent(void);
    glm::vec2 getRotateEvent(void);

    void toggleGrabMode(void);
    void togglePlayerMode(void);

    void clearRotateEventVec(void);
    void clearScrollEvent(void);

    private:
    Log* log;

    void handleEvents(void);
    void handleResize(int width, int height);

    void setGrabMode(SDL_bool);
    bool getGrabMode(void);

    bool* engineRun;
    SDL_Window * window;
    IEenum mode;

    glm::vec3 translEvent;
    glm::vec2 rotateEvent;    
    int scrollEvent;
  };

}

#endif
