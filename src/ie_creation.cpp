//___|"ie_creation.cpp"|________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_creation.h"

ie::CreationManager::CreationManager()
{
  reset();
}


void ie::CreationManager::reset(void)
{
  manager = NULL;
}


void ie::CreationManager::setLoadDestination(AssetManager* am)
{
  manager = am;
  terrainEditor.setLoadDestination(am);
}


void ie::CreationManager::quit(void)
{
  terrainEditor.quit();
}
