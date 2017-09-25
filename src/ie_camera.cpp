#include "ie_camera.h"

#include <iostream>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>

#include "ie_const.h"
#include "ie_messages.h"

//Camera
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

void ie::Camera::setGrabMode(SDL_bool mode)
{
  SDL_SetWindowGrab(window, mode);
  SDL_SetRelativeMouseMode(mode);
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
void ie::Camera::setWindow(SDL_Window* win)
{
  window = win;
}

SDL_Window* ie::Camera::getWindow(void)
{
  return window;
}

void ie::Camera::frameUpdate(float frameDelta)
{
  if (SDL_GetWindowGrab(window) == SDL_TRUE)
  {
    float delta = frameDelta * float(moveSpeed / ie::MSECS_PER_SEC); 
    posVector += translEventVec.z * delta * lookVector;
    posVector += translEventVec.x * delta * glm::cross(lookVector, upVector);
    posVector += translEventVec.y * delta * upVector;
    glm::mat3 yRotate = glm::mat3(glm::rotate(glm::mat4(),
                                  -float(rotateEventVec.x * lookSpeed),
                                  upVector));
    lookVector = yRotate * lookVector;
    glm::vec3 newXAxisVector = glm::cross(lookVector, upVector);
    glm::mat3 xRotate = glm::mat3(glm::rotate(glm::mat4(),
                           -float(rotateEventVec.y * lookSpeed),
                           newXAxisVector));
    lookVector =  xRotate * lookVector; 
    upVector = xRotate * upVector;
    rotateEventVec = glm::vec2(0.0f, 0.0f);
    viewMatrix = glm::lookAt(posVector, (lookVector+posVector), upVector);
  }
}


ie::RenderCameraMessage ie::Camera::sendRenderCameraMessage(void)
{
  ie::RenderCameraMessage msg;
  msg.cameraPos = posVector;
  msg.projectionMatrix = projectionMatrix;
  msg.viewMatrix = viewMatrix; 
  return msg;
}

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
