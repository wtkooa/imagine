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

#include "ie_messages.h"

namespace ie
{

  class Camera
  {
    public:
    Camera();

    void update(void);
    void firstPersonUpdate(void);

    //SENDING MESSAGES
    CameraStatusToRenderMessage sendCameraStatusToRenderMessage(void);

    //RECEIVING MESSAGES
    void receiveMessage(ie::TimeStatusMessage);
    void receiveMessage(ie::PlayerStatusToCameraMessage);

    //GETTERS AND SETTERS
    glm::mat4 getViewMatrix(void);
    glm::mat4 getProjectionMatrix(void);
    void setProjectionMatrix(glm::mat4);

    private:
    
    //DATA FROM TIME
    float frameDelta;

    //DATA FROM PLAYER
    glm::vec3 playerPosition;
    glm::vec3 playerRotation;
    glm::vec3 translEventVec;
    glm::vec2 rotateEventVec;    
    std::string mode;

    glm::vec3 upVector;
    glm::vec3 lookVector;
    glm::vec3 cameraPlayerOffset;
    glm::vec3 cameraPosition;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    float lookSpeed;
  };

}

#endif
