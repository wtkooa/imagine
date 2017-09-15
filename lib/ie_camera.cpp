#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.

#include "ie_camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ie_const.h"

//Camera
ie::Camera::Camera(void) : FIELD_OF_VIEW(60.0), ASPECT_RATIO(16.0/9.0),
                           Z_NEAR(0.01), Z_FAR(100.0)
{
  CameraType cameraType = FIRST_PERSON;
  moveSpeed = 1.0; //Meters per Second
  lookSpeed = glm::radians(0.05f); //Degrees per rel movment
  upVector = glm::vec3(0.0f, 1.0f, 0.0f);
  lookVector = glm::vec3(0.0f, 0.0f, -1.0f);
  posVector = glm::vec3(0.0f, 0.0f, 0.0f);
  translEventVec = glm::vec3(0.0f, 0.0f, 0.0f);
  rotateEventVec = glm::vec2(0.0f, 0.0f);
  projectionMatrix = glm::perspective(glm::radians(FIELD_OF_VIEW),
                                                   ASPECT_RATIO,
                                                   Z_NEAR, Z_FAR);
}

void ie::Camera::frameUpdate(float frameDelta)
{
  switch (cameraType)
  {
    case FIRST_PERSON:
      float delta = frameDelta * float(moveSpeed / ie::MSECS_PER_SEC); 
      posVector += translEventVec.z * delta * lookVector;
      posVector += translEventVec.x * delta * glm::cross(lookVector, upVector);
      posVector += translEventVec.y * delta * upVector;
      lookVector = glm::mat3(glm::rotate(glm::mat4(),
                             -float(rotateEventVec.x * lookSpeed),
                             upVector)) * lookVector;
      glm::vec3 newXAxisVector = glm::cross(lookVector, upVector);
      lookVector = glm::mat3(glm::rotate(glm::mat4(),
                             -float(rotateEventVec.y * lookSpeed),
                             newXAxisVector)) * lookVector; 
      rotateEventVec = glm::vec2(0.0f, 0.0f);
    break;
  }
}

glm::mat4 ie::Camera::getViewMatrix(void)
{
  return glm::lookAt(posVector, (lookVector+posVector), upVector);
}

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
