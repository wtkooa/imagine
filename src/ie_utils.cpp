#include "ie_utils.h"

#include <iostream>
#include <string>

#define GL_GLEXT_PROTOTYPES //Needs to be defined for some GL funcs to work.
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL_image.h>

//String Split 
std::string split(std::string str, char delim, size_t tokenNum)
{
  std::string substr; 
  size_t foundtoken = -1;
  size_t findlen = 0;
  size_t findstart = 0; 
  do
  {
    findlen = str.find(delim, findstart) - findstart;
    substr = str.substr(findstart, findlen);
    findstart = str.find(delim, findstart) + 1; 
    foundtoken++;
  }
  while (foundtoken < tokenNum);
  return substr;
}

//Loads textures image to vram; returns texture ID
GLuint loadTexture(std::string filename)
{
  SDL_Surface* texture;
  GLuint textureID;
  int mode;
  texture = IMG_Load(filename.c_str());
  if (!texture) {std::cout << "Warning: Texture '" << filename << "' failed to load..." << std::endl;}
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  if (texture->format->BytesPerPixel == 4) {mode = GL_RGBA;}
  else if (texture->format->BytesPerPixel = 3) {mode = GL_RGB;}
  glTexImage2D(GL_TEXTURE_2D, 0, mode, texture->w, texture->h, 0, mode, GL_UNSIGNED_BYTE, texture->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  SDL_FreeSurface(texture);
  return textureID;
}
