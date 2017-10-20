//___|"ie_render_engine.cpp"|___________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_render_engine.h"

#include <iostream>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ie_assets.h"
#include "ie_const.h"
#include "ie_nodes.h"
#include "ie_scenegraph.h"
#include "ie_vram.h"

//___|RECEIVING MESSAGES|_______________________________________________________

void ie::RenderEngine::receiveMessage(AssetStatusMessage msg)
{
  materials = msg.materials;
  models = msg.models;
  rus = msg.rus;
  shaders = msg.shaders;
  shaderNameIdMap = msg.shaderNameIdMap;
  lights = msg.lights;
  lightNameIdMap = msg.lightNameIdMap;
  terrains = msg.terrains;
  terrainNameIdMap = msg.terrainNameIdMap;
  textures = msg.textures;
  textureNameIdMap = msg.textureNameIdMap;
}


void ie::RenderEngine::receiveMessage(VramStatusToRenderMessage msg)
{
  vPair = msg.vPair;
  vnPair = msg.vnPair;
  vtnPair = msg.vtnPair;
  vtncbPair = msg.vtncbPair;
  terrainIndexPair = msg.terrainIndexPair;
}


void ie::RenderEngine::receiveMessage(GraphStatusMessage msg)
{
  firstBucket = msg.bucket;
}

//______________________________________________________________________________

//___|RENDERERS|________________________________________________________________

void ie::RenderEngine::render(void)
{
  SortBucket* currentBucket = firstBucket;
  while (currentBucket != NULL)
  {
    RenderInstructions* instruct = currentBucket->getRenderInstructions();
    std::vector<RenderPointers>* rpsList = currentBucket->getRenderUnits();
    GLuint shaderId = (*shaderNameIdMap)[instruct->shader];
    ShaderAsset* shader = &(*shaders)[shaderId];
    GLint currentProg;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProg);
    if (shader->programId != currentProg)
    {
      glUseProgram(shader->programId);
    }

    if (instruct->renderer == "materialed")
    {
      renderMaterialedEntities(rpsList, shader);
    }
    else if (instruct->renderer == "textured")
    {
      renderTexturedEntities(rpsList, shader);
    }
    else if (instruct->renderer == "terrain")
    {
      renderTerrainEntities(rpsList, shader);
    }
    else if (instruct->data == "player")
    {
      //Player to data
    }
    else if (instruct->data == "camera")
    {
      for (auto it = rpsList->begin(); it != rpsList->end(); it++)
      {
        RenderPointers cameraPointers = (*it);
        updateCamera(cameraPointers.camera);
      }
    }
    currentBucket = currentBucket->getNextBucket();
  }
  firstBucket->clear();
}

void ie::RenderEngine::updateCamera(CameraNode* camera)
{
  projectionMatrix = camera->projectionMatrix;
  viewMatrix = camera->viewMatrix;
  cameraPos = camera->translation;
}

