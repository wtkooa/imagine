//___|"ie_wavefront.cpp"|_______________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_wavefront.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "ie_asset_manager.h"
#include "ie_communication.h"
#include "ie_config.h"
#include "ie_enum.h"
#include "ie_log.h"
#include "ie_material.h"
#include "ie_mesh.h"
#include "ie_messages.h"
#include "ie_texture.h"
#include "ie_utils.h"

ie::WavefrontLoader::WavefrontLoader()
{
  reset();
}


void ie::WavefrontLoader::reset(void)
{
  manager = NULL;
  containsQuads = false;
  workingMesh = NULL;
  workingRenderUnit = NULL;
  bufferRenderUnit = NULL;
  workingMaterial = NULL;
  workingTexture = NULL;
}


void ie::WavefrontLoader::init(void)
{
  nexus->addConnection(IE_WAVEFRONT_ICOM_HANDLE, this);
  NexusMsg msg(IE_LOG_ICOM_HANDLE, IE_NEXUS_OUT_CONNECTION_CMD, this);
  nexus->rxMsg(&msg); 
  msg.target = IE_ASSET_MANAGER_ICOM_HANDLE;
  nexus->rxMsg(&msg); 
}


void ie::WavefrontLoader::setLoadDestination(AssetManager* m)
{
  manager = m;
}


void ie::WavefrontLoader::load(std::string filename)
{
  load(ie::DEFAULT_WAVEFRONT_PATH, filename);  
}

void ie::WavefrontLoader::load(std::string filepath, std::string filename)
{
  std::vector<std::string> tokens = split(filename, ".");  
  std::string extention = tokens.back();

  if (extention == "mtl") {loadMtl(filepath, filename);}
  else if (extention == "obj") {loadObj(filepath, filename);}
  else
  {
    //log->warning("Unrecognized Wavefront file extention '%s'", extention.c_str());
  }
}

void ie::WavefrontLoader::loadObj(std::string filepath, std::string filename)
{
  std::ifstream objFile;
  objFile.open((filepath + filename).c_str());
  bool failedToOpen = !objFile.is_open();

  if (failedToOpen)
  {
    //log->warning("Wavefront obj file '%s' failed to open for reading.",
    //             filename.c_str());
  }
  else
  {
    std::string line;

    while(getline(objFile, line))
    {
      std::vector<std::string> tokens = split(line, " ");
      std::string command = tokens[0];
      tokens.erase(tokens.begin());
      
      bool isUnusedCommand = (command == "s"); 

      if (isUnusedCommand) {continue;}
      else if (command == "") {continue;} //Blank Line
      else if (command == "#") {continue;} //Comment
      else if (command == "mtllib") {mtllib(filepath, tokens);}
      else if (command == "o") {object(tokens, filepath, filename);}
      else if (command == "v") {position(tokens);}
      else if (command == "vt") {mapping(tokens);}
      else if (command == "vn") {normal(tokens);}
      else if (command == "usemtl") {usemtl(tokens);}
      else if (command == "f") {face(tokens);}
      else
      {
        //log->warning("Unrecognized wavefront obj command '%s'",
        //             command.c_str());
      }
    }
    loadObject();
    objFile.close();
  }
}


void ie::WavefrontLoader::mtllib(std::string filepath, 
                                       std::vector<std::string> tokens)
{
  for (auto file = tokens.begin(); file != tokens.end(); file++)
  {
    loadMtl(filepath, *file);
  }
}


void ie::WavefrontLoader::loadObject(void)
{
  if (workingMesh != NULL)
  {
    if (containsQuads)
    {
      //log->warning("Object '%s' contains quad faces",
      //             workingMesh->getName().c_str()); 
    }
    loadRenderUnit();
    workingMesh->removeMeshDuplicates(ie::DEFAULT_OBJ_SENSITIVITY);
    manager->load(workingMesh);
    workingMesh = NULL;
    if (workingRenderUnit != NULL) {workingRenderUnit = NULL;}
    if (bufferRenderUnit != NULL)
    {
      delete bufferRenderUnit;
      bufferRenderUnit = NULL;
    }
    containsQuads = false;
  }
}


void ie::WavefrontLoader::loadRenderUnit(void)
{
  if (workingRenderUnit != NULL)
  {
    procRenderUnitBuffer(); 
    workingMesh->addRenderUnit(workingRenderUnit);
  }
}


