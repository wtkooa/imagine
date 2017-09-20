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

//GLSL Source File Reader
ie::GlslFileReader::GlslFileReader() {}
ie::GlslFileReader::GlslFileReader(std::string name)
{
  read(name);
}

bool ie::GlslFileReader::clear(void)
{
  package.sourceCode.clear();
}

ie::GlslSourcePackage ie::GlslFileReader::read(std::string filename)
{
  std::ifstream glslFile;
  glslFile.open(filename.c_str());
  if (glslFile.is_open())
  {
    clear();
    std::stringstream buffer;
    buffer << glslFile.rdbuf();
    package.sourceCode = buffer.str();
    glslFile.close();
  } else {
    std::cout << "GLSL file '" << filename << "' failed to open..." << std::endl;
  }
  package.uniforms = findUniforms(filename);
  return package;
}

std::vector<ie::GlslUniformPackage> ie::GlslFileReader::findUniforms(std::string filename)
{
  std::ifstream glslFile;
  glslFile.open(filename.c_str());
  if (glslFile.is_open())
  {
    std::vector<ie::GlslUniformPackage> uniforms;
    std::string line;
    while (getline(glslFile, line))
    {
      std::string token = split(line, ' ', 0);
      if (token == "uniform")
      {
        ie::GlslUniformPackage package;
        std::string type = split(line, ' ', 1);
        std::string name = split(line, ' ', 2);
        name.pop_back();
        package.name = name;
        package.type = type;
        uniforms.push_back(package);
      }
    }
  glslFile.close();
  return uniforms;
  }
  else
  {
    std::cout << "GLSL file '" << filename << "' failed to open for uniform search" << std::endl;
  }
}

ie::GlslSourcePackage ie::GlslFileReader::wrapGlslSourcePackage(void) {return package;}


//GLSL Compiler
ie::GlslCompiler::GlslCompiler() {}
ie::GlslCompiler::GlslCompiler(std::string name, std::string vShader, std::string fShader)
{
  compile(name, vShader, fShader);
}

bool ie::GlslCompiler::clear(void)
{
  package.uniforms.clear();
}

ie::ShaderProgramPackage ie::GlslCompiler::compile(std::string name,
                                                std::string vShader,
                                                std::string fShader)
{
  clear();
  ie::GlslFileReader glslReader;
  ie:: GlslSourcePackage vSourcePack = glslReader.read(vShader);
  vertexShaderId = compileShader(vShader, vSourcePack.sourceCode, GL_VERTEX_SHADER);
  ie::GlslSourcePackage fSourcePack = glslReader.read(fShader);
  fragmentShaderId = compileShader(fShader, fSourcePack.sourceCode, GL_FRAGMENT_SHADER);
  linkShaderProgram();
  package.name = name;
  package.programId = programId;
  package.vertexShaderId = vertexShaderId;
  package.fragmentShaderId = fragmentShaderId;
  vSourcePack.uniforms.insert(vSourcePack.uniforms.end(),
                              fSourcePack.uniforms.begin(),
                              fSourcePack.uniforms.end());
  package.uniforms = vSourcePack.uniforms; 
  unsigned int uniformAmount = package.uniforms.size();
  for (int uni = 0; uni < uniformAmount; uni++)
  {
    package.uniforms[uni].location = glGetUniformLocation(package.programId,
                                     package.uniforms[uni].name.c_str());
  }
  return package; 
}

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
    std::cout << "Shader '" << filename << "' Compile Error: " << buffer << std::endl;
    delete [] buffer;
  }
  return shaderId;
}

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

ie::ShaderProgramPackage ie::GlslCompiler::wrapShaderProgramPackage(void) {return package;}
