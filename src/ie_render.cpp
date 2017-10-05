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

void ie::RenderManager::receiveMessage(AssetStatusToRenderMessage msg)
{
  entities = msg.entities;
  materials = msg.materials;
  models = msg.models;
  shaders = msg.shaders;
  shaderNameIdMap = msg.shaderNameIdMap;
  lights = msg.lights;
  lightNameIdMap = msg.lightNameIdMap;
  terrains = msg.terrains;
  terrainNameIdMap = msg.terrainNameIdMap;
  staticVList = msg.staticVList;
  staticVNList = msg.staticVNList;
  staticVTNList = msg.staticVTNList;
  terrainVTNCBList = msg.terrainVTNCBList;
  
}


void ie::RenderManager::receiveMessage(VramStatusToRenderMessage msg)
{
  staticMemoryMap = msg.staticMemoryMap;
  terrainIndexMemoryMap = msg.terrainIndexMemoryMap;
  vPair = msg.vPair;
  vnPair = msg.vnPair;
  vtnPair = msg.vtnPair;
  vtncbPair = msg.vtncbPair;
  terrainIndexPair = msg.terrainIndexPair;
}


void ie::RenderManager::receiveMessage(CameraStatusToRenderMessage msg)
{
  projectionMatrix = msg.projectionMatrix;
  viewMatrix = msg.viewMatrix;
  cameraPos = msg.cameraPos;
}

//______________________________________________________________________________

//___|RENDERERS|________________________________________________________________

void ie::RenderManager::render(void)
{
  renderMaterialedEntities();
  renderTexturedEntities();
  renderTerrainEntities();
}

//STATIC MATERIALED ENTITIES RENDERER
void ie::RenderManager::renderMaterialedEntities(void)
{
  if ((*staticVNList).size() == 0) {return;}

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

  GLuint shaderId = (*shaderNameIdMap)["static"];
  ShaderProgramAsset* shader = &(*shaders)[shaderId];
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

  GLint currentProg;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProg);
  GLuint requiredProg = (*shader).programId;
  if (requiredProg != currentProg)
  {
    glUseProgram(requiredProg);
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

  for (unsigned int nEntity = 0; nEntity < (*staticVNList).size(); nEntity++)
  {
    ie::StaticQuickListElement listElement = (*staticVNList)[nEntity];
    unsigned int entityId = listElement.entityId;
    std::vector<short> renderUnitList = listElement.renderUnitList;
    ie::Entity* entity = &((*entities)[entityId]); 
    glm::mat4 translationMatrix = (*entity).translationMatrix; 
    glm::mat4 rotationMatrix = (*entity).rotationMatrix;
    glm::mat4 scaleMatrix = (*entity).scaleMatrix;
    ie::ModelAsset* modelAsset = &((*models)[(*entity).modelId]);
    unsigned int modelId = (*modelAsset).modelId;
    std::vector<RenderUnit>* renderUnits = &((*modelAsset).renderUnits); 

    glm::mat4 mtwMatrix = translationMatrix * rotationMatrix *
                          scaleMatrix;
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

      std::vector<ie::StaticRenderUnitLocation>* renderUnitLocations = &((*staticMemoryMap)[modelId]);
      ie::StaticRenderUnitLocation* renderUnitLocation = &((*renderUnitLocations)[renderUnitId]); 
      unsigned int location = (*renderUnitLocation).location;
      unsigned int indexAmount = (*renderUnitLocation).indexAmount;

      glDrawArrays(GL_TRIANGLES, location, indexAmount);
    }
  }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(2);
}


