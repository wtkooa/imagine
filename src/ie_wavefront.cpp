#include "ie_wavefront.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "ie_packages.h"
#include "ie_utils.h"


//Wavefront Object File Reader
ie::WavefrontObjectFileReader::WavefrontObjectFileReader(void) {}
ie::WavefrontObjectFileReader::WavefrontObjectFileReader(std::string filename)
{
  read(filename);
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


ie::WavefrontObjectFilePackage ie::WavefrontObjectFileReader::read(std::string filename)
{
  std::vector<std::string> mtllib;
  std::ifstream objFile;
  objFile.open(filename.c_str());
  if (objFile.is_open())
  {
    clear();
    filePackage.filename = filename;
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
    std::cout << "File " << filename << " failed to open for reading..." << std::endl;
  } 
  objFile.close();

  size_t materialAmount = mtllib.size();
  for (int n = 0; n < materialAmount; n++)
  {
    std::string materialFile = mtllib[n];
    materialReader.read(materialFile);
    filePackage.materialFilePackages.push_back(materialReader.wrapFilePackage());
  } 
  return filePackage;
}

ie::WavefrontObjectFilePackage ie::WavefrontObjectFileReader::wrapFilePackage(void) {return filePackage;}


//Wavefront Material File Reader
ie::WavefrontMaterialFileReader::WavefrontMaterialFileReader(void) {}
ie::WavefrontMaterialFileReader::WavefrontMaterialFileReader(std::string filename)
{
  read(filename);
}

bool ie::WavefrontMaterialFileReader::clear(void)
{
  filePackage.materials.clear();
}

ie::WavefrontMaterialFilePackage ie::WavefrontMaterialFileReader::read(std::string filename)
{
  std::ifstream mtlFile;
  mtlFile.open(filename.c_str());
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
        filePackage.materials[materialOffset].emission == glm::vec3(r,g,b);
      }     
      else if (command == "Ni")
      {
        filePackage.materials[materialOffset].opticalDensity == std::stof(split(line, ' ', 1));
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
        ie::WavefrontTexturePackage tex;
        tex.filename = split(line, ' ', 1);
        tex.type = ie::TextureType::DIFFUSE_MAP;
        filePackage.materials[materialOffset].texturePackages.push_back(tex);
      }
      else if (command == "map_bump")
      {
        ie::WavefrontTexturePackage tex;
        tex.filename = split(line, ' ', 1);
        tex.type = ie::TextureType::BUMP_MAP;
        filePackage.materials[materialOffset].texturePackages.push_back(tex);
      }
      else if (command == "map_Ka")
      {
        ie::WavefrontTexturePackage tex;
        tex.filename = split(line, ' ', 1);
        tex.type = ie::TextureType::AMBIENT_MAP;
        filePackage.materials[materialOffset].texturePackages.push_back(tex);
      }
      else if (command == "map_Ks")
      {
        ie::WavefrontTexturePackage tex;
        tex.filename = split(line, ' ', 1);
        tex.type = ie::TextureType::SPECULAR_MAP;
        filePackage.materials[materialOffset].texturePackages.push_back(tex);
      }
      else if (command == "map_Ns")
      {
        ie::WavefrontTexturePackage tex;
        tex.filename = split(line, ' ', 1);
        tex.type = ie::TextureType::HIGHLIGHT_MAP;
        filePackage.materials[materialOffset].texturePackages.push_back(tex);
      }
      else if (command == "map_d")
      {
        ie::WavefrontTexturePackage tex;
        tex.filename = split(line, ' ', 1);
        tex.type = ie::TextureType::ALPHA_MAP;
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
    std::cout << "File " << filename << " failed to open for reading..." << std::endl;
  }
  mtlFile.close();
  return filePackage;
}

ie::WavefrontMaterialFilePackage ie::WavefrontMaterialFileReader::wrapFilePackage(void) {return filePackage;}
