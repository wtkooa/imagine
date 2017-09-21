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
    WavefrontMaterialFilePackage read(std::string);
    WavefrontMaterialFilePackage wrapFilePackage(void);
    private:
    bool clear(void);
    WavefrontMaterialFilePackage filePackage;
  };

  class WavefrontObjectFileReader
  {
    public:
    WavefrontObjectFileReader();
    WavefrontObjectFileReader(std::string);
    WavefrontObjectFilePackage read(std::string);
    WavefrontObjectFilePackage wrapFilePackage(void);
    private:
    bool clear(void);
    WavefrontObjectFilePackage filePackage;
    WavefrontMaterialFileReader materialReader;
  };

}
