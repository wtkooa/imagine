//___|"ie_file_system.cpp"|_____________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_file_system.h"

#include <string>
#include <vector>

#include "ie_utils.h"

ie::File::File() {}

ie::File::File(std::string name)
{
  parseFilename(name);
}

ie::File::File(std::string path, std::string name)
{
  parseFilename(name);
  this->path = path;
}


void ie::File::parseFilename(std::string name)
{
  std::vector<std::string> tokens = split(name, "/");
  std::vector<std::string> subTokens = split(tokens[tokens.size() - 1], ".");
  if (subTokens.size() == 1) {name = tokens[0];}
  else if (tokens.size() == 2)
  {
    name = tokens[0];
    extension = tokens[1];
  }
  else
  {
    //TODO Include message to log
  }
}


void ie::File::setPath(std::string path) {this->path = path;}
void ie::File::setName(std::string name) {this->name = name;}
void ie::File::setExtension(std::string extension) {this->extension = extension;}

std::string ie::File::getPath(void) {return path;}
std::string ie::File::getName(void) {return name;}
std::string ie::File::getExtension(void) {return extension;}
std::string ie::File::getString(void) {return path + name +extension;}
const char* ie::File::getCString(void) {return (path + name +extension).c_str();}
