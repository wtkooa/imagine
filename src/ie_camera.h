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

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "ie_messages.h"

namespace ie
{

  class Camera
  {
    public:
    Camera();
    glm::mat4 getViewMatrix(void);
    void frameUpdate(float);
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
    void toggleGrabMode(void);
    void setWindow(SDL_Window*);
    RenderCameraMessage sendRenderCameraMessage(void);
    SDL_Window* getWindow(void);
    glm::vec3 translEventVec;
    glm::vec2 rotateEventVec;    
    private:
    SDL_Window * window;
    float moveSpeed;
    float lookSpeed;
    glm::vec3 upVector;
    glm::vec3 posVector;
    glm::vec3 lookVector;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
  };

}

#endif
