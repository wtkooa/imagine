#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.

#include "ie_lighting.h"

#include <glm/glm.hpp>

glm::vec3 ie::PointLight::globalAmbient = glm::vec3(0.1f, 0.1f, 0.1f);

ie::PointLight::PointLight()
{
  posVector = glm::vec3(0.0f, 0.0f, 0.0f);
  lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
  lightSpecular = glm::vec3(0.4f, 0.4f, 0.4f);
  lightDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
  constantFalloff = 1.0f;
  linearFalloff = 0.1f;
  quadraticFalloff = 0.0f;
}

glm::vec3 ie::PointLight::getGlobalAmbient(void) {return globalAmbient;}
void ie::PointLight::setGlobalAmbient(glm::vec3 gamb) {globalAmbient = gamb;}

glm::vec3 ie::PointLight::getPosVector(void) {return posVector;}
void ie::PointLight::setPosVector(glm::vec3 pos) {posVector = pos;}

glm::vec3 ie::PointLight::getLightAmbient(void) {return lightAmbient;}
void ie::PointLight::setLightAmbient(glm::vec3 lamb) {lightAmbient = lamb;}

glm::vec3 ie::PointLight::getLightSpecular(void) {return lightSpecular;}
void ie::PointLight::setLightSpecular(glm::vec3 lspc) {lightSpecular = lspc;}

glm::vec3 ie::PointLight::getLightDiffuse(void) {return lightDiffuse;}
void ie::PointLight::setLightDiffuse(glm::vec3 ldif) {lightDiffuse = ldif;}

float ie::PointLight::getConstantFalloff(void) {return constantFalloff;}
void ie::PointLight::setConstantFalloff(float c) {constantFalloff = c;}

float ie::PointLight::getLinearFalloff(void) {return linearFalloff;}
void ie::PointLight::setLinearFalloff(float l) {linearFalloff = l;}

float ie::PointLight::getQuadraticFalloff(void) {return quadraticFalloff;}
void ie::PointLight::setQuadraticFalloff(float q) {quadraticFalloff = q;}
