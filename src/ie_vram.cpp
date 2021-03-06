//___|"ie_vram.cpp"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_vram.h"

#include <algorithm>

//___|VBO ID PAIR IMPLEMENTATION|_______________________________________________

void ie::VboPair::genBuffers(void)
{
  glGenBuffers(1, &readVbo);
  glGenBuffers(1, &writeVbo);
}


void ie::VboPair::swap(void)
{
  std::swap(readVbo, writeVbo);
}


void ie::VboPair::release(void)
{
  glDeleteBuffers(1, &readVbo);
  glDeleteBuffers(1, &writeVbo);
}

//______________________________________________________________________________
