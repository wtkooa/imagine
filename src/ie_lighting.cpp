//___|"ie_lighting.cpp"|________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_lighting.h"

#include <iostream>

#include <glm/glm.hpp>

#include "ie_packages.h"

ie::PointLight::PointLight()
{
  posVector = glm::vec3(0.0f, 0.0f, 0.0f);
  globalAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
  lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
  lightSpecular = glm::vec3(0.4f, 0.4f, 0.4f);
  lightDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
  constantFalloff = 1.0f;
  linearFalloff = 0.1f;
  quadraticFalloff = 0.0f;
}

void ie::PointLight::setName(std::string n) {name = n;} 
void ie::PointLight::setPosVector(glm::vec3 pos) {posVector = pos;}
void ie::PointLight::setGlobalAmbient(glm::vec3 gamb) {globalAmbient = gamb;}
void ie::PointLight::setLightAmbient(glm::vec3 lamb) {lightAmbient = lamb;}
void ie::PointLight::setLightSpecular(glm::vec3 lspc) {lightSpecular = lspc;}
void ie::PointLight::setLightDiffuse(glm::vec3 ldif) {lightDiffuse = ldif;}
void ie::PointLight::setConstantFalloff(float c) {constantFalloff = c;}
void ie::PointLight::setLinearFalloff(float l) {linearFalloff = l;}
void ie::PointLight::setQuadraticFalloff(float q) {quadraticFalloff = q;}

ie::LightPackage ie::PointLight::wrapLightPackage(void)
{
  ie::LightPackage package;
  package.name = name;
  package.posVector = posVector;
  package.globalAmbient = globalAmbient;
  package.lightAmbient = lightAmbient;
  package.lightSpecular = lightSpecular;
  package.lightDiffuse = lightDiffuse;
  package.constantFalloff = constantFalloff;
  package.linearFalloff = linearFalloff;
  package.quadraticFalloff = quadraticFalloff;
  return package;
}
