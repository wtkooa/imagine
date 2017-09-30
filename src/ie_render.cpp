//___|"ie_render.cpp"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_render.h"

#include <iostream>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ie_assets.h"
#include "ie_const.h"
#include "ie_memory.h"

//___|RECEIVING MESSAGES|_______________________________________________________

//MESSAGES FOR STATIC RENDERER
void ie::StaticRender::receiveMessage(RenderAssetMessage msg)
{
  shader = msg.shaderProgram;
  list = msg.quickList;
  materials = msg.materials;
  models = msg.models;

  cameraPosLoc = (*shader).uniforms["cameraPos"].location;
  mtwMatrixLoc = (*shader).uniforms["mtwMatrix"].location;
  transformationMatrixLoc = (*shader).uniforms["transformationMatrix"].location;
  pointLightPosLoc = (*shader).uniforms["pointLightPos"].location;
  globalAmbientLoc = (*shader).uniforms["globalAmbient"].location;
  lightAmbientLoc = (*shader).uniforms["lightAmbient"].location;
  lightSpecularLoc = (*shader).uniforms["lightSpecular"].location;
  lightDiffuseLoc = (*shader).uniforms["lightDiffuse"].location;
  lightConstantLoc = (*shader).uniforms["lightConstantFalloff"].location;
  lightLinearLoc = (*shader).uniforms["lightLinearFalloff"].location;
  lightQuadraticLoc = (*shader).uniforms["lightQuadraticFalloff"].location;
  materialSpecularLoc = (*shader).uniforms["materialSpecular"].location;
  materialShininessLoc = (*shader).uniforms["materialShininess"].location;
  materialDiffuseLoc = (*shader).uniforms["materialDiffuse"].location;
  materialAmbientLoc = (*shader).uniforms["materialAmbient"].location;
  materialEmissionLoc = (*shader).uniforms["materialEmission"].location;
  usingTextureLoc = (*shader).uniforms["usingTexture"].location;
}


void ie::StaticRender::receiveMessage(RenderMemoryMessage msg)
{
  memMap = msg.memMap;
  vboPair = msg.vboPair;
  formatType = msg.formatType;
}


void ie::StaticRender::receiveMessage(RenderCameraMessage msg)
{
  projectionMatrix = msg.projectionMatrix;
  viewMatrix = msg.viewMatrix;
  cameraPos = msg.cameraPos;
}


void ie::StaticRender::receiveMessage(RenderLightMessage msg)
{
  pointLightPos = msg.posVector; 
  globalAmbient = msg.globalAmbient;
  lightAmbient = msg.lightAmbient;
  lightSpecular = msg.lightSpecular;
  lightDiffuse = msg.lightDiffuse;
  constantFalloff = msg.constantFalloff;
  linearFalloff = msg.linearFalloff;
  quadraticFalloff = msg.quadraticFalloff;
}

//______________________________________________________________________________

//___|RENDERERS|________________________________________________________________

