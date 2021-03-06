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

#include "ie_packages.h"
#include "ie_utils.h"

//___|WAVEFRONT OBJ FILE READER|________________________________________________

ie::WavefrontObjectFileReader::WavefrontObjectFileReader(void) {}
ie::WavefrontObjectFileReader::WavefrontObjectFileReader(std::string filepath,
                                                         std::string filename)
{
  read(filepath, filename);
}


bool ie::WavefrontObjectFileReader::clear(void)
{
  filePackage.filename = "";
  filePackage.objects.clear();
  filePackage.objectGroups.clear();
  filePackage.materialGroups.clear();
  filePackage.smoothGroups.clear();
  filePackage.v.clear();
  filePackage.vt.clear();
  filePackage.vn.clear();
  filePackage.f.clear();
  filePackage.materialFilePackages.clear();
}


ie::WavefrontObjectFilePackage ie::WavefrontObjectFileReader::read(std::string filepath,
                                                                   std::string filename)
{
  hasQuads = false;
  std::vector<std::string> mtllib;
  std::ifstream objFile;
  objFile.open((filepath + filename).c_str());
  if (objFile.is_open())
  {
    clear();
    filePackage.filename = filename;
    filePackage.filepath = filepath;
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
        filePackage.materialGroups[indexOffset] = mtl;  
      }

      else if (command == "s")
      {
          std::string smooth = split(line, ' ', 1);
          if (smooth == "off")
          {
            filePackage.smoothGroups[indexOffset] = 0;
          }
          else
          {
            filePackage.smoothGroups[indexOffset] = std::stoi(smooth);
          }
      }

      else if (command == "o")
      {
        filePackage.objects[indexOffset] = split(line, ' ', 1);
      }

      else if (command == "g")
      {
        filePackage.objectGroups[indexOffset] = split(line, ' ', 1);
      }

      else if (command == "v")
      {
        float x = std::stof(split(line, ' ', 1));
        float y = std::stof(split(line, ' ', 2));
        float z = std::stof(split(line, ' ', 3));  
        float w = 1.0f;
        if (tokenAmount == 4) {w = std::stof(split(line, ' ', 4));}
        filePackage.v.push_back(glm::vec4(x,y,z,w));
      }

      else if (command == "vt")
      {
        float u = std::stof(split(line, ' ', 1));
        float v = std::stof(split(line, ' ', 2));
        float w = 0.0f;
        if (tokenAmount == 3) {w = std::stof(split(line, ' ', 3));}
        filePackage.vt.push_back(glm::vec3(u, v, w));
      }

      else if (command == "vn")
      {
        float x = std::stof(split(line, ' ', 1));
        float y = std::stof(split(line, ' ', 2));
        float z = std::stof(split(line, ' ', 3));
        filePackage.vn.push_back(glm::vec3(x,y,z));
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
        filePackage.f.push_back(glm::ivec4(v,t,n,w));
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
          filePackage.f.push_back(glm::ivec4(v,t,n,w));
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
          filePackage.f.push_back(glm::ivec4(v,t,n,w));
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
    filePackage.materialFilePackages.push_back(materialReader.wrapFilePackage());
  } 
  if (hasQuads)
  {
    std::cout << "Warning: This OBJ file contains QUAD faces." <<
    " These will not be rendered." << std::endl;
  }
  return filePackage;
}


ie::WavefrontObjectFilePackage ie::WavefrontObjectFileReader::wrapFilePackage(void)
{
  return filePackage;
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
  filePackage.materials.clear();
}