void ie::WavefrontLoader::procRenderUnitBuffer(void)
{
  unsigned int indexAmount = bufferRenderUnit->getElementArrayAmount();
  for (unsigned int n = 0; n < indexAmount;)
  {
    unsigned int vertexAmount = bufferRenderUnit->getIndexAttrib(n);
    n++;
    for (unsigned int vertex = 0; vertex < vertexAmount; vertex++)
    {
      unsigned int vertexFormat = bufferRenderUnit->getIndexAttrib(n);
      n++;
      for (unsigned int element = 0; element < vertexFormat; element++)
      {
        unsigned int attrib = bufferRenderUnit->getIndexAttrib(n);
        n++;
        if (attrib == 1)
        {
          unsigned int positionIndex = bufferRenderUnit->getIndexAttrib(n);
          glm::vec3 position = bufferRenderUnit->getPositionAttrib(positionIndex - 1); 
          workingRenderUnit->addPositionAttrib(position);
        }
        else if (attrib == 2)
        {
          unsigned int mapIndex = bufferRenderUnit->getIndexAttrib(n);
          glm::vec2 map = bufferRenderUnit->getMapAttrib(mapIndex - 1); 
          workingRenderUnit->addMapAttrib(map);
        }
        else if (attrib == 3)
        {
          unsigned int normalIndex = bufferRenderUnit->getIndexAttrib(n);
          unsigned int normal = bufferRenderUnit->getPackedNormalAttrib(normalIndex - 1); 
          workingRenderUnit->addPackedNormalAttrib(normal);
        }
        n++;
      }
      workingRenderUnit->addIndexAttrib(workingRenderUnit->getVertexArrayAmount() - 1);
    }
  }
  bufferRenderUnit->clearAttrib(IE_ELEMENT_ATTRIB);
}


void ie::WavefrontLoader::object(std::vector<std::string> tokens,
                                       std::string filepath,
                                       std::string filename)
{
  loadObject();
  workingMesh = new Mesh();
  bufferRenderUnit = new RenderUnit();
  workingMesh->setName(tokens[0]);
  workingMesh->setFilepath(filepath);
  workingMesh->setFilename(filename);
}


void ie::WavefrontLoader::position(std::vector<std::string> tokens)
{
  bufferRenderUnit->addPositionAttrib(glm::vec3(stof(tokens[0]),
                                                stof(tokens[1]),
                                                stof(tokens[2])));
}


void ie::WavefrontLoader::mapping(std::vector<std::string> tokens)
{
  bufferRenderUnit->addMapAttrib(glm::vec2(stof(tokens[0]),
                                           stof(tokens[1])));
}


void ie::WavefrontLoader::normal(std::vector<std::string> tokens)
{
  bufferRenderUnit->addUnpackedNormalAttrib(glm::vec3(stof(tokens[0]),
                                              stof(tokens[1]),
                                              stof(tokens[2])));
}


void ie::WavefrontLoader::usemtl(std::vector<std::string> tokens)
{
  loadRenderUnit();
  workingRenderUnit = new RenderUnit(); 
  workingRenderUnit->setMaterial(manager->getMaterial(tokens[0]));
}


void ie::WavefrontLoader::face(std::vector<std::string> tokens)
{
  unsigned int tokenAmount = tokens.size();
  if (tokenAmount == 4)
  {
    containsQuads = true;
    return;
  }
  bufferRenderUnit->addIndexAttrib(tokenAmount);

  for (auto it = tokens.begin(); it != tokens.end(); it++)
  {
    std::vector<std::string> subTokens = split(*it, "/");
    unsigned int subTokenAmount = subTokens.size();
    bufferRenderUnit->addIndexAttrib(eraseTokens(subTokens, "").size());

    if (subTokenAmount == 1)
    {
      bufferRenderUnit->addIndexAttrib(1);
      bufferRenderUnit->addIndexAttrib(stoi(subTokens[0]));
    }
    else if (subTokenAmount == 2)
    {
      if (subTokens[0] != "")
      {
        bufferRenderUnit->addIndexAttrib(1);
        bufferRenderUnit->addIndexAttrib(stoi(subTokens[0]));
      }
      if (subTokens[1] != "")
      {
        bufferRenderUnit->addIndexAttrib(2);
        bufferRenderUnit->addIndexAttrib(stoi(subTokens[1]));
      }
    }
    else if (subTokenAmount == 3)
    {
      if (subTokens[0] != "")
      {
        bufferRenderUnit->addIndexAttrib(1);
        bufferRenderUnit->addIndexAttrib(stoi(subTokens[0]));
      }
      if (subTokens[1] != "")
      {
        bufferRenderUnit->addIndexAttrib(2);
        bufferRenderUnit->addIndexAttrib(stoi(subTokens[1]));
      }
      if (subTokens[2] != "")
      {
        bufferRenderUnit->addIndexAttrib(3);
        bufferRenderUnit->addIndexAttrib(stoi(subTokens[2]));
      }
    }
  }
}


