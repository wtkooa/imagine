#pragma once

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

#include "ie_messages.h"
#include "ie_packages.h"

namespace ie
{

  class GlslFileReader
  {
    public:
    GlslFileReader();
    GlslFileReader(std::string, std::string);
    GlslSourcePackage read(std::string, std::string);
    GlslSourcePackage wrapGlslSourcePackage(void);
    private:
    bool clear(void);
    std::string filename;
    std::string filepath;
    GlslSourcePackage package;
  };


  class GlslCompiler
  {
    public:
    GlslCompiler();
    GlslCompiler(std::string, std::string, std::string, std::string, std::string);
    ShaderProgramPackage compile(std::string, std::string, std::string, std::string, std::string);
    ShaderProgramPackage wrapShaderProgramPackage(void);
    private:
    std::map<std::string, GlslUniformPackage> findUniforms(GLuint, std::string, std::string);
    bool clear(void);
    GLuint compileShader(std::string, std::string, GLenum);
    bool linkShaderProgram(void);
    ShaderProgramPackage package;
    GLuint vertexShaderId;
    GLuint fragmentShaderId;
    GLuint programId;
  };

}
