#ifndef IE_TEXTURE_H
#define IE_TEXTURE_H

//___|"ie_texture.h"|___________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

#include "ie_asset.h"

namespace ie
{

  class Texture : public Asset
  {
    public:
    Texture();
    ~Texture();

    void setMipmapped(bool);
    void setAnisotropy(bool);
    void setRepeating(bool);
    bool getMipmapped(void);
    bool getAnisotropy(void);
    bool getRepeating(void);

    virtual void reset(void);

    private:
    GLuint glId;
    bool mipmapped;
    bool anisotropy;
    bool repeating;
  };

}

#endif
