//___|"ie_glsl.cpp"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_glsl.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

#include "ie_asset_manager.h"
#include "ie_config.h"
#include "ie_shader.h"

void ie::GlslLoader::setLoadDestination(AssetManager* m) {manager = m;}


void ie::GlslLoader::load(std::string name, std::string vertex,
                          std::string fragment)
{
  load(name, ie::DEFAULT_GLSL_PATH, vertex, ie::DEFAULT_GLSL_PATH, fragment); 
}

void ie::GlslLoader::load(std::string name, std::string vertexPath,
                          std::string vertex, std::string fragmentPath,
                          std::string fragment)
{
  std::vector<std::string> files;
  files.push_back(vertexPath + vertex);
  files.push_back(fragmentPath + fragment);

  createProgram(name, files);
}


void ie::GlslLoader::createProgram(std::string name, std::vector<std::string> files)
{
  Shader* shader = new Shader(); 
  shader->setName(name);
  unsigned int fileAmount = files.size();

  for (auto it = files.begin(); it != files.end(); it++)
  {
    *it = read(*it);
  }

  std::vector<GLuint> shaderIds;

  if (fileAmount == 2)
  {
    shaderIds.push_back(compileShader(GL_VERTEX_SHADER, files[0]));
    shaderIds.push_back(compileShader(GL_FRAGMENT_SHADER, files[1]));
  }

  linkShaders(shader, shaderIds);

  loadProgram(shader);
}


std::string ie::GlslLoader::read(std::string file)
{
  std::ifstream glslFile;
  glslFile.open(file.c_str());
  std::stringstream buffer;
  
  if (glslFile.is_open())
  {
    buffer << glslFile.rdbuf();
    glslFile.close();
  }
  else
  {
    std::cout << "GLSL file (" << file <<
    ") failed to open for reading." << std::endl;
  }
  return buffer.str();
}


GLuint ie::GlslLoader::compileShader(GLenum type, std::string source)
{
  GLuint shaderId = glCreateShader(type);
  const char* codeParamAdapter[1]; //glShaderSource needs char**
  codeParamAdapter[0] = source.c_str();
  glShaderSource(shaderId, 1, codeParamAdapter, 0);
  glCompileShader(shaderId);
  GLint compileStatus;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus != GL_TRUE)
  {
    GLint infoLogLength;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar* buffer = new GLchar[infoLogLength];
    GLsizei bufferSize; //Not used: required as func param
    glGetShaderInfoLog(shaderId, infoLogLength, &bufferSize, buffer);
    std::cout << "Shader Compile Error: " << buffer << std::endl;
    delete [] buffer;
  }
  return shaderId;
}


void ie::GlslLoader::linkShaders(Shader* shader, std::vector<GLuint> shaderIds)
{
  GLuint programId = shader->getProgramId();

  for (auto id = shaderIds.begin(); id != shaderIds.end(); id++)
  {
    glAttachShader(programId, *id);
  }

  glLinkProgram(programId);
  GLint linkStatus;
  glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
  if (linkStatus != GL_TRUE)
  {
    GLint infoLogLength;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar* buffer = new GLchar[infoLogLength];
    GLsizei bufferSize; //Not used: required as func param
    glGetProgramInfoLog(programId, infoLogLength, &bufferSize, buffer);
    std::cout << "Shader Program Link Error: " << buffer << std::endl;
    delete [] buffer;
  }

  for (auto id = shaderIds.begin(); id != shaderIds.end(); id++)
  {
    glDetachShader(programId, *id);
    glDeleteShader(*id);
  }
}


void ie::GlslLoader::loadProgram(Shader* shader)
{
  manager->load(shader);
}
