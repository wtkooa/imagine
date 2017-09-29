#ifndef IE_RENDER_H
#define IE_RENDER_H

//___|"ie_render.h"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <map>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "ie_assets.h"
#include "ie_definitions.h"
#include "ie_messages.h"

namespace ie
{

  class StaticRender
  {
    public:
    void receiveMessage(RenderAssetMessage);
    void receiveMessage(RenderMemoryMessage);
    void receiveMessage(RenderCameraMessage);
    void receiveMessage(RenderLightMessage);

    void render(void);
    private:
    ShaderProgramAsset* shader;
    std::string formatType;
    std::vector<QuickListElement>* list;
    std::map<unsigned int, MaterialAsset>* materials;
    std::map<unsigned int, ModelAsset>* models;
    std::map<unsigned int, std::vector<VboRenderUnitLocation>>* memMap;
    VboPair* vboPair;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec3 cameraPos;
    glm::vec3 pointLightPos;
    glm::vec3 globalAmbient;
    glm::vec3 lightAmbient;
    glm::vec3 lightSpecular;
    glm::vec3 lightDiffuse;
    float constantFalloff;
    float linearFalloff;
    float quadraticFalloff; 

    GLint cameraPosLoc;
    GLint mtwMatrixLoc;
    GLint transformationMatrixLoc;
    GLint pointLightPosLoc;
    GLint globalAmbientLoc;
    GLint lightAmbientLoc;
    GLint lightSpecularLoc;
    GLint lightDiffuseLoc;
    GLint lightConstantLoc;
    GLint lightLinearLoc;
    GLint lightQuadraticLoc; 
    GLint materialSpecularLoc; 
    GLint materialShininessLoc;
    GLint materialDiffuseLoc;
    GLint materialAmbientLoc;
    GLint materialEmissionLoc; 
    GLint usingTextureLoc;
  };

}

#endif
