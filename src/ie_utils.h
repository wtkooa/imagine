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
#include <vector>

#include <glm/gtc/type_precision.hpp>
#include <glm/vec3.hpp>

namespace ie
{
  std::vector<std::string> split(std::string, std::string);
  std::string split(std::string, std::string, unsigned int);

  std::string concatenateTokens(std::vector<std::string>);

  std::vector<std::string> eraseTokens(std::vector<std::string>, std::string);

  std::string removeChar(std::string, char);

  int countTokens(std::string, char);
  int countChar(std::string, char);

  std::string popFrontToken(std::string, char);
  std::string popFrontToken(std::string, char, int);

  unsigned int spatialHash(glm::vec3, float);
}

#endif
