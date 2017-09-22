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

