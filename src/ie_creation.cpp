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

#include "ie_log.h"

ie::CreationManager::CreationManager()
{
  reset();
}


void ie::CreationManager::reset(void)
{
  manager = NULL;
}


void ie::CreationManager::setLog(Log* l)
{
  log = l;
  terrainEditor.setLog(log);
}


void ie::CreationManager::setLoadDestination(AssetManager* am)
{
  manager = am;
  terrainEditor.setLoadDestination(am);
}


void ie::CreationManager::quit(void)
{
  terrainEditor.quit();

  log->info("Creation Manager Shutdown");
}
