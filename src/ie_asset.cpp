//___|"ie_asset.cpp"|___________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_asset.h"

#include <string>

#include "ie_log.h"

ie::Asset::Asset()
{
  reset();
}

void ie::Asset::reset(void)
{
  log = NULL;
  assetId = 0;
  name = "none";
  filename = "none";
  filepath = "none";
}

//GETTERS AND SETTERS
void ie::Asset::setLog(Log* l) {log = l;}

void ie::Asset::setAssetId(unsigned int id) {assetId = id;}
void ie::Asset::setName(std::string n) {name = n;}

unsigned int ie::Asset::getAssetId(void) {return assetId;}
std::string ie::Asset::getName(void) {return name;}

void ie::Asset::setFilename(std::string fn) {filename = fn;}
std::string ie::Asset::getFilename(void) {return filename;}


void ie::Asset::setFilepath(std::string fp) {filename = fp;}
std::string ie::Asset::getFilepath(void) {return filepath;}
