//___|"ie_texture.cpp"|_________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_texture.h"

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>

ie::Texture::Texture()
{
  reset();
}

ie::Texture::~Texture()
{
  glDeleteTextures(1, &glId);
}

void ie::Texture::reset(void)
{
  glGenTextures(1, &glId);
  mipmapped = true;
  anisotropy = true;
  repeating = true;
}


void ie::Texture::setMipmapped(bool m) {mipmapped = m;}
bool ie::Texture::getMipmapped(void) {return mipmapped;}


void ie::Texture::setAnisotropy(bool a) {anisotropy = a;}
bool ie::Texture::getAnisotropy(void) {return anisotropy;}


void ie::Texture::setRepeating(bool r) {repeating = r;}
bool ie::Texture::getRepeating(void) {return repeating;}
