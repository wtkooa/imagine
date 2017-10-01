#ifndef IE_LIGHTING_H
#define IE_LIGHTING_H

//___|"ie_lighting.h"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <string>

#include <glm/glm.hpp>

#include "ie_packages.h"

namespace ie
{
  
  class PointLight
  {
    public:
    PointLight();


    //SETTERS
    void setName(std::string);
    void setGlobalAmbient(glm::vec3);
    void setPosVector(glm::vec3);
    void setLightAmbient(glm::vec3);
    void setLightSpecular(glm::vec3);
    void setLightDiffuse(glm::vec3);
    void setConstantFalloff(float);
    void setLinearFalloff(float);
    void setQuadraticFalloff(float);
    
    //WRAPPING PACKAGES
    LightPackage wrapLightPackage(void);

    private:
    std::string name;
    glm::vec3 posVector;
    glm::vec3 globalAmbient;
    glm::vec3 lightAmbient;
    glm::vec3 lightSpecular;
    glm::vec3 lightDiffuse;
    float constantFalloff;
    float linearFalloff;
    float quadraticFalloff;
  };

}

#endif
