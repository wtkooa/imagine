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
#include <glm/gtx/vector_angle.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ie_config.h"
#include "ie_const.h"
#include "ie_messages.h"

ie::Camera::Camera()
{
  lookSpeed = ie::DEFAULT_CAMERA_LOOKSPEED; 
  upVector = ie::UP_VECTOR;
  lookVector = glm::vec3(0.0f, 0.0f, -1.0f);
  projectionMatrix = glm::perspective(ie::FIELD_OF_VIEW,
                                      ie::ASPECT_RATIO,
                                      ie::NEAR_PLANE, ie::FAR_PLANE);
  cameraPlayerOffset = glm::vec3(0.0f, 0.0f, 5.0f);
}



//___|UPDATING THE CAMERA FOR THE FRAME|________________________________________

void ie::Camera::update()
{
  if (mode == "firstperson")
  {
    firstPersonUpdate();
  }
}

void ie::Camera::firstPersonUpdate(void)
{

    cameraPosition = playerPosition + cameraPlayerOffset;
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

    viewMatrix = glm::lookAt(cameraPosition, (lookVector + cameraPosition), upVector);
}

//______________________________________________________________________________

//___|SENDING MESSAGES|_________________________________________________________

ie::CameraStatusToRenderMessage ie::Camera::sendCameraStatusToRenderMessage(void)
{
  ie::CameraStatusToRenderMessage msg;
  msg.cameraPos = cameraPosition;
  msg.projectionMatrix = projectionMatrix;
  msg.viewMatrix = viewMatrix; 
  return msg;
}

//______________________________________________________________________________

//___|RECEIVING MESSAGES|_________________________________________________________

void ie::Camera::receiveMessage(ie::TimeStatusMessage msg)
{
  frameDelta = msg.frameDelta;
}


void ie::Camera::receiveMessage(ie::PlayerStatusToCameraMessage msg)
{
  playerPosition = msg.playerPosition;
  playerRotation = msg.playerRotation;
  translEventVec = msg.translEventVec;
  rotateEventVec = msg.rotateEventVec;
  mode = msg.mode;
}

//______________________________________________________________________________

//___|GETTERS AND SETTERS|______________________________________________________

glm::mat4 ie::Camera::getViewMatrix(void) {return viewMatrix;}
glm::mat4 ie::Camera::getProjectionMatrix(void) {return projectionMatrix;}
void ie::Camera::setProjectionMatrix(glm::mat4 matrix) {projectionMatrix = matrix;}

//______________________________________________________________________________
