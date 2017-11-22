//___|"ie_messages.cpp"|________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//_______________________________________________________________________________

#include "ie_messages.h"

#include <cstdarg>
#include <stdio.h>

#include "ie_enum.h"

ie::NexusMsg::NexusMsg(IEenum target, IEenum command, Icommunication* origin)
{
  type = IE_NEXUS_CONNECTION_MSG;
  this->target = target;
  this->command = command;
  this->origin = origin;
}


ie::LogMsg::LogMsg(IEenum command)
{
  type = IE_LOG_MSG; 
  this->command = command;
}

void ie::LogMsg::fString(const char* fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  vsprintf(str, fmt, args);
  va_end(args);
}


ie::SystemMsg::SystemMsg(IEenum command)
{
  type = IE_SYSTEM_MSG;
  this->command = command;
}
