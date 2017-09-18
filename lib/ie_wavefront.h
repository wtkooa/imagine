#pragma once

#include <string>
#include <vector>

#include "ie_packages.h"

namespace ie
{

  class WavefrontMaterialFileReader
  {
    public:
    WavefrontMaterialFileReader();
    WavefrontMaterialFileReader(std::string);
    bool read(std::string);
    WavefrontMaterialFilePackage getFilePackage(void);
    private:
    bool clear(void);
    WavefrontMaterialFilePackage filePackage;
  };

  class WavefrontObjectFileReader
  {
    public:
    WavefrontObjectFileReader();
    WavefrontObjectFileReader(std::string);
    bool read(std::string);
    WavefrontObjectFilePackage getFilePackage(void);
    private:
    bool clear(void);
    WavefrontObjectFilePackage filePackage;
    WavefrontMaterialFileReader materialReader;
  };

}
