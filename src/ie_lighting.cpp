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

ie::LightGenerator::LightGenerator()
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

void ie::LightGenerator::setName(std::string n) {name = n;} 
void ie::LightGenerator::setPosVector(glm::vec3 pos) {posVector = pos;}
void ie::LightGenerator::setGlobalAmbient(glm::vec3 gamb) {globalAmbient = gamb;}
void ie::LightGenerator::setLightAmbient(glm::vec3 lamb) {lightAmbient = lamb;}
void ie::LightGenerator::setLightSpecular(glm::vec3 lspc) {lightSpecular = lspc;}
void ie::LightGenerator::setLightDiffuse(glm::vec3 ldif) {lightDiffuse = ldif;}
void ie::LightGenerator::setConstantFalloff(float c) {constantFalloff = c;}
void ie::LightGenerator::setLinearFalloff(float l) {linearFalloff = l;}
void ie::LightGenerator::setQuadraticFalloff(float q) {quadraticFalloff = q;}

ie::LightPackage ie::LightGenerator::wrapLightPackage(void)
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
