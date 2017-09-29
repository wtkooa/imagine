#include "ie_utils.h"

#include <algorithm>
#include <string>

//String Split 
std::string ie::split(std::string str, char delim, size_t tokenNum)
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

std::string ie::removeChar(std::string str, char c)
{
  str.erase(std::remove(str.begin(), str.end(), c), str.end());
  return str;
}

int ie::countTokens(std::string line, char delim)
{
  return std::count(line.begin(), line.end(), delim) + 1;
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
