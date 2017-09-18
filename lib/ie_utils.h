#pragma once

#include <string>

#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>

//String Split 
std::string split(std::string, char, size_t);

//Texture Loader
GLuint loadTexture(std::string);
