//___|"ie_controller.cpp"|______________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_controller.h"

#include <iostream>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <SDL2/SDL.h>

#include "ie_messages.h"

ie::Controller::Controller()
{
  translEventVec = glm::vec3(0.0f, 0.0f, 0.0f);
  rotateEventVec = glm::vec2(0.0f, 0.0f);
  setGrabMode(SDL_TRUE);
}

void ie::Controller::setGrabMode(SDL_bool mode)
{
  SDL_SetWindowGrab(window, mode);
  SDL_SetRelativeMouseMode(mode);
}


void ie::Controller::toggleGrabMode(void)
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

bool ie::Controller::getGrabMode(void)
{
  return SDL_GetWindowGrab(window) == SDL_TRUE;
}

void ie::Controller::clearRotateEventVec(void)
{
  rotateEventVec = glm::vec2(0.0f);
}

ie::ControllerStatusMessage ie::Controller::sendControllerStatusMessage(void)
{
  ie::ControllerStatusMessage msg;
  msg.controller = this;
  msg.translEventVec = &translEventVec;
  msg.rotateEventVec = &rotateEventVec;
  return msg;
}

void ie::Controller::setWindow(SDL_Window* win) {window = win;}
