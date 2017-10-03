#ifndef IE_CAMERA_H
#define IE_CAMERA_H

//___|"ie_camera.h"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <SDL2/SDL.h>

#include "ie_messages.h"

namespace ie
{

  class Camera
  {
    public:
    Camera();

    void frameUpdate(void);
    void toggleGrabMode(void);

    //SENDING MESSAGES
    CameraStatusToRenderMessage sendCameraStatusToRenderMessage(void);

    //RECEIVING MESSAGES
    void receiveMessage(ie::TimeStatusToCameraMessage);

    //GETTERS AND SETTERS
    glm::mat4 getViewMatrix(void);
    float getMoveSpeed(void);
    void setMoveSpeed(float);
    float getLookSpeed(void);
    void setLookSpeed(float);
    glm::vec3 getUpVector(void);
    void setUpVector(glm::vec3);
    glm::vec3 getPosVector(void);
    void setPosVector(glm::vec3);
    glm::vec3 getLookVector(void);
    void setLookVector(glm::vec3);
    glm::mat4 getProjectionMatrix(void);
    void setProjectionMatrix(glm::mat4);
    void setGrabMode(SDL_bool);
    void setWindow(SDL_Window*);
    SDL_Window* getWindow(void);

    //EVENT VECTORS
    glm::vec3 translEventVec;
    glm::vec2 rotateEventVec;    

    private:
    SDL_Window * window;
    float moveSpeed;
    float lookSpeed;
    float frameDelta;
    glm::vec3 upVector;
    glm::vec3 posVector;
    glm::vec3 lookVector;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
  };

}

#endif