//STATIC MATERIALED ENTITIES RENDERER
void ie::RenderEngine::renderMaterialedEntities(std::vector<RenderPointers>* rpsList,
                                                ShaderAsset* shader)
{
  unsigned int lightId = (*lightNameIdMap)["light0"];
  LightAsset* light = &(*lights)[lightId];
  glm::vec3 pointLightPos = (*light).posVector; 
  glm::vec3 globalAmbient = (*light).globalAmbient;
  glm::vec3 lightAmbient = (*light).lightAmbient;
  glm::vec3 lightSpecular = (*light).lightSpecular;
  glm::vec3 lightDiffuse = (*light).lightDiffuse;
  float constantFalloff = (*light).constantFalloff;
  float linearFalloff = (*light).linearFalloff;
  float quadraticFalloff = (*light).quadraticFalloff;

  GLuint cameraPosLoc = (*shader).uniforms["cameraPos"].location;
  GLuint mtwMatrixLoc = (*shader).uniforms["mtwMatrix"].location;
  GLuint transformationMatrixLoc = (*shader).uniforms["transformationMatrix"].location;
  GLuint pointLightPosLoc = (*shader).uniforms["pointLightPos"].location;
  GLuint globalAmbientLoc = (*shader).uniforms["globalAmbient"].location;
  GLuint lightAmbientLoc = (*shader).uniforms["lightAmbient"].location;
  GLuint lightSpecularLoc = (*shader).uniforms["lightSpecular"].location;
  GLuint lightDiffuseLoc = (*shader).uniforms["lightDiffuse"].location;
  GLuint lightConstantLoc = (*shader).uniforms["lightConstantFalloff"].location;
  GLuint lightLinearLoc = (*shader).uniforms["lightLinearFalloff"].location;
  GLuint lightQuadraticLoc = (*shader).uniforms["lightQuadraticFalloff"].location;
  GLuint usesGlobalAmbientLoc = (*shader).uniforms["usesGlobalAmbient"].location;
  GLuint usesLightAmbientLoc = (*shader).uniforms["usesLightAmbient"].location;
  GLuint usesLightDiffuseLoc = (*shader).uniforms["usesLightDiffuse"].location;
  GLuint usesLightSpecularLoc = (*shader).uniforms["usesLightSpecular"].location;
  GLuint usesLightFalloffLoc = (*shader).uniforms["usesLightFalloff"].location;
  GLuint materialSpecularLoc = (*shader).uniforms["materialSpecular"].location;
  GLuint materialShininessLoc = (*shader).uniforms["materialShininess"].location;
  GLuint materialDiffuseLoc = (*shader).uniforms["materialDiffuse"].location;
  GLuint materialAmbientLoc = (*shader).uniforms["materialAmbient"].location;
  GLuint materialEmissionLoc = (*shader).uniforms["materialEmission"].location;
  GLuint usingTextureLoc = (*shader).uniforms["usingTexture"].location;

  GLint currentBoundVbo;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBoundVbo);
  GLint requiredVbo = (*vnPair).readVbo;
  if (requiredVbo != currentBoundVbo)
  {
    glBindBuffer(GL_ARRAY_BUFFER, requiredVbo); 
  }

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(0, ie::VEC3_DIM, GL_FLOAT, GL_FALSE,
                        ie::STRIDE_VN_BYTES, ie::OFFSET_VOID);
  glVertexAttribPointer(2, ie::VEC3_DIM, GL_FLOAT, GL_FALSE,
                        ie::STRIDE_VN_BYTES, ie::OFFSET_1VEC3);

  glUniform3fv(cameraPosLoc, 1, &cameraPos[0]);
  glUniform3fv(pointLightPosLoc, 1, &pointLightPos[0]); 
  glUniform3fv(globalAmbientLoc, 1,  &globalAmbient[0]);
  glUniform3fv(lightAmbientLoc, 1, &lightAmbient[0]);
  glUniform3fv(lightSpecularLoc, 1, &lightSpecular[0]);
  glUniform3fv(lightDiffuseLoc, 1, &lightDiffuse[0]);
  glUniform1f(lightConstantLoc, constantFalloff);
  glUniform1f(lightLinearLoc, linearFalloff);
  glUniform1f(lightQuadraticLoc, quadraticFalloff);

  for (auto it = rpsList->begin(); it != rpsList->end(); it++)
  {
    EntityNode* entity = it->entity;
    RenderUnit* ru = it->ru;
    glm::mat4 mtwMatrix = entity->mtwMatrix; 
    bool usesGlobalAmbientE = entity->usesGlobalAmbient;
    bool usesLightAmbientE = entity->usesLightAmbient;
    bool usesLightDiffuseE = entity->usesLightDiffuse;
    bool usesLightSpecularE = entity->usesLightSpecular;
    bool usesLightFalloffE = entity->usesLightFalloff;
    glm::mat4 transformationMatrix = projectionMatrix *
                                     viewMatrix * mtwMatrix;
    glUniformMatrix4fv(mtwMatrixLoc, 1, GL_FALSE, &mtwMatrix[0][0]);
    glUniformMatrix4fv(transformationMatrixLoc, 1, GL_FALSE,
                       &transformationMatrix[0][0]);

    unsigned int materialId = ru->material;
    unsigned int vramLocation = ru->vramLocation;
    unsigned int indexAmount = ru->vertexAmount;
    MaterialAsset* material = &((*materials)[materialId]);
    float materialShininess = (*material).shininess;
    glm::vec3 materialAmbient = (*material).ambient;
    glm::vec3 materialDiffuse = (*material).diffuse;
    glm::vec3 materialSpecular = (*material).specular;
    glm::vec3 materialEmission = (*material).emission;
    bool usesGlobalAmbientM = (*material).usesGlobalAmbient;
    bool usesLightAmbientM = (*material).usesLightAmbient;
    bool usesLightDiffuseM = (*material).usesLightDiffuse;
    bool usesLightSpecularM = (*material).usesLightSpecular;
    bool usesLightFalloffM = (*material).usesLightFalloff;
    bool containsTexture = (*material).containsTexture;
    GLuint diffuseMapId = (*material).diffuseMapId;

    bool usesGlobalAmbient = usesGlobalAmbientM && usesGlobalAmbientE;
    bool usesLightAmbient = usesLightAmbientM && usesLightAmbientE;
    bool usesLightDiffuse = usesLightDiffuseM && usesLightDiffuseE;
    bool usesLightSpecular = usesLightSpecularM && usesLightSpecularE;
    bool usesLightFalloff = usesLightFalloffM && usesLightFalloffE;

    glUniform1f(materialShininessLoc, materialShininess);
    glUniform3fv(materialSpecularLoc, 1, &materialSpecular[0]);
    glUniform3fv(materialAmbientLoc, 1, &materialAmbient[0]);
    glUniform3fv(materialDiffuseLoc, 1, &materialDiffuse[0]);
    glUniform3fv(materialEmissionLoc, 1, &materialEmission[0]);
    glUniform1i(usesGlobalAmbientLoc, usesGlobalAmbient);
    glUniform1i(usesLightAmbientLoc, usesLightAmbient);
    glUniform1i(usesLightDiffuseLoc, usesLightDiffuse);
    glUniform1i(usesLightSpecularLoc, usesLightSpecular);
    glUniform1i(usesLightFalloffLoc, usesLightFalloff);

    glDrawArrays(GL_TRIANGLES, vramLocation, indexAmount);
  }
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(2);
}


