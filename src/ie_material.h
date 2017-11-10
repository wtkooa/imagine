#ifndef IE_MATERIAL_H
#define IE_MATERIAL_H

//___|"ie_material.h"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_asset.h"

#include <glm/vec3.hpp>

#include "ie_texture.h"

namespace ie
{

  class Material : public Asset
  {
    public:
    Material();

    void setShininess(float);
    void setAmbient(glm::vec3);
    void setDiffuse(glm::vec3);
    void setSpecular(glm::vec3);
    void setEmission(glm::vec3);
    void setUseGlobalAmbient(bool);
    void setUseLightAmbient(bool);
    void setUseLightDiffuse(bool);
    void setUseLightSpecular(bool);
    void setUseLightFalloff(bool);
    void setDiffuseTexture(Texture*);
    void setNormalTexture(Texture*);
    float getShininess(void);
    glm::vec3 getAmbient(void);
    glm::vec3 getDiffuse(void);
    glm::vec3 getSpecular(void);
    glm::vec3 getEmission(void);
    bool getUseGlobalAmbient(void);
    bool getUseLightAmbient(void);
    bool getUseLightDiffuse(void);
    bool getUseLightSpecular(void);
    bool getUseLightFalloff(void);
    Texture* getDiffuseTexture(void);
    Texture* getNormalTexture(void);

    private:
    float shininess;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emission;
    bool usesGlobalAmbient;
    bool usesLightAmbient;
    bool usesLightDiffuse;
    bool usesLightSpecular;
    bool usesLightFalloff;
    Texture* diffuseTexture; 
    Texture* normalTexture;
  };

}

#endif
