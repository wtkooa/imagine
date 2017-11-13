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
#include "ie_config.h"
#include "ie_material.h"
#include "ie_mesh.h"
#include "ie_texture.h"
#include "ie_utils.h"

//___|WAVEFRONT OBJ FILE READER|________________________________________________

ie::WavefrontObjectLoader::WavefrontObjectLoader()
{
  reset();
}


void ie::WavefrontObjectLoader::reset(void)
{
  manager = NULL;
  workingMesh = NULL;
  workingRenderUnit = NULL;
  bufferRenderUnit = NULL;
  workingMaterial = NULL;
  workingTexture = NULL;
}

void ie::WavefrontObjectLoader::setLoadDestination(AssetManager* m)
{
  manager = m;
}


void ie::WavefrontObjectLoader::read(std::string filename)
{
  read(ie::WAVEFRONT_PATH, filename);  
}

void ie::WavefrontObjectLoader::read(std::string filepath, std::string filename)
{
  std::vector<std::string> tokens = split(filename, ".");  
  std::string extention = tokens.back();

  if (extention == "mtl") {readMtl(filepath, filename);}
  else if (extention == "obj") {readObj(filepath, filename);}
  else
  {
    std::cout << "Warning: Unrecognized wavefront file extention (" <<
    filename << ")." << std::endl;
  }
}

void ie::WavefrontObjectLoader::readObj(std::string filepath, std::string filename)
{
  std::ifstream objFile;
  objFile.open((filepath + filename).c_str());
  bool failedToOpen = !objFile.is_open();

  if (failedToOpen)
  {
    std::cout << "Warning: Wavefront obj file (" << filename <<
    ") failed to open for reading." << std::endl;
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
        std::cout << "Warning: Unrecognized wavefront obj command. ("  <<
        command << ")" << std::endl;
      }
    }
    loadObject();
    objFile.close();
  }
}


void ie::WavefrontObjectLoader::mtllib(std::string filepath, 
                                       std::vector<std::string> tokens)
{
  for (auto file = tokens.begin(); file != tokens.end(); file++)
  {
    readMtl(filepath, *file);
  }
}


void ie::WavefrontObjectLoader::loadObject(void)
{
  if (workingMesh != NULL)
  {
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
  }
}


void ie::WavefrontObjectLoader::loadRenderUnit(void)
{
  if (workingRenderUnit != NULL)
  {
    procRenderUnitBuffer(); 
    workingMesh->addRenderUnit(workingRenderUnit);
  }
}


void ie::WavefrontObjectLoader::procRenderUnitBuffer(void)
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
  bufferRenderUnit->clearAttrib(E_ATTRIB);
}


