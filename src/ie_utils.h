#ifndef IE_UTILS_H
#define IE_UTILS_H

//___|"ie_utils.h"|_____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <string>

#include <glm/gtc/type_precision.hpp>
#include <glm/vec3.hpp>

namespace ie
{

  std::string split(std::string, char, size_t);

  std::string removeChar(std::string, char);

  int countTokens(std::string, char);
  int countChar(std::string, char);

  std::string popFrontToken(std::string, char);
  std::string popFrontToken(std::string, char, int);

  unsigned int packNormalAttrib(glm::vec3);
  glm::vec3 unpackNormalAttrib(unsigned int);

  glm::u8vec4 packColorAttrib(glm::vec3);
  glm::vec3 unpackColorAttrib(glm::u8vec4);

  unsigned int spatialHash(glm::vec3, float);
}

#endif