//STATIC TEXTURED ENTITIES RENDERER
void ie::RenderEngine::renderTexturedEntities(std::vector<RenderPointers>* rpsList,
                                              ShaderAsset* shader)
{
  unsigned int lightId = (*lightNameIdMap)["light0"];
  LightAsset* light = &(*lights)[lightId];
  glm::vec3 pointLightPos = (*light).posVector; 
  glm::vec3 globalAmbient = (*light).globalAmbient;
  glm::vec3 lightAmbient = (*light).lightAmbient;
  glm::vec3 lightSpecular = (*light).lightSpecular;
  glm::vec3 lightDiffuse = (*light).lightDiffuse;
  float constantFalloff = (*light).constantFalloff;
  float linearFalloff = (*light).linearFalloff;
  float quadraticFalloff = (*light).quadraticFalloff;

  GLuint cameraPosLoc = (*shader).uniforms["cameraPos"].location;
  GLuint mtwMatrixLoc = (*shader).uniforms["mtwMatrix"].location;
  GLuint transformationMatrixLoc = (*shader).uniforms["transformationMatrix"].location;
  GLuint pointLightPosLoc = (*shader).uniforms["pointLightPos"].location;
  GLuint globalAmbientLoc = (*shader).uniforms["globalAmbient"].location;
  GLuint lightAmbientLoc = (*shader).uniforms["lightAmbient"].location;
  GLuint lightSpecularLoc = (*shader).uniforms["lightSpecular"].location;
  GLuint lightDiffuseLoc = (*shader).uniforms["lightDiffuse"].location;
  GLuint lightConstantLoc = (*shader).uniforms["lightConstantFalloff"].location;
  GLuint lightLinearLoc = (*shader).uniforms["lightLinearFalloff"].location;
  GLuint lightQuadraticLoc = (*shader).uniforms["lightQuadraticFalloff"].location;
  GLuint usesGlobalAmbientLoc = (*shader).uniforms["usesGlobalAmbient"].location;
  GLuint usesLightAmbientLoc = (*shader).uniforms["usesLightAmbient"].location;
  GLuint usesLightDiffuseLoc = (*shader).uniforms["usesLightDiffuse"].location;
  GLuint usesLightSpecularLoc = (*shader).uniforms["usesLightSpecular"].location;
  GLuint usesLightFalloffLoc = (*shader).uniforms["usesLightFalloff"].location;
  GLuint materialSpecularLoc = (*shader).uniforms["materialSpecular"].location;
  GLuint materialShininessLoc = (*shader).uniforms["materialShininess"].location;
  GLuint materialDiffuseLoc = (*shader).uniforms["materialDiffuse"].location;
  GLuint materialAmbientLoc = (*shader).uniforms["materialAmbient"].location;
  GLuint materialEmissionLoc = (*shader).uniforms["materialEmission"].location;
  GLuint usingTextureLoc = (*shader).uniforms["usingTexture"].location;


  GLint currentBoundVbo;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBoundVbo);
  GLint requiredVbo = (*vtnPair).readVbo;
  if (requiredVbo != currentBoundVbo)
  {
    glBindBuffer(GL_ARRAY_BUFFER, requiredVbo); 
  }

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(0, ie::VEC3_DIM, GL_FLOAT, GL_FALSE,
                        ie::STRIDE_VTN_BYTES, ie::OFFSET_VOID);
  glVertexAttribPointer(1, ie::VEC2_DIM, GL_FLOAT, GL_FALSE,
                        ie::STRIDE_VTN_BYTES, ie::OFFSET_1VEC3);
  glVertexAttribPointer(2, ie::VEC3_DIM, GL_FLOAT, GL_FALSE,
                        ie::STRIDE_VTN_BYTES, ie::OFFSET_1VEC3_1VEC2);

  glUniform3fv(cameraPosLoc, 1, &cameraPos[0]);
  glUniform3fv(pointLightPosLoc, 1, &pointLightPos[0]); 
  glUniform3fv(globalAmbientLoc, 1,  &globalAmbient[0]);
  glUniform3fv(lightAmbientLoc, 1, &lightAmbient[0]);
  glUniform3fv(lightSpecularLoc, 1, &lightSpecular[0]);
  glUniform3fv(lightDiffuseLoc, 1, &lightDiffuse[0]);
  glUniform1f(lightConstantLoc, constantFalloff);
  glUniform1f(lightLinearLoc, linearFalloff);
  glUniform1f(lightQuadraticLoc, quadraticFalloff);

  for (auto it = rpsList->begin(); it != rpsList->end(); it++)
  {
    EntityNode* entity = it->entity;
    RenderUnit* ru = it->ru;
    glm::mat4 mtwMatrix = entity->mtwMatrix; 
    bool usesGlobalAmbientE = entity->usesGlobalAmbient;
    bool usesLightAmbientE = entity->usesLightAmbient;
    bool usesLightDiffuseE = entity->usesLightDiffuse;
    bool usesLightSpecularE = entity->usesLightSpecular;
    bool usesLightFalloffE = entity->usesLightFalloff;

    glm::mat4 transformationMatrix = projectionMatrix *
                                     viewMatrix * mtwMatrix;
    glUniformMatrix4fv(mtwMatrixLoc, 1, GL_FALSE, &mtwMatrix[0][0]);
    glUniformMatrix4fv(transformationMatrixLoc, 1, GL_FALSE,
                       &transformationMatrix[0][0]);

    unsigned int materialId = ru->material;
    unsigned int vramLocation = ru->vramLocation;
    unsigned int indexAmount = ru->vertexAmount;
    MaterialAsset* material = &((*materials)[materialId]);
    float materialShininess = (*material).shininess;
    glm::vec3 materialAmbient = (*material).ambient;
    glm::vec3 materialDiffuse = (*material).diffuse;
    glm::vec3 materialSpecular = (*material).specular;
    glm::vec3 materialEmission = (*material).emission;
    bool usesGlobalAmbientM = (*material).usesGlobalAmbient;
    bool usesLightAmbientM = (*material).usesLightAmbient;
    bool usesLightDiffuseM = (*material).usesLightDiffuse;
    bool usesLightSpecularM = (*material).usesLightSpecular;
    bool usesLightFalloffM = (*material).usesLightFalloff;
    bool containsTexture = (*material).containsTexture;
    unsigned int textureAssetId = (*material).diffuseMapId;
    GLuint diffuseMapId = (*textures)[textureAssetId].textureId;

    bool usesGlobalAmbient = usesGlobalAmbientM && usesGlobalAmbientE;
    bool usesLightAmbient = usesLightAmbientM && usesLightAmbientE;
    bool usesLightDiffuse = usesLightDiffuseM && usesLightDiffuseE;
    bool usesLightSpecular = usesLightSpecularM && usesLightSpecularE;
    bool usesLightFalloff = usesLightFalloffM && usesLightFalloffE;

    glUniform1f(materialShininessLoc, materialShininess);
    glUniform3fv(materialSpecularLoc, 1, &materialSpecular[0]);
    glUniform3fv(materialAmbientLoc, 1, &materialAmbient[0]);
    glUniform3fv(materialDiffuseLoc, 1, &materialDiffuse[0]);
    glUniform3fv(materialEmissionLoc, 1, &materialEmission[0]);
    glUniform1i(usesGlobalAmbientLoc, usesGlobalAmbient);
    glUniform1i(usesLightAmbientLoc, usesLightAmbient);
    glUniform1i(usesLightDiffuseLoc, usesLightDiffuse);
    glUniform1i(usesLightSpecularLoc, usesLightSpecular);
    glUniform1i(usesLightFalloffLoc, usesLightFalloff);

    if (containsTexture)
    {
      glBindTexture(GL_TEXTURE_2D, diffuseMapId);
      glUniform1i(usingTextureLoc, 1);
    }
    else
    {
      glUniform1i(usingTextureLoc, 0);
    }

    glDrawArrays(GL_TRIANGLES, vramLocation, indexAmount);
    
    if (containsTexture)
    {
      glBindTexture(GL_TEXTURE_2D, 0);
      glUniform1i(usingTextureLoc, 0);
    }
  }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}