//STATIC RENDERER
void ie::StaticRender::render(void)
{
  if ((*list).size() == 0) {return;}

  GLint currentBoundVbo;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBoundVbo);
  GLint requiredVbo = (*vboPair).readVbo;
  if (requiredVbo != currentBoundVbo)
  {
    glBindBuffer(GL_ARRAY_BUFFER, requiredVbo); 
  }

  GLint currentProg;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProg);
  GLuint requiredProg = (*shader).programId;
  if (requiredProg != currentProg)
  {
    glUseProgram(requiredProg);
  }
  
  if (formatType == "VTN")
  {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, ie::VEC3_DIM, GL_FLOAT, GL_FALSE,
                          ie::STRIDE_VTN_BYTES, ie::OFFSET_VOID);
    glVertexAttribPointer(1, ie::VEC2_DIM, GL_FLOAT, GL_FALSE,
                          ie::STRIDE_VTN_BYTES, ie::OFFSET_1VEC3);
    glVertexAttribPointer(2, ie::VEC3_DIM, GL_FLOAT, GL_FALSE,
                          ie::STRIDE_VTN_BYTES, ie::OFFSET_1VEC3_1VEC2);
  }
  else if (formatType == "VN")
  {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, ie::VEC3_DIM, GL_FLOAT, GL_FALSE,
                          ie::STRIDE_VN_BYTES, ie::OFFSET_VOID);
    glVertexAttribPointer(2, ie::VEC3_DIM, GL_FLOAT, GL_FALSE,
                          ie::STRIDE_VN_BYTES, ie::OFFSET_1VEC3);
  }

  glUniform3fv(cameraPosLoc, 1, &cameraPos[0]);
  glUniform3fv(pointLightPosLoc, 1, &pointLightPos[0]); 
  glUniform3fv(globalAmbientLoc, 1,  &globalAmbient[0]);
  glUniform3fv(lightAmbientLoc, 1, &lightAmbient[0]);
  glUniform3fv(lightSpecularLoc, 1, &lightSpecular[0]);
  glUniform3fv(lightDiffuseLoc, 1, &lightDiffuse[0]);
  glUniform1f(lightConstantLoc, constantFalloff);
  glUniform1f(lightLinearLoc, linearFalloff);
  glUniform1f(lightQuadraticLoc, quadraticFalloff);

  for (unsigned int nModel = 0; nModel < (*list).size(); nModel++)
  {
    ie::QuickListElement listElement = (*list)[nModel];
    unsigned int modelId = listElement.modelId;
    std::vector<short> renderUnitList = listElement.renderUnitList;
    ie::ModelAsset* modelAsset = &((*models)[modelId]); 
    glm::mat4 translationMatrix = (*modelAsset).translationMatrix; 
    glm::mat4 rotationMatrix = (*modelAsset).rotationMatrix;
    std::vector<RenderUnit>* renderUnits = &((*modelAsset).renderUnits); 

    glm::mat4 mtwMatrix = translationMatrix * rotationMatrix;
    glm::mat4 transformationMatrix = projectionMatrix *
                                     viewMatrix * mtwMatrix;
    glUniformMatrix4fv(mtwMatrixLoc, 1, GL_FALSE, &mtwMatrix[0][0]);
    glUniformMatrix4fv(transformationMatrixLoc, 1, GL_FALSE,
                       &transformationMatrix[0][0]);

    for (short nRu = 0; nRu < renderUnitList.size(); nRu++)
    {
      short renderUnitId = renderUnitList[nRu];
      unsigned int materialId = (*renderUnits)[renderUnitId].materialId;
      MaterialAsset* material = &((*materials)[materialId]);
      float materialShininess = (*material).shininess;
      glm::vec3 materialAmbient = (*material).ambient;
      glm::vec3 materialDiffuse = (*material).diffuse;
      glm::vec3 materialSpecular = (*material).specular;
      glm::vec3 materialEmission = (*material).emission;
      bool containsTexture = (*material).containsTexture;
      GLuint diffuseMapId = (*material).diffuseMapId;

      glUniform1f(materialShininessLoc, materialShininess);
      glUniform3fv(materialSpecularLoc, 1, &materialSpecular[0]);
      glUniform3fv(materialAmbientLoc, 1, &materialAmbient[0]);
      glUniform3fv(materialDiffuseLoc, 1, &materialDiffuse[0]);
      glUniform3fv(materialEmissionLoc, 1, &materialEmission[0]);

      std::vector<ie::VboRenderUnitLocation>* renderUnitLocations = &((*memMap)[modelId]);
      ie::VboRenderUnitLocation* renderUnitLocation = &((*renderUnitLocations)[renderUnitId]); 
      unsigned int location = (*renderUnitLocation).location;
      unsigned int indexAmount = (*renderUnitLocation).indexAmount;

      if (containsTexture)
      {
        glBindTexture(GL_TEXTURE_2D, diffuseMapId);
        glUniform1i(usingTextureLoc, 1);
      }
      else
      {
        glUniform1i(usingTextureLoc, 0);
      }

      glDrawArrays(GL_TRIANGLES, location, indexAmount);
      
      if (containsTexture) {glBindTexture(GL_TEXTURE_2D, 0);}
    }
  }
  if (formatType == "VTN")
  {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
  }
  else if (formatType == "VN")
  {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
  }
}

//______________________________________________________________________________