void ie::WavefrontObjectLoader::object(std::vector<std::string> tokens,
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


void ie::WavefrontObjectLoader::position(std::vector<std::string> tokens)
{
  bufferRenderUnit->addPositionAttrib(glm::vec3(stof(tokens[0]),
                                                stof(tokens[1]),
                                                stof(tokens[2])));
}


void ie::WavefrontObjectLoader::mapping(std::vector<std::string> tokens)
{
  bufferRenderUnit->addMapAttrib(glm::vec2(stof(tokens[0]),
                                           stof(tokens[1])));
}


void ie::WavefrontObjectLoader::normal(std::vector<std::string> tokens)
{
  bufferRenderUnit->addUnpackedNormalAttrib(glm::vec3(stof(tokens[0]),
                                              stof(tokens[1]),
                                              stof(tokens[2])));
}


void ie::WavefrontObjectLoader::usemtl(std::vector<std::string> tokens)
{
  loadRenderUnit();
  workingRenderUnit = new RenderUnit(); 
  workingRenderUnit->setMaterial(manager->getMaterial(tokens[0]));
}


void ie::WavefrontObjectLoader::face(std::vector<std::string> tokens)
{
  unsigned int tokenAmount = tokens.size();
  if (tokenAmount == 4)
  {
    std::cout << "Warning: Quad face detected" << std::endl;
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


void ie::WavefrontObjectLoader::readMtl(std::string filepath, std::string filename)
{
  std::ifstream mtlFile;
  mtlFile.open((filepath + filename).c_str());
  bool failedToOpen = !mtlFile.is_open();

  if (failedToOpen)
  {
    std::cout << "Warning: Wavefront mlt file (" << filename <<
    ") failed to open for reading." << std::endl;
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
      else if (command == "newmtl") {newmtl(tokens);}
      else if (command == "Ns") {shininess(tokens);}
      else if (command == "Ka") {ambient(tokens);}
      else if (command == "Kd") {diffuse(tokens);}
      else if (command == "Ks") {specular(tokens);}
      else if (command == "Ke") {emission(tokens);}
      else if (command == "map_Kd") {map_Kd(tokens);}
      else if (command == "map_bump") {map_bump(tokens);}
      else
      {
        std::cout << "Warning: Unrecognized wavefront mtl command. ("  <<
        command << ")" << std::endl;
      }
    }
    loadMaterial();
  }
}


void ie::WavefrontObjectLoader::loadMaterial(void)
{
  if (workingMaterial != NULL)
  {
    manager->load(workingMaterial);
    workingMaterial = NULL;
  }
}


void ie::WavefrontObjectLoader::newmtl(std::vector<std::string> tokens)
{
  loadMaterial();
  workingMaterial = new Material();   
  workingMaterial->setName(tokens[0]);
}


void ie::WavefrontObjectLoader::shininess(std::vector<std::string> tokens)
{
  workingMaterial->setShininess(stof(tokens[0]));
}


void ie::WavefrontObjectLoader::ambient(std::vector<std::string> tokens)
{
  workingMaterial->setAmbient(glm::vec3(stof(tokens[0]),
                                        stof(tokens[1]),
                                        stof(tokens[2])));
}


void ie::WavefrontObjectLoader::diffuse(std::vector<std::string> tokens)
{
  workingMaterial->setDiffuse(glm::vec3(stof(tokens[0]),
                                        stof(tokens[1]),
                                        stof(tokens[2])));
}


void ie::WavefrontObjectLoader::specular(std::vector<std::string> tokens)
{
  workingMaterial->setSpecular(glm::vec3(stof(tokens[0]),
                                         stof(tokens[1]),
                                         stof(tokens[2])));
}


void ie::WavefrontObjectLoader::emission(std::vector<std::string> tokens)
{
  workingMaterial->setEmission(glm::vec3(stof(tokens[0]),
                                         stof(tokens[1]),
                                         stof(tokens[2])));
}


ie::Texture* ie::WavefrontObjectLoader::loadTexture(std::vector<std::string> tokens)
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


void ie::WavefrontObjectLoader::map_Kd(std::vector<std::string> tokens)
{
  Texture* texture = loadTexture(tokens);
  workingMaterial->setDiffuseTexture(texture);
}


void ie::WavefrontObjectLoader::map_bump(std::vector<std::string> tokens)
{
  Texture* texture = loadTexture(tokens);
  workingMaterial->setNormalTexture(texture);
}


void ie::WavefrontObjectLoader::quit(void)
{
  if (workingMesh != NULL) {delete workingMesh;}
  if (workingRenderUnit != NULL) {delete workingRenderUnit;}
  if (bufferRenderUnit != NULL) {delete bufferRenderUnit;}
  if (workingMaterial != NULL) {delete workingMaterial;}
  if (workingTexture != NULL) {delete workingTexture;}
  reset();
}

/*
void ie::WavefrontObjectLoader::read(std::string filepath, std::string filename)
{
  bool hasQuads = false;
  std::vector<std::string> mtllib;
  std::ifstream objFile;
  objFile.open((filepath + filename).c_str());
  if (objFile.is_open())
  {
    size_t indexOffset = 0;
    int noData = -1;
    std::string line;
    while (getline(objFile, line))
    {
      std::string command = split(line, ' ', 0);
      int tokenAmount = std::count(line.begin(), line.end(), ' ');

      if (command == "#") {continue;} //Comment
      else if (command == "") {continue;} //Blank Line
  
      else if (command == "mtllib")
      {
        for (int n = 1; n <= tokenAmount; n++)  
        {
          std::string mtlFilename = split(line, ' ', n);
          mtllib.push_back(mtlFilename);
        }
      }

      else if (command == "usemtl")
      {
        std::string mtl = split(line, ' ', 1);
      }

      else if (command == "s")
      {
          std::string smooth = split(line, ' ', 1);
          if (smooth == "off")
          {
          }
          else
          {
          }
      }

      else if (command == "o")
      {
      }

      else if (command == "g")
      {
      }

      else if (command == "v")
      {
        float x = std::stof(split(line, ' ', 1));
        float y = std::stof(split(line, ' ', 2));
        float z = std::stof(split(line, ' ', 3));  
        float w = 1.0f;
        if (tokenAmount == 4) {w = std::stof(split(line, ' ', 4));}
      }

      else if (command == "vt")
      {
        float u = std::stof(split(line, ' ', 1));
        float v = std::stof(split(line, ' ', 2));
        float w = 0.0f;
        if (tokenAmount == 3) {w = std::stof(split(line, ' ', 3));}
      }

      else if (command == "vn")
      {
        float x = std::stof(split(line, ' ', 1));
        float y = std::stof(split(line, ' ', 2));
        float z = std::stof(split(line, ' ', 3));
      }

      else if (command == "p")
      {
        int verticesInPoint = 1;
        std::string token = split(line, ' ', 1);
        int subTokenAmount = std::count(token.begin(), token.end(), '/') + 1;
        int v = std::stoi(split(token, '/', 0));
        int t = noData; 
        int n = noData;
        int w = verticesInPoint;
        if (subTokenAmount == 2)
        {
          n = std::stoi(split(token, ' ', 1));
        }
        else if (subTokenAmount == 3)
        {
          std::string tex = split(token, '/', 1);
          if (tex != "") {t = std::stoi(tex);}
          n = std::stoi(split(token, '/', 2));
        }
        indexOffset++;
      }

      else if (command == "l")
      {
        int verticesInLine = 2;
        for (int vert = 1; vert <= verticesInLine; vert++)
        {
          std::string token = split(line, ' ', vert);  
          int subTokenAmount = std::count(token.begin(), token.end(), '/') + 1;
          int v = std::stoi(split(token, '/', 0));
          int t = noData;
          int n = noData;
          int w = verticesInLine;
          if (subTokenAmount == 2)
          {
            n = std::stoi(split(token, ' ', 1));
          }
          else if (subTokenAmount == 3)
          {
            std::string tex = split(token, '/', 1);
            if (tex != "") {t = std::stoi(tex);}
            n = std::stoi(split(token, '/', 2));
          }
          indexOffset++;
        }
      }

      else if (command == "f")
      {
        int verticesInFace = tokenAmount;
        if (verticesInFace == 4) {hasQuads = true;}
        for (int vertex = 1; vertex <= verticesInFace; vertex++)
        {
          std::string token = split(line, ' ', vertex);
          int subTokenAmount = std::count(token.begin(), token.end(), '/') + 1;
          int v = std::stoi(split(token, '/', 0));
          int t = noData;
          int n = noData;
          int w = verticesInFace;
          if (subTokenAmount == 2)
          {
            n = std::stoi(split(token, ' ', 1));
          }
          else if (subTokenAmount == 3)
          {
            std::string tex = split(token, '/', 1);
            if (tex != "") {t = std::stoi(tex);}
            n = std::stoi(split(token, '/', 2));
          }
          indexOffset++;
        } 
      }

      else
      {
        std::cout << "Warning: Unrecognized command '" << command <<
               "' in OBJ file '" << filename << "'." << std::endl;
      }
    }
  }
  else
  {
    std::cout << "File " << filename <<
    " failed to open for reading..." << std::endl;
  } 
  objFile.close();

  size_t materialAmount = mtllib.size();
  for (int n = 0; n < materialAmount; n++)
  {
    std::string materialFilename = mtllib[n];
    materialReader.read(filepath, materialFilename);
  } 
  if (hasQuads)
  {
    std::cout << "Warning: This OBJ file contains QUAD faces." <<
    " These will not be rendered." << std::endl;
  }
}


//______________________________________________________________________________

//___|WAVEFRONT MTL FILE READER|________________________________________________

ie::WavefrontMaterialFileReader::WavefrontMaterialFileReader(void) {}
ie::WavefrontMaterialFileReader::WavefrontMaterialFileReader(std::string filepath,
                                                             std::string filename)
{
  read(filepath, filename);
}

bool ie::WavefrontMaterialFileReader::clear(void)
{
}

void ie::WavefrontMaterialFileReader::read(std::string filepath, std::string filename)
{
  std::ifstream mtlFile;
  mtlFile.open((filepath + filename).c_str());
  if (mtlFile.is_open())
  {
    clear();
    int materialOffset = -1;
    std::string line;
    while (getline(mtlFile, line))
    {
      std::string command = split(line, ' ', 0);
      if (command == "#") {continue;} //Comment
      else if (command == "") {continue;} //Blank

      else if (command == "newmtl")
      {
        materialOffset++;
        std::string name = split(line, ' ', 1);
      }

      else if (command == "Ns")
      {
      }

      else if (command == "Ka")
      {
        float r = std::stof(split(line, ' ', 1));
        float g = std::stof(split(line, ' ', 2));
        float b = std::stof(split(line, ' ', 3));
      }

      else if (command == "Kd")
      {
        float r = std::stof(split(line, ' ', 1));
        float g = std::stof(split(line, ' ', 2));
        float b = std::stof(split(line, ' ', 3));
      }

      else if (command == "Ks")
      {
        float r = std::stof(split(line, ' ', 1));
        float g = std::stof(split(line, ' ', 2));
        float b = std::stof(split(line, ' ', 3));
      }

      else if (command == "Ke")
      {
        float r = std::stof(split(line, ' ', 1));
        float g = std::stof(split(line, ' ', 2));
        float b = std::stof(split(line, ' ', 3));
      }     

      else if (command == "Ni")
      {
      }

      else if (command == "d")
      {
      }

      else if (command == "Tr")
      {
        float Tr = std::stof(split(line, ' ', 1));
        float d = (Tr - 1) * -1;
      }

      else if (command == "illum")
      {
      }
      else if (command == "map_Kd")
      {
        ie::TexturePackage tex;
        size_t start = line.find(" ") + 1;
        tex.filename = line.substr(start);
        tex.type = ie::TextureType::DIFFUSE_MAP;
        tex.mipmapped = true;
        tex.anisotropy = true;
        tex.repeating = true;
      }

      else if (command == "map_bump")
      {
        ie::TexturePackage tex;
        size_t start = line.find(" ") + 1;
        tex.filename = line.substr(start);
        tex.type = ie::TextureType::BUMP_MAP;
        tex.mipmapped = true;
        tex.anisotropy = true;
        tex.repeating = true;
      }
      else
      {
        std::cout << "Warning: Unrecognized command '" << command <<
               "' in MTL file '" << filename << "'." << std::endl;
      }
    }
  }
  else
  {
    std::cout << "File " << filename <<
    " failed to open for reading..." << std::endl;
  }
  mtlFile.close();
  return filePackage;
}
*/
//______________________________________________________________________________
