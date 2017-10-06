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
#include <string>


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
