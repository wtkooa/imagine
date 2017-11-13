//___|"ie_shader.cpp"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_shader.h"

#include <iostream>
#include <string>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

ie::Shader::Shader()
{
  programId = glCreateProgram();
}


ie::Shader::~Shader()
{
  glUseProgram(0);
  glDeleteProgram(programId);
}


GLuint ie::Shader::getProgramId(void) {return programId;}


GLint ie::Shader::getUniformLocation(std::string uniform)
{
  GLint location = glGetUniformLocation(programId, uniform.c_str());
  if (location == -1)
  {
    std::cout << "Couldn't find " << uniform << " location."  << std::endl;
  }
  return location;
}


GLint ie::Shader::getUniformArrayLocation(std::string uniform,
                                          unsigned int numIndex)
{
  std::string strIndex = std::to_string(numIndex);
  uniform = uniform + "[" + strIndex + "]";
  GLint location = glGetUniformLocation(programId, uniform.c_str()); 
  if (location == -1)
  {
    std::cout << "Couldn't find " << uniform << " location."  << std::endl;
  }
  return location;
}


GLint ie::Shader::getUniformArrayStructLocation(std::string uniform,
                                                unsigned int numIndex,
                                                std::string structure)
{
  std::string strIndex = std::to_string(numIndex);
  uniform = uniform + "[" + strIndex + "]." + structure;
  GLint location = glGetUniformLocation(programId, uniform.c_str()); 
  if (location == -1)
  {
    std::cout << "Couldn't find " << uniform << " location."  << std::endl;
  }
  return location;
}


void ie::Shader::setUniform(GLint location, glm::mat4 data)
{
  glUniformMatrix4fv(location, 1, GL_FALSE, &data[0][0]);
}

void ie::Shader::setUniform(GLint location, glm::vec3 data)
{
  glUniform3fv(location, 1, &data[0]);
}

void ie::Shader::setUniform(GLint location, float data)
{
  glUniform1f(location, data);
}

void ie::Shader::setUniform(GLint location, bool data)
{
  glUniform1i(location, data);
}


void ie::Shader::setUniform(std::string uniform, glm::mat4 data)
{
  GLint location = getUniformLocation(uniform);
  setUniform(location, data);
}

void ie::Shader::setUniform(std::string uniform, glm::vec3 data)
{
  GLint location = getUniformLocation(uniform);
  setUniform(location, data);
}

void ie::Shader::setUniform(std::string uniform, float data)
{
  GLint location = getUniformLocation(uniform);
  setUniform(location, data);
}

void ie::Shader::setUniform(std::string uniform, bool data)
{
  GLint location = getUniformLocation(uniform);
  setUniform(location, data);
}


void ie::Shader::setUniform(std::string uniform, unsigned int numIndex, glm::mat4 data)
{
  GLint location = getUniformArrayLocation(uniform, numIndex); 
  setUniform(location, data);
}

void ie::Shader::setUniform(std::string uniform, unsigned int numIndex, glm::vec3 data)
{
  GLint location = getUniformArrayLocation(uniform, numIndex); 
  setUniform(location, data);
}

void ie::Shader::setUniform(std::string uniform, unsigned int numIndex, float data)
{
  GLint location = getUniformArrayLocation(uniform, numIndex); 
  setUniform(location, data);
}

void ie::Shader::setUniform(std::string uniform, unsigned int numIndex, bool data)
{
  GLint location = getUniformArrayLocation(uniform, numIndex); 
  setUniform(location, data);
}


void ie::Shader::setUniform(std::string uniform,
                            unsigned int numIndex,
                            std::string structure,
                            glm::mat4 data)
{
  GLint location = getUniformArrayStructLocation(uniform, numIndex, structure); 
  setUniform(location, data);
}

void ie::Shader::setUniform(std::string uniform,
                            unsigned int numIndex,
                            std::string structure,
                            glm::vec3 data)
{
  GLint location = getUniformArrayStructLocation(uniform, numIndex, structure); 
  setUniform(location, data);
}

void ie::Shader::setUniform(std::string uniform,
                            unsigned int numIndex,
                            std::string structure,
                            float data)
{
  GLint location = getUniformArrayStructLocation(uniform, numIndex, structure); 
  setUniform(location, data);
}

void ie::Shader::setUniform(std::string uniform,
                            unsigned int numIndex,
                            std::string structure,
                            bool data)
{
  GLint location = getUniformArrayStructLocation(uniform, numIndex, structure); 
  setUniform(location, data);
}
