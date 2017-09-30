//___|"ie_camera.cpp"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_camera.h"

#include <cmath>
#include <iostream>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <SDL2/SDL.h>

#include "ie_const.h"
#include "ie_messages.h"

ie::Camera::Camera()
{
  moveSpeed = 1.0; //Meters per Second
  lookSpeed = glm::radians(0.05f); //Degrees per rel movment
  upVector = glm::vec3(0.0f, 1.0f, 0.0f);
  lookVector = glm::vec3(0.0f, 0.0f, -1.0f);
  posVector = glm::vec3(0.0f, 0.0f, 0.0f);
  translEventVec = glm::vec3(0.0f, 0.0f, 0.0f);
  rotateEventVec = glm::vec2(0.0f, 0.0f);
  projectionMatrix = glm::perspective(glm::radians(60.0f),
                                                  (16.0f / 9.0f),
                                                   0.01f, 100.0f);
  setGrabMode(SDL_TRUE);
}


void ie::Camera::toggleGrabMode(void)
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

//___|UPDATING THE CAMERA FOR THE FRAME|________________________________________

void ie::Camera::frameUpdate(float frameDelta)
{
  float refinedMoveSpeed = 0;
  if (SDL_GetWindowGrab(window) == SDL_TRUE)
  {
    short directionality = std::abs(translEventVec.x) +
                           std::abs(translEventVec.y) +
                           std::abs(translEventVec.z);
    if (directionality == 1)
    {
      refinedMoveSpeed = moveSpeed;
    }
    else if (directionality == 2)
    {
      refinedMoveSpeed = moveSpeed * (1 / std::sqrt(2));
    }
    else if (directionality == 3)
    {
      refinedMoveSpeed = moveSpeed * (1 / std::sqrt(3));
    }
    float delta = frameDelta * float(refinedMoveSpeed / ie::MSECS_PER_SEC); 
    
    posVector += translEventVec.z * delta * lookVector;
    posVector += translEventVec.x * delta * glm::normalize(glm::cross(lookVector,
                                                                      upVector));
    posVector += translEventVec.y * delta * ie::UP_VECTOR;

    glm::vec3 newXAxisVector = glm::normalize(glm::cross(lookVector, upVector));
    glm::mat3 yRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(rotateEventVec.x * lookSpeed),
                                  UP_VECTOR));
    glm::mat3 xRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(rotateEventVec.y * lookSpeed),
                                  newXAxisVector)); 
    glm::mat3 rotation = yRotate * xRotate;
    lookVector = rotation * lookVector;

    float xOffsetAngle = glm::angle(ie::UP_VECTOR, lookVector);
    if (xOffsetAngle > glm::radians(170.0f))
    {
      float correctionAngle = xOffsetAngle - glm::radians(170.0f);
      glm::mat3 correctionRotation = glm::mat3(glm::rotate(glm::mat4(),
                                               correctionAngle,
                                               newXAxisVector));
      lookVector = correctionRotation * lookVector;
    }
    else if (xOffsetAngle < glm::radians(10.0f))
    {
      float correctionAngle = glm::radians(10.0f) - xOffsetAngle;
      glm::mat3 correctionRotation = glm::mat3(glm::rotate(glm::mat4(),
                                               -correctionAngle,
                                               newXAxisVector));
      lookVector = correctionRotation * lookVector;
    }

    viewMatrix = glm::lookAt(posVector, (lookVector+posVector), upVector);
    rotateEventVec = glm::vec2(0.0f, 0.0f);
  }
}

//______________________________________________________________________________

//___|SENDING MESSAGES|_________________________________________________________
ie::RenderCameraMessage ie::Camera::sendRenderCameraMessage(void)
{
  ie::RenderCameraMessage msg;
  msg.cameraPos = posVector;
  msg.projectionMatrix = projectionMatrix;
  msg.viewMatrix = viewMatrix; 
  return msg;
}

//______________________________________________________________________________

//___|GETTERS AND SETTERS|______________________________________________________

glm::mat4 ie::Camera::getViewMatrix(void) {return viewMatrix;}

float ie::Camera::getMoveSpeed(void) {return moveSpeed;}
void ie::Camera::setMoveSpeed(float speed) {moveSpeed = speed;}

float ie::Camera::getLookSpeed(void) {return lookSpeed;}
void ie::Camera::setLookSpeed(float speed) {lookSpeed = speed;}

glm::vec3 ie::Camera::getUpVector(void) {return upVector;}
void ie::Camera::setUpVector(glm::vec3 up) {upVector = up;}

glm::vec3 ie::Camera::getPosVector(void) {return posVector;}
void ie::Camera::setPosVector(glm::vec3 pos) {posVector = pos;}

glm::vec3 ie::Camera::getLookVector(void) {return lookVector;}
void ie::Camera::setLookVector(glm::vec3 look) {lookVector = look;}

glm::mat4 ie::Camera::getProjectionMatrix(void) {return projectionMatrix;}
void ie::Camera::setProjectionMatrix(glm::mat4 matrix) {projectionMatrix = matrix;}

void ie::Camera::setWindow(SDL_Window* win) {window = win;}
SDL_Window* ie::Camera::getWindow(void) {return window;}

void ie::Camera::setGrabMode(SDL_bool mode)
{
  SDL_SetWindowGrab(window, mode);
  SDL_SetRelativeMouseMode(mode);
}

//______________________________________________________________________________