ie::WavefrontMaterialFilePackage ie::WavefrontMaterialFileReader::read(std::string filepath,
                                                                       std::string filename)
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
        ie::WavefrontMaterialPackage newMaterialPackage;
        filePackage.materials.push_back(newMaterialPackage);
        filePackage.materials[materialOffset].filename = filename;
        filePackage.materials[materialOffset].filepath = filepath;
        filePackage.materials[materialOffset].name = name;
      }

      else if (command == "Ns")
      {
        filePackage.materials[materialOffset].shininess = std::stof(split(line, ' ', 1));
      }

      else if (command == "Ka")
      {
        float r = std::stof(split(line, ' ', 1));
        float g = std::stof(split(line, ' ', 2));
        float b = std::stof(split(line, ' ', 3));
        filePackage.materials[materialOffset].ambient = glm::vec3(r,g,b);
      }

      else if (command == "Kd")
      {
        float r = std::stof(split(line, ' ', 1));
        float g = std::stof(split(line, ' ', 2));
        float b = std::stof(split(line, ' ', 3));
        filePackage.materials[materialOffset].diffuse = glm::vec3(r,g,b);
      }

      else if (command == "Ks")
      {
        float r = std::stof(split(line, ' ', 1));
        float g = std::stof(split(line, ' ', 2));
        float b = std::stof(split(line, ' ', 3));
        filePackage.materials[materialOffset].specular = glm::vec3(r,g,b);
      }

      else if (command == "Ke")
      {
        float r = std::stof(split(line, ' ', 1));
        float g = std::stof(split(line, ' ', 2));
        float b = std::stof(split(line, ' ', 3));
        filePackage.materials[materialOffset].emission = glm::vec3(r,g,b);
      }     

      else if (command == "Ni")
      {
        filePackage.materials[materialOffset].opticalDensity = std::stof(split(line, ' ', 1));
      }

      else if (command == "d")
      {
        filePackage.materials[materialOffset].dissolve = std::stof(split(line, ' ', 1));
      }

      else if (command == "Tr")
      {
        float Tr = std::stof(split(line, ' ', 1));
        float d = (Tr - 1) * -1;
        filePackage.materials[materialOffset].dissolve = d;
      }

      else if (command == "illum")
      {
        filePackage.materials[materialOffset].illum = std::stoi(split(line, ' ', 1));
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
        filePackage.materials[materialOffset].texturePackages.push_back(tex);
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
        filePackage.materials[materialOffset].texturePackages.push_back(tex);
      }

      else if (command == "map_Ka")
      {
        ie::TexturePackage tex;
        size_t start = line.find(" ") + 1;
        tex.filename = line.substr(start);
        tex.type = ie::TextureType::AMBIENT_MAP;
        tex.mipmapped = true;
        tex.anisotropy = true;
        tex.repeating = true;
        filePackage.materials[materialOffset].texturePackages.push_back(tex);
      }

      else if (command == "map_Ks")
      {
        ie::TexturePackage tex;
        size_t start = line.find(" ") + 1;
        tex.filename = line.substr(start);
        tex.type = ie::TextureType::SPECULAR_MAP;
        tex.mipmapped = true;
        tex.anisotropy = true;
        tex.repeating = true;
        filePackage.materials[materialOffset].texturePackages.push_back(tex);
      }

      else if (command == "map_Ns")
      {
        ie::TexturePackage tex;
        size_t start = line.find(" ") + 1;
        tex.filename = line.substr(start);
        tex.type = ie::TextureType::HIGHLIGHT_MAP;
        tex.mipmapped = true;
        tex.anisotropy = true;
        tex.repeating = true;
        filePackage.materials[materialOffset].texturePackages.push_back(tex);
      }

      else if (command == "map_d")
      {
        ie::TexturePackage tex;
        size_t start = line.find(" ") + 1;
        tex.filename = line.substr(start);
        tex.type = ie::TextureType::ALPHA_MAP;
        tex.mipmapped = true;
        tex.anisotropy = true;
        tex.repeating = true;
        filePackage.materials[materialOffset].texturePackages.push_back(tex);
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


ie::WavefrontMaterialFilePackage ie::WavefrontMaterialFileReader::wrapFilePackage(void)
{
  return filePackage;
}

//______________________________________________________________________________
