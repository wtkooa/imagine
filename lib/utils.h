#pragma once
#define GL_GLEXT_PROTOTYPES // Needs to be defined for some GL funcs to work.
#include <map>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include "ie_wavefront.h"



class VRAMManager
{
  public:
  VRAMManager();
  bool genVBO(modelResource*, int);
  bool releaseMem(void);  
  GLuint vboID;
  int vboSizeBytes;
  void* positionStart;
  GLuint positionDim;
  GLuint textureDim;
  void* textureStart;
  GLuint normalDim;
  void* normalStart;
  GLuint vboStride;
};


//Shader
class GLSL_Reader
{
  public:
  GLSL_Reader();
  GLSL_Reader(std::string);
  void setFilename(std::string);
  std::string getFilename(void);
  bool read(void);
  std::string getCode(void);
  private:
  std::string filename;
  std::string code;
};


class GLSL_Compiler
{
  public:
  bool compileVertexShader(std::string);
  bool compileFragmentShader(std::string);
  bool linkShaderProgram(void);
  GLuint getProgramID(void);
  bool cleanUpShaders(void);
  private:
  GLuint vertexShaderID;
  GLuint fragmentShaderID;
  GLuint programID;
};
