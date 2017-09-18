#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cmath>
#include <exception>
#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include "ie_wavefront.h"




//Video Memory Manger
VRAMManager::VRAMManager(void)
{
  vboSizeBytes = 0;
  positionStart = (void*)(0);
  positionDim = 3;
  textureStart = (void*)(sizeof(float) * 3);
  textureDim = 3;
  normalStart = (void*)(sizeof(float) * 6);
  normalDim = 3;
  vboStride = sizeof(float) * 9;
  glGenBuffers(1, &vboID);
  glBindBuffer(GL_ARRAY_BUFFER, vboID); 
}

bool VRAMManager::genVBO(modelResource* model, int modelAmount)
{
  int newVBOSizeBytes = 0;
  for (int n = 0; n < modelAmount; n++)
  {
    if (model[n].vboloaded == true || model[n].tobevboloaded == true)
    {
      newVBOSizeBytes += model[n].vertexSizeBytes;
    }
  }
  glDeleteBuffers(1, &vboID);
  glGenBuffers(1, &vboID);
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glBufferData(GL_ARRAY_BUFFER, newVBOSizeBytes, NULL, GL_STATIC_DRAW);
  vboSizeBytes = newVBOSizeBytes;
  size_t vOffset = 0;
  size_t iOffset = 0;
  for (int n = 0; n < modelAmount; n++)
  {
    if (model[n].vboloaded == true || model[n].tobevboloaded == true)
    {
      model[n].vboOffsetBytes = vOffset;
      model[n].vboOffsetIndex = iOffset;
      glBufferSubData(GL_ARRAY_BUFFER,
             vOffset,
              model[n].vertexSizeBytes,
              model[n].vertexData);
      vOffset += (model[n].vertexSizeBytes);
      iOffset += (model[n].vertexAmount);
      model[n].vboloaded = true;
      model[n].tobevboloaded = false;
    } 
  } 
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, positionDim,
              GL_FLOAT, GL_FALSE,
              vboStride, positionStart);
  glVertexAttribPointer(1, textureDim,
              GL_FLOAT, GL_FALSE,
              vboStride, textureStart);
  glVertexAttribPointer(2, normalDim,
              GL_FLOAT, GL_FALSE,
              vboStride, normalStart);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool VRAMManager::releaseMem(void)
{ 
  glDeleteBuffers(1, &vboID);
}


//Shader
GLSL_Reader::GLSL_Reader() {}
GLSL_Reader::GLSL_Reader(std::string name) {filename = name;}

void GLSL_Reader::setFilename(std::string name) {filename = name;}

std::string GLSL_Reader::getFilename(void) {return filename;}

bool GLSL_Reader::read(void)
{
  std::ifstream glslFile;
  glslFile.open(filename.c_str());
  if (glslFile.is_open()) {
    std::stringstream buffer;
    buffer << glslFile.rdbuf();
    code = buffer.str();
    glslFile.close();
    return true;
  } else {
    std::cout << "File " << filename << " failed to open..." << std::endl;
    return false;
  }
}

std::string GLSL_Reader::getCode(void) {return code;}

bool GLSL_Compiler::compileVertexShader(std::string filename)
{
  GLSL_Reader vertexShaderReader(filename);
  vertexShaderReader.read();
  std::string vertexShaderCode = vertexShaderReader.getCode();
  vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  const char* codeParamAdapter[1]; //glShaderSource needs char**
  codeParamAdapter[0] = vertexShaderCode.c_str();
  glShaderSource(vertexShaderID, 1, codeParamAdapter, 0);
  glCompileShader(vertexShaderID);
    GLint compileStatus;
  glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus != GL_TRUE)
  {
    GLint infoLogLength;
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    try 
    {
      GLchar* buffer = new GLchar[infoLogLength];
      GLsizei bufferSize; //Not used: required as func param
      glGetShaderInfoLog(vertexShaderID, infoLogLength, &bufferSize, buffer);
      std::cout << "Vertex Shader Compile Error: " << buffer << std::endl;
      delete [] buffer;
    }
    catch (std::exception& exc) {std::cout << exc.what() << std::endl;}
  }
}

bool GLSL_Compiler::compileFragmentShader(std::string filename)
{
  GLSL_Reader fragmentShaderReader(filename);
  fragmentShaderReader.read();
  std::string fragmentShaderCode = fragmentShaderReader.getCode();
  fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  const char* codeParamAdapter[1]; //glShaderSource needs char**
  codeParamAdapter[0] = fragmentShaderCode.c_str();
  glShaderSource(fragmentShaderID, 1, codeParamAdapter, 0);
  glCompileShader(fragmentShaderID);
  GLint compileStatus;
  glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus != GL_TRUE)
  {
    GLint infoLogLength;
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    try 
    {
      GLchar* buffer = new GLchar[infoLogLength];
      GLsizei bufferSize; //Not used: required as func param
      glGetShaderInfoLog(fragmentShaderID, infoLogLength, &bufferSize, buffer);
      std::cout << "Fragment Shader Compile Error: " << buffer << std::endl;
      delete [] buffer;
    }
    catch (std::exception& exc) {std::cout << exc.what() << std::endl;}
  }
}

bool GLSL_Compiler::linkShaderProgram(void)
{
  programID = glCreateProgram();
  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  glLinkProgram(programID);
  GLint linkStatus;
  glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
  if (linkStatus != GL_TRUE)
  {
    GLint infoLogLength;
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    try
    {
      GLchar* buffer = new GLchar[infoLogLength];
      GLsizei bufferSize; //Not used: required as func param
      glGetProgramInfoLog(programID, infoLogLength, &bufferSize, buffer);
      std::cout << "Shader Program Link Error: " << buffer << std::endl;
      delete [] buffer;
    }
    catch (std::exception& exc) {std::cout << exc.what() << std::endl;}
  }

}

GLuint GLSL_Compiler::getProgramID(void) {return programID;}

bool GLSL_Compiler::cleanUpShaders(void)
{
  glDetachShader(programID, vertexShaderID);
  glDetachShader(programID, fragmentShaderID);
  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);
  glUseProgram(0);
  glDeleteProgram(programID);
}
