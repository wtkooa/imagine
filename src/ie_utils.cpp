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

#include <glm/glm.hpp>


std::string ie::split(std::string str, char delim, size_t tokenNum)
{
  std::string subStr; 
  size_t foundToken = -1;
  size_t findLen = 0;
  size_t findStart = 0; 
  do
  {
    findLen = str.find(delim, findStart) - findStart;
    subStr = str.substr(findStart, findLen);
    findStart = str.find(delim, findStart) + 1; 
    foundToken++;
  }
  while (foundToken < tokenNum);
  return subStr;
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


unsigned int ie::packNormalAttrib(glm::vec3 normal)
{
  normal = glm::normalize(normal);
  std::bitset<10> x = int(normal.x * 511);
  std::bitset<10> y = int(normal.y * 511);
  std::bitset<10> z = int(normal.z * 511);
  std::bitset<32> i2310;
  i2310[30] = true;
  for (int n = 0; n < x.size(); n++) {i2310[n+20] = x[n];}
  for (int n = 0; n < x.size(); n++) {i2310[n+10] = y[n];}
  for (int n = 0; n < x.size(); n++) {i2310[n] = z[n];}
  return (unsigned int)i2310.to_ulong();
}


glm::vec3 ie::unpackNormalAttrib(unsigned int uinormal)
{
  glm::vec3 normal;
  std::bitset<32> i2310 = uinormal;
  std::bitset<10> x;
  std::bitset<10> y;
  std::bitset<10> z;
  for (int n = 0; n < x.size(); n++) {x[n] = i2310[n+20];}
  for (int n = 0; n < x.size(); n++) {y[n] = i2310[n+10];}
  for (int n = 0; n < x.size(); n++) {z[n] = i2310[n];}
  if (x[9] == false)
  {
    normal.x = (float)x.to_ulong();
  }
  else
  {
    int i = x.to_ulong();
    i *= -1;
    i += 2;
    normal.x = (float)i;
  }
  if (y[9] == false)
  {
    normal.y = (float)y.to_ulong();
  }
  else
  {
    int i = y.to_ulong();
    i *= -1;
    i += 2;
    normal.y = (float)i;
  }
  if (z[9] == false)
  {
    normal.z = (float)z.to_ulong();
  }
  else
  {
    int i = z.to_ulong();
    i *= -1;
    i += 2;
    normal.z = (float)i;
  }
  return glm::normalize(normal);
}

glm::u8vec4 ie::packColorAttrib(glm::vec3 c)
{
  glm::u8vec4 color;
  color.x = (unsigned int)(c.x * 255);
  color.y = (unsigned int)(c.y * 255);
  color.z = (unsigned int)(c.z * 255);
  color.w = 255;
  return color;
}

glm::vec3 ie::unpackColorAttrib(glm::u8vec4 c)
{
  glm::vec3 color;
  color.x = float(c.x) / 255;;
  color.y = float(c.y) / 255;;
  color.z = float(c.z) / 255;;
  return color;
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
