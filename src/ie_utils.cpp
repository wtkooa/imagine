#include "ie_utils.h"

#include <string>

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
