#ifndef IE_IMPORT_H
#define IE_IMPORT_H

//___|"ie_import.h"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <string>

#include "ie_asset_manager.h"
#include "ie_glsl.h"
#include "ie_wavefront.h"

namespace ie
{
  class LoadManager 
  {
    public:
    void setLoadDestination(AssetManager*);

    void quit(void);

    void load(std::string);
    void load(std::string, std::string);
    void load(std::string, std::string, std::string);
    void load(std::string, std::string, std::string,
              std::string, std::string);

    private:
    AssetManager* manager;

    WavefrontLoader obj; 
    GlslLoader glsl; 
  };
}

#endif
