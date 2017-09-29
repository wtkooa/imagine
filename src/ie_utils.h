#ifndef IE_UTILS_H
#define IE_UTILS_H

#include <string>

namespace ie
{

  std::string split(std::string, char, size_t);

  std::string removeChar(std::string, char);

  int countTokens(std::string, char);

  std::string popFrontToken(std::string, char);
  std::string popFrontToken(std::string, char, int);

}

#endif
