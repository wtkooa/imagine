#ifndef IE_DEFINITIONS_H
#define IE_DEFINITIONS_H

//___|"ie_definitions.h"|_______________________________________________________
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
#include <glm/glm.hpp>

#include "ie_assets.h"

namespace ie
{

  class QuickListElement
  {
    public:
    unsigned int modelId;
    std::vector<short> renderUnitList;
  };

    class VFormat
    {
    public:
    glm::vec3 vertex;
  };

  class VNFormat
  {
    public:
    glm::vec3 vertex;
    glm::vec3 normal;
  };

  class VTNFormat
  {
    public:
    glm::vec3 vertex;
    glm::vec2 texture;
    glm::vec3 normal;
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

  enum VboDataFormat {V, VN, VTN};

  class VboRenderUnitLocation 
  {
    public:
    short renderUnit;
    VboDataFormat format;
    unsigned int location;
    unsigned int indexAmount;
  };

}

#endif
