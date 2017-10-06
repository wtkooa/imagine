#ifndef IE_TERRAIN_H
#define IE_TERRAIN_H

//___|"ie_terrain.h"|___________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec3.hpp>

#include "ie_packages.h"

namespace ie
{

  class TerrainGenerator
  {
    public:
    TerrainGenerator();
    TerrainGenerator(short);
    TerrainGenerator(short, float);

    void generateTerrain(void);
    void generateTerrain(short, float);

    void applyPerlin(float, float, float);
    void applyDemoBlends(void);
    void calcFaceNormals(void);
    void smoothNormals(void);
    glm::uvec2 setBlendValue(glm::uvec2, short, short);

    void addTexture(std::string, std::string); 

    ie::TerrainPackage wrapTerrainPackage(void);

    void setName(std::string);
    void setDim(short);

    void setShininess(float);
    void setAmbient(glm::vec3);
    void setDiffuse(glm::vec3); 
    void setSpecular(glm::vec3);
    void setEmission(glm::vec3);

    private:
    void clear(void);

    std::string name;
    short dim;
    float unitSize;
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::uvec2> blends;
    std::vector<glm::ivec4> indices;
    std::vector<TexturePackage> textures;
    std::vector<glm::vec3> faceNormals;

    float shininess;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emission;
  };

}

#endif
