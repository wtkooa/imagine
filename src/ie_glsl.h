#ifndef IE_GLSL_H
#define IE_GLSL_H

//___|"ie_glsl.h"|______________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <vector>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

#include "ie_asset_manager.h"
#include "ie_log.h"
#include "ie_shader.h"

namespace ie
{

  class GlslLoader
  {
    public:
    GlslLoader();

    void reset(void);

    void load(std::string, std::string, std::string);
    void load(std::string, std::string, std::string,
              std::string, std::string);

    void setLog(Log*);
    void setLoadDestination(AssetManager*);

    private:
    Log* log;
    AssetManager* manager;

    void createProgram(std::string, std::vector<std::string>);
    std::string read(std::string);
    GLuint compileShader(GLenum, std::string, std::string);
    bool linkShaders(Shader*, std::vector<GLuint>);
    void loadProgram(Shader*);

  };

}

#endif
