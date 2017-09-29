#ifndef IE_LIGHTING_H
#define IE_LIGHTING_H

#include <glm/glm.hpp>

#include "ie_messages.h"

namespace ie
{
  
  class PointLight
  {
    public:
    PointLight();
    glm::vec3 getGlobalAmbient(void);
    void setGlobalAmbient(glm::vec3);
    glm::vec3 getPosVector(void);
    void setPosVector(glm::vec3);
    glm::vec3 getLightAmbient(void);
    void setLightAmbient(glm::vec3);
    glm::vec3 getLightSpecular(void);
    void setLightSpecular(glm::vec3);
    glm::vec3 getLightDiffuse(void);
    void setLightDiffuse(glm::vec3);
    float getConstantFalloff(void);
    void setConstantFalloff(float);
    float getLinearFalloff(void);
    void setLinearFalloff(float);
    float getQuadraticFalloff(void);
    void setQuadraticFalloff(float);
    RenderLightMessage sendRenderLightMessage(void);
    private:
    static glm::vec3 globalAmbient;
    glm::vec3 posVector;
    glm::vec3 lightAmbient;
    glm::vec3 lightSpecular;
    glm::vec3 lightDiffuse;
    float constantFalloff;
    float linearFalloff;
    float quadraticFalloff;
  };

}

#endif
