//___|"ie_utils.cpp"|___________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_utils.h"

#include <algorithm>
#include <bitset>
#include <string>
#include <vector>

#include <glm/glm.hpp>


std::vector<std::string> ie::split(std::string str, std::string delim)
{
  size_t position = 0;
  std::vector<std::string> tokens;
  while(position != std::string::npos)
  {
    position = str.find(delim);
    tokens.push_back(str.substr(0, position));
    str.erase(0, position + delim.length());
  }
  return tokens;
}
std::string ie::split(std::string str, std::string delim, unsigned int index)
{
  std::vector<std::string> tokens = split(str, delim);
  return tokens[index];
}


std::string ie::concatenateTokens(std::vector<std::string> tokens)
{
  std::string newString;
  for (auto it = tokens.begin(); it != tokens.end(); it++)
  {
    newString += *it;
  }
  return newString;
}

std::vector<std::string> ie::eraseTokens(std::vector<std::string> tokens,
                                         std::string delim)
{
  for (auto it = tokens.begin(); it != tokens.end(); it++)
  {
    if (*it == delim) {tokens.erase(it);}
  }
  return tokens;
}


std::string ie::removeChar(std::string str, char c)
{
  str.erase(std::remove(str.begin(), str.end(), c), str.end());
  return str;
}


int ie::countTokens(std::string line, char delim)
{
  return std::count(line.begin(), line.end(), delim) + 1;
}


int ie::countChar(std::string line, char c)
{
  return std::count(line.begin(), line.end(), c);
}


std::string ie::popFrontToken(std::string line, char delim)
{
  return line.substr(line.find(delim) + 1);
}


std::string ie::popFrontToken(std::string line, char delim, int amount)
{
  for (int n = 0; n < amount; n++)
  {
    return line.substr(line.find(delim) + 1);
  }
}


unsigned int ie::spatialHash(glm::vec3 vector, float gridSize)
{
  glm::ivec3 ivector;
  vector = glm::floor(vector / gridSize);
  ivector.x = (int)vector.x;
  ivector.y = (int)vector.y;
  ivector.z = (int)vector.z;
  ivector.x *= 73856093;
  ivector.y *= 19349663;
  ivector.z *= 83492791;
  return (unsigned int)(ivector.x xor ivector.y xor ivector.z);
}