void ie::WavefrontLoader::loadMtl(std::string filepath, std::string filename)
{
  std::ifstream mtlFile;
  mtlFile.open((filepath + filename).c_str());
  bool failedToOpen = !mtlFile.is_open();

  if (failedToOpen)
  {
    //log->warning("Wavefront mlt file '%s' failed to open for reading",
    //             filename.c_str());
  }
  else
  {
    std::string line;

    while(getline(mtlFile, line))
    {
      std::vector<std::string> tokens = split(line, " ");
      std::string command = tokens[0];
      tokens.erase(tokens.begin());

      bool isUnusedCommand = (command == "Ni") ||
                             (command == "d") ||
                             (command == "illum");

      if (isUnusedCommand) {continue;}
      else if (command == "") {continue;} //Blank Line
      else if (command == "#") {continue;} //Comment
      else if (command == "newmtl") {newmtl(tokens, filepath, filename);}
      else if (command == "Ns") {shininess(tokens);}
      else if (command == "Ka") {ambient(tokens);}
      else if (command == "Kd") {diffuse(tokens);}
      else if (command == "Ks") {specular(tokens);}
      else if (command == "Ke") {emission(tokens);}
      else if (command == "map_Kd") {map_Kd(tokens);}
      else if (command == "map_bump") {map_bump(tokens);}
      else
      {
        //log->warning("Unrecognized wavefront mtl command '%s'",
        //             command.c_str());
      }
    }
    loadMaterial();
  }
}


void ie::WavefrontLoader::loadMaterial(void)
{
  if (workingMaterial != NULL)
  {
    manager->load(workingMaterial);
    workingMaterial = NULL;
  }
}


void ie::WavefrontLoader::newmtl(std::vector<std::string> tokens,
                                       std::string filepath,
                                       std::string filename)
{
  loadMaterial();
  workingMaterial = new Material();   
  workingMaterial->setName(tokens[0]);
  workingMaterial->setFilepath(filepath);
  workingMaterial->setFilename(filename);
}


void ie::WavefrontLoader::shininess(std::vector<std::string> tokens)
{
  workingMaterial->setShininess(stof(tokens[0]));
}


void ie::WavefrontLoader::ambient(std::vector<std::string> tokens)
{
  workingMaterial->setAmbient(glm::vec3(stof(tokens[0]),
                                        stof(tokens[1]),
                                        stof(tokens[2])));
}


void ie::WavefrontLoader::diffuse(std::vector<std::string> tokens)
{
  workingMaterial->setDiffuse(glm::vec3(stof(tokens[0]),
                                        stof(tokens[1]),
                                        stof(tokens[2])));
}


void ie::WavefrontLoader::specular(std::vector<std::string> tokens)
{
  workingMaterial->setSpecular(glm::vec3(stof(tokens[0]),
                                         stof(tokens[1]),
                                         stof(tokens[2])));
}


void ie::WavefrontLoader::emission(std::vector<std::string> tokens)
{
  workingMaterial->setEmission(glm::vec3(stof(tokens[0]),
                                         stof(tokens[1]),
                                         stof(tokens[2])));
}


ie::Texture* ie::WavefrontLoader::loadTexture(std::vector<std::string> tokens)
{
  tokens = split(tokens[0], "/");
  std::string filename = tokens.back();
  std::string name = split(filename, ".", 0);
  tokens.pop_back();

  std::string filepath = concatenateTokens(tokens);

  workingTexture = new Texture;
  workingTexture->setName(name);
  workingTexture->setFilename(filename);
  workingTexture->setFilepath(filepath);
  workingTexture->setMipmapped(true);
  workingTexture->setAnisotropy(true);
  workingTexture->setRepeating(true);

  Texture* texture = workingTexture;
  manager->load(workingTexture);
  workingTexture = NULL;

  return texture;
}


void ie::WavefrontLoader::map_Kd(std::vector<std::string> tokens)
{
  Texture* texture = loadTexture(tokens);
  workingMaterial->setDiffuseTexture(texture);
}


void ie::WavefrontLoader::map_bump(std::vector<std::string> tokens)
{
  Texture* texture = loadTexture(tokens);
  workingMaterial->setNormalTexture(texture);
}

void ie::WavefrontLoader::rxMsg(Imessage*)
{
  //TODO populate
}

void ie::WavefrontLoader::quit(void)
{
  if (workingMesh != NULL) {delete workingMesh;}
  if (workingRenderUnit != NULL) {delete workingRenderUnit;}
  if (bufferRenderUnit != NULL) {delete bufferRenderUnit;}
  if (workingMaterial != NULL) {delete workingMaterial;}
  if (workingTexture != NULL) {delete workingTexture;}
  reset();
}
