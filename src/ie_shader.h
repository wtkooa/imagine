#ifndef IE_SHADER_H
#define IE_SHADER_H

//___|"ie_shader.h"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <map>
#include <string>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "ie_asset.h"

namespace ie
{

  class Shader : public Asset
  {
    public:
    Shader();
    ~Shader();

    void setUniform(GLint, glm::mat4);
    void setUniform(GLint, glm::vec3);
    void setUniform(GLint, float); 
    void setUniform(GLint, bool);

    void setUniform(std::string, glm::mat4);
    void setUniform(std::string, glm::vec3);
    void setUniform(std::string, float);
    void setUniform(std::string, bool);

    void setUniform(std::string, unsigned int, glm::mat4);
    void setUniform(std::string, unsigned int, glm::vec3);
    void setUniform(std::string, unsigned int, float);
    void setUniform(std::string, unsigned int, bool);

    void setUniform(std::string, unsigned int, std::string, glm::mat4);
    void setUniform(std::string, unsigned int, std::string, glm::vec3);
    void setUniform(std::string, unsigned int, std::string, float);
    void setUniform(std::string, unsigned int, std::string, bool);

    GLuint getProgramId(void);
    GLint getUniformLocation(std::string);
    GLint getUniformArrayLocation(std::string, unsigned int);
    GLint getUniformArrayStructLocation(std::string, unsigned int, std::string);

    private:
    GLuint programId;
  };

}

#endif
