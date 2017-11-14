#ifndef IE_VRAM_H
#define IE_VRAM_H

//___|"ie_vram.h"|______________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ie_enum.h"

namespace ie
{

  class PFormat
  {
    public:
    glm::vec3 vertex;
  };


  class PNFormat
  {
    public:
    glm::vec3 vertex;
    glm::vec3 normal;
  };


  class PMNFormat
  {
    public:
    glm::vec3 vertex;
    glm::vec2 texture;
    glm::vec3 normal;
  };


  class PMNCBFormat
  {
    public:
    glm::vec3 vertex;
    glm::vec2 texture;
    glm::vec3 normal;
    glm::vec3 color;
    glm::uvec2 blend;
  };


  class VboPair
  {
    public:
    void genBuffers(void);
    GLuint readVbo;
    GLuint writeVbo;
    void swap(void);
    void release(void);
  };


  class StaticRenderUnitLocation 
  {
    public:
    short renderUnit;
    IEenum format;
    unsigned int location;
    unsigned int indexAmount;
  };


  class TerrainRenderUnitLocation
  {
    public:
    unsigned int location;
    unsigned int indexAmount;
  };

}

#endif
