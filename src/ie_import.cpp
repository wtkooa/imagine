//___|"ie_import.cpp"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_import.h"

#include <iostream>
#include <string>
#include <vector>

#include "ie_asset_manager.h"
#include "ie_config.h"
#include "ie_utils.h"
#include "ie_wavefront.h"

void ie::LoadManager::setLoadDestination(AssetManager* am)
{
  manager = am;
  obj.setLoadDestination(am);
}


void ie::LoadManager::load(std::string filename)
{
  load("", filename);
}

void ie::LoadManager::load(std::string filepath, std::string filename)
{
  std::vector<std::string> tokens = split(filename, ".");
  std::string extention = tokens.back();

  bool isPathless = filepath == "";
  bool isWavefront = (extention == "obj") || (extention == "mtl");

  if (isWavefront)
  {
    if (isPathless) {obj.read(filename);}
    else {obj.read(filepath, filename);}
  }
  else
  {
    std::cout << "Warning: Unrecognized file format (" << filename << ")." << std::endl;
  }
}


void ie::LoadManager::quit(void)
{
  obj.quit();
}
