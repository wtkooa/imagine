//___|"ie_material.cpp"|________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_material.h"

#include <glm/vec3.hpp>

#include "ie_texture.h"

ie::Material::Material()
{
  shininess = 1;
  ambient = glm::vec3(1.0f, 1.0f, 1.0f);
  diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
  specular = glm::vec3(0.0f, 0.0f, 0.0f);
  emission = glm::vec3(0.0f, 0.0f, 0.0f);
  usesGlobalAmbient = true;
  usesLightAmbient = true;
  usesLightDiffuse = true;
  usesLightSpecular = true;
  usesLightFalloff = true;
  diffuseTexture = NULL;
  normalTexture = NULL;
}


void ie::Material::setShininess(float s) {shininess = s;}
float ie::Material::getShininess(void) {return shininess;}


void ie::Material::setAmbient(glm::vec3 a) {ambient = a;}
glm::vec3 ie::Material::getAmbient(void) {return ambient;}


void ie::Material::setDiffuse(glm::vec3 d) {diffuse = d;}
glm::vec3 ie::Material::getDiffuse(void) {return diffuse;}


void ie::Material::setSpecular(glm::vec3 s) {specular = s;}
glm::vec3 ie::Material::getSpecular(void) {return specular;}


void ie::Material::setEmission(glm::vec3 e) {emission = e;}
glm::vec3 ie::Material::getEmission(void) {return emission;}


void ie::Material::setUseGlobalAmbient(bool ga) {usesGlobalAmbient = ga;}
bool ie::Material::getUseGlobalAmbient(void) {return usesGlobalAmbient;}


void ie::Material::setUseLightAmbient(bool la) {usesLightAmbient = la;}
bool ie::Material::getUseLightAmbient(void) {return usesLightAmbient;}


void ie::Material::setUseLightDiffuse(bool ld) {usesLightDiffuse = ld;}
bool ie::Material::getUseLightDiffuse(void) {return usesLightDiffuse;}


void ie::Material::setUseLightSpecular(bool ls) {usesLightSpecular = ls;}
bool ie::Material::getUseLightSpecular(void) {return usesLightSpecular;}


void ie::Material::setUseLightFalloff(bool lf) {usesLightFalloff = lf;}
bool ie::Material::getUseLightFalloff(void) {return usesLightFalloff;}


void ie::Material::setDiffuseTexture(Texture* t) {diffuseTexture = t;}
ie::Texture* ie::Material::getDiffuseTexture(void) {return diffuseTexture;}


void ie::Material::setNormalTexture(Texture* n) {normalTexture = n;}
ie::Texture* ie::Material::getNormalTexture(void) {return normalTexture;}
