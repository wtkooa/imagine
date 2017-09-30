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
#include <fstream>
#include <map>
#include <string>
#include <sstream>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

#include "ie_messages.h"
#include "ie_packages.h"
#include "ie_utils.h"

//___|READING GLSL SOURCE FILES|________________________________________________

ie::GlslFileReader::GlslFileReader() {}
ie::GlslFileReader::GlslFileReader(std::string filepath, std::string filename)
{
  read(filepath, filename);
}


bool ie::GlslFileReader::clear(void)
{
  package.sourceCode.clear();
}

//READING GLSL SOURCE FROM FILE
ie::GlslSourcePackage ie::GlslFileReader::read(std::string filepath,
                                               std::string filename)
{
  std::ifstream glslFile;
  glslFile.open((filepath + filename).c_str());
  if (glslFile.is_open())
  {
    clear();
    std::stringstream buffer;
    buffer << glslFile.rdbuf();
    package.sourceCode = buffer.str();
    package.filename = filename;
    package.filepath = filepath;
    glslFile.close();
  } else {
    std::cout << "GLSL file '" << filename <<
    "' failed to open..." << std::endl;
  }
  return package;
}


ie::GlslSourcePackage ie::GlslFileReader::wrapGlslSourcePackage(void)
{
    return package;
}

//______________________________________________________________________________

//___|COMPILING GLSL SOURCE AND CREATING SHADER PROGRAMS|_______________________

ie::GlslCompiler::GlslCompiler() {}
ie::GlslCompiler::GlslCompiler(std::string name,
                               std::string vShaderFilepath,
                               std::string vShaderFilename,
                               std::string fShaderFilepath,
                               std::string fShaderFilename)
{
  compile(name,vShaderFilepath, vShaderFilename,
          fShaderFilepath, fShaderFilename);
}


bool ie::GlslCompiler::clear(void)
{
  package.uniforms.clear();
}

//FINDING UNIFORMS - COMPILING GLSL SOURCE - LINKING SHADER PROGRAM
ie::ShaderProgramPackage ie::GlslCompiler::compile(std::string name,
                                        std::string vShaderFilepath,
                                        std::string vShaderFilename,
                                        std::string fShaderFilepath,
                                        std::string fShaderFilename)
{
  clear();
  ie::GlslFileReader glslReader;
  ie::GlslSourcePackage vSourcePack = glslReader.read(vShaderFilepath,
                                                      vShaderFilename);
  vertexShaderId = compileShader(vShaderFilename, vSourcePack.sourceCode,
                                                       GL_VERTEX_SHADER);
  ie::GlslSourcePackage fSourcePack = glslReader.read(fShaderFilepath,
                                                      fShaderFilename);
  fragmentShaderId = compileShader(fShaderFilename, fSourcePack.sourceCode,
                                                       GL_FRAGMENT_SHADER);
  linkShaderProgram();
  package.name = name;
  package.vShaderFilename = vShaderFilename;
  package.vShaderFilepath = vShaderFilepath;
  package.fShaderFilename = fShaderFilename;
  package.fShaderFilepath = fShaderFilepath;
  package.programId = programId;
  package.vertexShaderId = vertexShaderId;
  package.fragmentShaderId = fragmentShaderId;

  std::map<std::string, GlslUniformPackage> vShaderUniforms =
                                      findUniforms(programId,
                                                   vShaderFilepath,
                                                   vShaderFilename);
  std::map<std::string, GlslUniformPackage> fShaderUniforms =
                                      findUniforms(programId,
                                                   fShaderFilepath,
                                                   fShaderFilename);
  vShaderUniforms.insert(fShaderUniforms.begin(),
                         fShaderUniforms.end());
  package.uniforms = vShaderUniforms; 
  return package; 
}


//COMPILING SHADER SOURCE CODE
GLuint ie::GlslCompiler::compileShader(std::string filename,
                                       std::string sourceCode,
                                       GLenum type)
{
  GLuint shaderId = glCreateShader(type);
  const char* codeParamAdapter[1]; //glShaderSource needs char**
  codeParamAdapter[0] = sourceCode.c_str();
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
    std::cout << "Shader '" << filename <<
    "' Compile Error: " << buffer << std::endl;
    delete [] buffer;
  }
  return shaderId;
}

//PARSING GLSL SOURCE CODE FOR UNIFORMS
std::map<std::string, ie::GlslUniformPackage> ie::GlslCompiler::findUniforms(
                                              GLuint progId,
                                              std::string filepath,
                                              std::string filename)
{
  std::ifstream glslFile;
  glslFile.open((filepath + filename).c_str());
  if (glslFile.is_open())
  {
    std::map<std::string, ie::GlslUniformPackage> uniforms;
    std::string line;
    while (getline(glslFile, line))
    {
      std::string token = split(line, ' ', 0);
      if (token == "uniform")
      {
        ie::GlslUniformPackage uniPackage;
        std::string type = split(line, ' ', 1);
        std::string name = split(line, ' ', 2);
        name.pop_back();
        uniPackage.name = name;
        uniPackage.type = type;
        uniPackage.location = glGetUniformLocation(progId,
                                                   uniPackage.name.c_str());
        uniforms[uniPackage.name] = uniPackage;
      }
    }
  glslFile.close();
  return uniforms;
  }
  else
  {
    std::cout << "GLSL file '" << filename <<
    "' failed to open for uniform search" << std::endl;
  }
}


//LINKING COMPILED SHADERS
bool ie::GlslCompiler::linkShaderProgram(void)
{
  programId = glCreateProgram();
  glAttachShader(programId, vertexShaderId);
  glAttachShader(programId, fragmentShaderId);
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
}


ie::ShaderProgramPackage ie::GlslCompiler::wrapShaderProgramPackage(void)
{
  return package;
}

//______________________________________________________________________________
