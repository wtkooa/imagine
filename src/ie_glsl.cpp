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
#include "ie_communication.h"
#include "ie_config.h"
#include "ie_log.h"
#include "ie_messages.h"
#include "ie_shader.h"
#include "ie_utils.h"

ie::GlslLoader::GlslLoader()
{
  reset();
}


void ie::GlslLoader::reset(void)
{
  manager = NULL;
}


void ie::GlslLoader::init(void)
{
  nexus->addConnection(IE_GLSL_ICOM_HANDLE, this);
  NexusMsg msg(IE_LOG_ICOM_HANDLE, IE_NEXUS_OUT_CONNECTION_CMD, this);
  nexus->rxMsg(&msg); 
  msg.target = IE_ASSET_MANAGER_ICOM_HANDLE;
  nexus->rxMsg(&msg); 
}


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
  bool abort = false;
  Shader* shader = new Shader(); 
  shader->setName(name);
  unsigned int fileAmount = files.size();
  std::vector<std::string> sources = files;

  for (auto it = sources.begin(); it != sources.end(); it++)
  {
    *it = read(*it);
    if (*it == "")
    {
      abort = true;
    }
  }

  std::vector<GLuint> shaderIds;

  if (abort == false)
  {
    if (fileAmount == 2)
    {
      shaderIds.push_back(compileShader(GL_VERTEX_SHADER, sources[0], files[0]));
      shaderIds.push_back(compileShader(GL_FRAGMENT_SHADER, sources[1], files[1]));
    }
  }

  GLuint impossibleValue = -1;
  for (auto it = shaderIds.begin(); it != shaderIds.end(); it++)
  {
    if (*it == impossibleValue) {abort = true;}
  }

  if (abort == false)
  {
    bool result = linkShaders(shader, shaderIds);
    if (result == false) {abort = true;}
  }

  if (abort == false)
  {
    loadProgram(shader);
  }
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

    //log->info("GLSL file '%s' read successfully",
    //          getFilenameFromPath(file).c_str());

    return buffer.str();
  }
  else
  {
    //log->warning("GLSL file '%s' failed to open for reading",
    //             getFilenameFromPath(file).c_str());
    return "";
  }
}


GLuint ie::GlslLoader::compileShader(GLenum type, std::string source,
                                     std::string filename)
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
    //log->warning("Shader Compilation Error in file '%s'",
    //             getFilenameFromPath(filename).c_str());
    //log->debug(buffer);
    delete [] buffer;
    GLuint impossibleValue = -1;
    return impossibleValue;
  }
  else
  {
    //log->info("Shader '%s' Compiled Successfully",
    //          getFilenameFromPath(filename).c_str());
    return shaderId;
  }
}


bool ie::GlslLoader::linkShaders(Shader* shader, std::vector<GLuint> shaderIds)
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
    //log->warning("Shader Linking Error");
    //log->debug(buffer);
    delete [] buffer;
    return false;
  }
  else
  {
    //log->info("Shader Linking Successful");
    return true;
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


void ie::GlslLoader::rxMsg(Imessage* msg)
{
  //TODO populate
}
