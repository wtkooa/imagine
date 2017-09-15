#pragma once

#include <glm/glm.hpp>

namespace ie
{

  enum CameraType {FIRST_PERSON, THIRD_PERSON};

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
    glm::vec3 translEventVec;
    glm::vec2 rotateEventVec;    
    private:
    CameraType cameraType;
    const float FIELD_OF_VIEW;
    const float ASPECT_RATIO;
    const float Z_NEAR;
    const float Z_FAR;
    float moveSpeed;
    float lookSpeed;
    glm::vec3 upVector;
    glm::vec3 posVector;
    glm::vec3 lookVector;
    glm::mat4 projectionMatrix;
  };

}
