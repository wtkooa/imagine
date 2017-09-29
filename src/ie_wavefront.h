#ifndef IE_WAVEFRONT_H
#define IE_WAVEFRONT_H

//___|"ie_wavefront.h"|_________________________________________________________
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

#include "ie_packages.h"

namespace ie
{

  class WavefrontMaterialFileReader
  {
    public:
    WavefrontMaterialFileReader();
    WavefrontMaterialFileReader(std::string, std::string);
    WavefrontMaterialFilePackage read(std::string, std::string);
    WavefrontMaterialFilePackage wrapFilePackage(void);
    private:
    bool clear(void);
    WavefrontMaterialFilePackage filePackage;
  };

  class WavefrontObjectFileReader
  {
    public:
    WavefrontObjectFileReader();
    WavefrontObjectFileReader(std::string, std::string);
    WavefrontObjectFilePackage read(std::string, std::string);
    WavefrontObjectFilePackage wrapFilePackage(void);
    private:
    bool hasQuads;
    bool clear(void);
    WavefrontObjectFilePackage filePackage;
    WavefrontMaterialFileReader materialReader;
  };

}

#endif
