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

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <SDL2/SDL.h>

#include "ie_communication.h"
#include "ie_enum.h"
#include "ie_log.h"
#include "ie_messages.h"

ie::Controller::Controller()
{
  reset();
}


void ie::Controller::reset(void)
{
  translEvent = glm::vec3(0.0f, 0.0f, 0.0f);
  rotateEvent = glm::vec2(0.0f, 0.0f);
  setGrabMode(SDL_TRUE);
  mode = IE_FIRST_PERSON_MODE;
}


void ie::Controller::init(void)
{
  nexus->addConnection(IE_CONTROLLER_ICOM_HANDLE, this);
  NexusMsg msg(IE_LOG_ICOM_HANDLE, IE_NEXUS_OUT_CONNECTION_CMD, this);
  nexus->rxMsg(&msg); 
  msg.target = IE_ENGINE_ICOM_HANDLE;
  nexus->rxMsg(&msg); 
}


void ie::Controller::update(void)
{
  handleEvents();
}


void ie::Controller::setGrabMode(SDL_bool mode)
{
  SDL_SetWindowGrab(window, mode);
  SDL_SetRelativeMouseMode(mode);
}


glm::vec3 ie::Controller::getTranslEvent(void)
{
  return translEvent;
}


glm::vec2 ie::Controller::getRotateEvent(void)
{
  return rotateEvent;
}


void ie::Controller::toggleGrabMode(void)
{
  if (SDL_GetWindowGrab(window) == SDL_FALSE) 
  {
    //log->info("Toggled Grab Mode On");
    setGrabMode(SDL_TRUE);
  }
  else
  {
    //log->info("Toggled Grab Mode Off");
    setGrabMode(SDL_FALSE);
  } 
}

void ie::Controller::togglePlayerMode(void)
{
  if (getGrabMode())
  {
    if (mode == IE_FIRST_PERSON_MODE)
    {
      //log->info("Toggled Third Person Mode");
      mode = IE_THIRD_PERSON_MODE;
    }
    else if (mode == IE_THIRD_PERSON_MODE)
    {
      //log->info("Toggled First Person Mode");
      mode = IE_FIRST_PERSON_MODE;
    }
  }
}

bool ie::Controller::getGrabMode(void)
{
  return SDL_GetWindowGrab(window) == SDL_TRUE;
}

void ie::Controller::clearRotateEventVec(void)
{
  rotateEvent = glm::vec2(0.0f);
}


void ie::Controller::clearScrollEvent(void)
{
  scrollEvent = 0;
}


void ie::Controller::setWindow(SDL_Window* win) {window = win;}


void ie::Controller::handleEvents(void)
{
  SDL_Event evnt;
  while (SDL_PollEvent(&evnt))
  {
    switch (evnt.type)
    {

      case SDL_QUIT:
        handleShutdown();
      break;

      case SDL_WINDOWEVENT:
        switch (evnt.window.event)
        {
          case SDL_WINDOWEVENT_RESIZED:
            handleResize(evnt.window.data1, evnt.window.data2);
            break;
        }
        break;
      case SDL_MOUSEMOTION:
        if (SDL_GetRelativeMouseMode() == SDL_TRUE)
        {
          rotateEvent.x += evnt.motion.xrel;  
          rotateEvent.y += evnt.motion.yrel;  
        }
        break;
      case SDL_MOUSEWHEEL:
        if (evnt.wheel.y == 1)
        {
          scrollEvent += evnt.wheel.y;
        }
        else if (evnt.wheel.y == -1)
        {
          scrollEvent += evnt.wheel.y;
        }
        break;
      case SDL_KEYDOWN:
        switch (evnt.key.keysym.sym)
        {

          case SDLK_ESCAPE:
            handleShutdown();
          break;

          case SDLK_e:
            if (!evnt.key.repeat)
            {
              toggleGrabMode();
            }
            break;
          case SDLK_c:
            if (!evnt.key.repeat)
            {
              togglePlayerMode();
            }
            break;
          case SDLK_w:
            if (!evnt.key.repeat)
            { 
              translEvent.z += 1.0;
            }
            break;
          case SDLK_s:
            if (!evnt.key.repeat)
            {
              translEvent.z -= 1.0;
            }
            break;
          case SDLK_d:
            if (!evnt.key.repeat)
            {
              translEvent.x += 1.0;
            }
            break;
          case SDLK_a:
            if (!evnt.key.repeat)
            {
              translEvent.x -= 1.0;
            }
            break;
          case SDLK_SPACE:
            if (!evnt.key.repeat)
            {
              translEvent.y += 1.0;
            }
            break;
          case SDLK_LSHIFT:
            if (!evnt.key.repeat)
            {
              translEvent.y -= 1.0;
            }
            break;
        }
        break;        
      case SDL_KEYUP:
        switch(evnt.key.keysym.sym)
        {
          case SDLK_w:
              translEvent.z -= 1.0;
            break;
          case SDLK_s:
              translEvent.z += 1.0;
            break;
          case SDLK_d:
              translEvent.x -= 1.0;
            break;
          case SDLK_a:
              translEvent.x += 1.0;
            break;
          case SDLK_SPACE:
              translEvent.y -= 1.0;
            break;
          case SDLK_LSHIFT:
              translEvent.y += 1.0;
            break;
        }
        break;
    }
  }
}


void ie::Controller::handleShutdown(void)
{
  SystemMsg sys(IE_SYSTEM_SHUTDOWN_CMD);
  txMsg(&sys);
}


void ie::Controller::rxMsg(Imessage*)
{
  //TODO populate
}


void ie::Controller::handleResize(int width, int height)
{
  float aspectRatio = float(width) / float(height);
  SDL_SetWindowSize(window, width, height);
  glViewport(0, 0, width, height);
  //sg.updateAspectRatio(aspectRatio);
}