//STATIC TEXTURED ENTITIES RENDERER
void ie::RenderManager::renderTexturedEntities(void)
{
  if ((*staticVTNList).size() == 0) {return;}

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

  GLuint shaderId = (*shaderNameIdMap)["static"];
  ShaderProgramAsset* shader = &(*shaders)[shaderId];
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

  GLint currentProg;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProg);
  GLuint requiredProg = (*shader).programId;
  if (requiredProg != currentProg)
  {
    glUseProgram(requiredProg);
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

  for (unsigned int nEntity = 0; nEntity < (*staticVTNList).size(); nEntity++)
  {
    ie::StaticQuickListElement listElement = (*staticVTNList)[nEntity];
    unsigned int entityId = listElement.entityId;
    std::vector<short> renderUnitList = listElement.renderUnitList;
    ie::Entity* entity = &((*entities)[entityId]); 
    glm::mat4 translationMatrix = (*entity).translationMatrix; 
    glm::mat4 rotationMatrix = (*entity).rotationMatrix;
    glm::mat4 scaleMatrix = (*entity).scaleMatrix;
    ie::ModelAsset* modelAsset = &((*models)[(*entity).modelId]);
    unsigned int modelId = (*modelAsset).modelId;
    std::vector<RenderUnit>* renderUnits = &((*modelAsset).renderUnits); 

    glm::mat4 mtwMatrix = translationMatrix * rotationMatrix *
                          scaleMatrix;
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

      std::vector<ie::StaticRenderUnitLocation>* renderUnitLocations = &((*staticMemoryMap)[modelId]);
      ie::StaticRenderUnitLocation* renderUnitLocation = &((*renderUnitLocations)[renderUnitId]); 
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
      
      if (containsTexture)
      {
        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(usingTextureLoc, 0);
      }
    }
  }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}


//TERRAIN ENTITIES RENDERER
void ie::RenderManager::renderTerrainEntities(void)
{

  if ((*terrainVTNCBList).size() == 0) {return;}    

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

  GLuint shaderId = (*shaderNameIdMap)["terrain"];
  ShaderProgramAsset* shader = &(*shaders)[shaderId];
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
  GLuint materialSpecularLoc = (*shader).uniforms["materialSpecular"].location;
  GLuint materialShininessLoc = (*shader).uniforms["materialShininess"].location;
  GLuint materialDiffuseLoc = (*shader).uniforms["materialDiffuse"].location;
  GLuint materialAmbientLoc = (*shader).uniforms["materialAmbient"].location;
  GLuint materialEmissionLoc = (*shader).uniforms["materialEmission"].location;
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

  GLint currentProg;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProg);
  GLuint requiredProg = (*shader).programId;
  if (requiredProg != currentProg)
  {
    glUseProgram(requiredProg);
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

  for (unsigned int nEntity = 0; nEntity < (*terrainVTNCBList).size(); nEntity++)
  {
    unsigned int entityId = (*terrainVTNCBList)[nEntity];
    Entity* entity = &(*entities)[entityId];
    unsigned int terrainId = (*entity).terrainId;
    TerrainAsset* terrain = &(*terrains)[terrainId]; 
    glm::mat4 translationMatrix = (*entity).translationMatrix;
    glm::mat4 rotationMatrix = (*entity).rotationMatrix;
    glm::mat4 scaleMatrix = (*entity).scaleMatrix;

    glm::mat4 mtwMatrix = translationMatrix * rotationMatrix *
                          scaleMatrix;
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
    int texture1Id = (*terrain).textureIds[0];

    glUniform1f(materialShininessLoc, materialShininess);
    glUniform3fv(materialSpecularLoc, 1, &materialSpecular[0]);
    glUniform3fv(materialAmbientLoc, 1, &materialAmbient[0]);
    glUniform3fv(materialDiffuseLoc, 1, &materialDiffuse[0]);
    glUniform3fv(materialEmissionLoc, 1, &materialEmission[0]);

    ie::TerrainRenderUnitLocation* loc = &(*terrainIndexMemoryMap)[terrainId];
    unsigned int location = (*loc).location;
    location = location * sizeof(unsigned int);
    void* p_location = (void*)(long(location));
    unsigned int indexAmount = (*loc).indexAmount;

    glBindTexture(GL_TEXTURE_2D, 2);

    glDrawElements(GL_TRIANGLES, indexAmount, GL_UNSIGNED_INT, p_location);

    glBindTexture(GL_TEXTURE_2D, 0);

  }
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(4);
}

//______________________________________________________________________________