//TERRAIN ENTITIES RENDERER
void ie::RenderEngine::renderTerrainEntities(std::vector<RenderPointers>* rpsList,
                                             ShaderAsset* shader)
{
  unsigned int lightId = (*lightNameIdMap)["light0"];
  LightAsset* light = &(*lights)[lightId];
  glm::vec3 pointLightPos = (*light).posVector; 
  glm::vec3 globalAmbient = (*light).globalAmbient;
  glm::vec3 lightAmbient = (*light).lightAmbient;
  glm::vec3 lightSpecular = (*light).lightSpecular;
  glm::vec3 lightDiffuse = (*light).lightDiffuse;
  float constantFalloff = (*light).constantFalloff;
  float linearFalloff = (*light).linearFalloff;
  float quadraticFalloff = (*light).quadraticFalloff;

  GLuint cameraPosLoc = (*shader).uniforms["cameraPos"].location;
  GLuint mtwMatrixLoc = (*shader).uniforms["mtwMatrix"].location;
  GLuint transformationMatrixLoc = (*shader).uniforms["transformationMatrix"].location;
  GLuint pointLightPosLoc = (*shader).uniforms["pointLightPos"].location;
  GLuint globalAmbientLoc = (*shader).uniforms["globalAmbient"].location;
  GLuint lightAmbientLoc = (*shader).uniforms["lightAmbient"].location;
  GLuint lightSpecularLoc = (*shader).uniforms["lightSpecular"].location;
  GLuint lightDiffuseLoc = (*shader).uniforms["lightDiffuse"].location;
  GLuint lightConstantLoc = (*shader).uniforms["lightConstantFalloff"].location;
  GLuint lightLinearLoc = (*shader).uniforms["lightLinearFalloff"].location;
  GLuint lightQuadraticLoc = (*shader).uniforms["lightQuadraticFalloff"].location;
  GLuint usesGlobalAmbientLoc = (*shader).uniforms["usesGlobalAmbient"].location;
  GLuint usesLightAmbientLoc = (*shader).uniforms["usesLightAmbient"].location;
  GLuint usesLightDiffuseLoc = (*shader).uniforms["usesLightDiffuse"].location;
  GLuint usesLightSpecularLoc = (*shader).uniforms["usesLightSpecular"].location;
  GLuint usesLightFalloffLoc = (*shader).uniforms["usesLightFalloff"].location;
  GLuint materialSpecularLoc = (*shader).uniforms["materialSpecular"].location;
  GLuint materialShininessLoc = (*shader).uniforms["materialShininess"].location;
  GLuint materialDiffuseLoc = (*shader).uniforms["materialDiffuse"].location;
  GLuint materialAmbientLoc = (*shader).uniforms["materialAmbient"].location;
  GLuint materialEmissionLoc = (*shader).uniforms["materialEmission"].location;
  GLuint texture1Loc = (*shader).uniforms["texture1"].location;
  GLuint texture2Loc = (*shader).uniforms["texture2"].location;
  GLuint texture3Loc = (*shader).uniforms["texture3"].location;
  GLuint texture4Loc = (*shader).uniforms["texture4"].location;
  GLuint texture5Loc = (*shader).uniforms["texture5"].location;
  GLuint texture6Loc = (*shader).uniforms["texture6"].location;
  GLuint texture7Loc = (*shader).uniforms["texture7"].location;
  GLuint texture8Loc = (*shader).uniforms["texture8"].location;
  GLuint textureAmountLoc = (*shader).uniforms["textureAmount"].location;

  GLint currentBoundArrayBuffer;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBoundArrayBuffer);
  GLint requiredArray = (*vtncbPair).readVbo;
  if (requiredArray != currentBoundArrayBuffer)
  {
    glBindBuffer(GL_ARRAY_BUFFER, requiredArray); 
  }

  GLint currentBoundElementBuffer;
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER, &currentBoundElementBuffer);
  GLint requiredIndex = (*terrainIndexPair).readVbo;
  if (requiredIndex != currentBoundElementBuffer)
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, requiredIndex); 
  }

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);


  glVertexAttribPointer(0, ie::VEC3_DIM, GL_FLOAT, GL_FALSE,
                        ie::STRIDE_VTNCB_BYTES, ie::OFFSET_VOID);
  glVertexAttribPointer(1, ie::VEC2_DIM, GL_FLOAT, GL_FALSE,
                        ie::STRIDE_VTNCB_BYTES, ie::OFFSET_1VEC3);
  glVertexAttribPointer(2, ie::VEC3_DIM, GL_FLOAT, GL_FALSE,
                        ie::STRIDE_VTNCB_BYTES, ie::OFFSET_1VEC3_1VEC2);
  glVertexAttribPointer(3, ie::VEC3_DIM, GL_FLOAT, GL_FALSE,
                        ie::STRIDE_VTNCB_BYTES, ie::OFFSET_2VEC3_1VEC2);
  glVertexAttribPointer(4, ie::UVEC2_DIM, GL_UNSIGNED_INT, GL_FALSE,
                        ie::STRIDE_VTNCB_BYTES, ie::OFFSET_3VEC3_1VEC2);

  glUniform3fv(cameraPosLoc, 1, &cameraPos[0]);
  glUniform3fv(pointLightPosLoc, 1, &pointLightPos[0]); 
  glUniform3fv(globalAmbientLoc, 1,  &globalAmbient[0]);
  glUniform3fv(lightAmbientLoc, 1, &lightAmbient[0]);
  glUniform3fv(lightSpecularLoc, 1, &lightSpecular[0]);
  glUniform3fv(lightDiffuseLoc, 1, &lightDiffuse[0]);
  glUniform1f(lightConstantLoc, constantFalloff);
  glUniform1f(lightLinearLoc, linearFalloff);
  glUniform1f(lightQuadraticLoc, quadraticFalloff);
  glUniform1i(texture1Loc, 0);
  glUniform1i(texture2Loc, 1);
  glUniform1i(texture3Loc, 2);
  glUniform1i(texture4Loc, 3);
  glUniform1i(texture5Loc, 4);
  glUniform1i(texture6Loc, 5);
  glUniform1i(texture7Loc, 6);
  glUniform1i(texture8Loc, 7);

  for (auto it = rpsList->begin(); it != rpsList->end(); it++)
  {
    EntityNode* entity = it->entity;
    TerrainAsset* terrain = it->ta;
    glm::mat4 mtwMatrix = entity->mtwMatrix; 
    bool usesGlobalAmbientE = entity->usesGlobalAmbient;
    bool usesLightAmbientE = entity->usesLightAmbient;
    bool usesLightDiffuseE = entity->usesLightDiffuse;
    bool usesLightSpecularE = entity->usesLightSpecular;
    bool usesLightFalloffE = entity->usesLightFalloff;

    glm::mat4 transformationMatrix = projectionMatrix *
                                     viewMatrix * mtwMatrix;
    glUniformMatrix4fv(mtwMatrixLoc, 1, GL_FALSE, &mtwMatrix[0][0]);
    glUniformMatrix4fv(transformationMatrixLoc, 1, GL_FALSE,
                       &transformationMatrix[0][0]);
    
    float materialShininess = (*terrain).shininess;
    glm::vec3 materialSpecular = (*terrain).specular;
    glm::vec3 materialAmbient = (*terrain).ambient;
    glm::vec3 materialDiffuse = (*terrain).diffuse;
    glm::vec3 materialEmission = (*terrain).emission;
    short textureAmount = (*terrain).textureIds.size();
    unsigned int vramIndexLocation = (*terrain).vramIndexLocation;
    unsigned int vramIndexAmount = (*terrain).vramIndexAmount;
    void* p_vramLocation = (void*)(long(vramIndexLocation));

    glUniform1f(materialShininessLoc, materialShininess);
    glUniform3fv(materialSpecularLoc, 1, &materialSpecular[0]);
    glUniform3fv(materialAmbientLoc, 1, &materialAmbient[0]);
    glUniform3fv(materialDiffuseLoc, 1, &materialDiffuse[0]);
    glUniform3fv(materialEmissionLoc, 1, &materialEmission[0]);
    glUniform1i(usesGlobalAmbientLoc, usesGlobalAmbientE);
    glUniform1i(usesLightAmbientLoc, usesLightAmbientE);
    glUniform1i(usesLightDiffuseLoc, usesLightDiffuseE);
    glUniform1i(usesLightSpecularLoc, usesLightSpecularE);
    glUniform1i(usesLightFalloffLoc, usesLightFalloffE);

    glUniform1i(textureAmountLoc, textureAmount);
    for (short tex = 0; tex < textureAmount; tex++)
    {
      glActiveTexture(GL_TEXTURE0 + tex);
      unsigned int textureAssetId = (*terrain).textureIds[tex];
      GLuint diffuseMapId = (*textures)[textureAssetId].textureId;
      glBindTexture(GL_TEXTURE_2D, diffuseMapId);
    }
    glDrawElements(GL_TRIANGLES,
                   vramIndexAmount,
                   GL_UNSIGNED_INT,
                   p_vramLocation);

    for (short tex = textureAmount - 1; tex >= 0; tex--)
    {
      glActiveTexture(GL_TEXTURE0 + tex);
      glBindTexture(GL_TEXTURE_2D, 0);
    }

  }
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(4);
}

//______________________________________________________________________________
