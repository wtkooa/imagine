#ifndef IE_MESSAGES_H
#define IE_MESSAGES_H

//___|"ie_messages.h"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//_______________________________________________________________________________

#include <cstdarg>
#include <string>

#include "ie_config.h"
#include "ie_enum.h"

namespace ie
{

  class Icommunication;

  class Imessage
  {
    public:
    IEenum type;
  };

  class NexusMsg : public Imessage
  {
    public:
    NexusMsg(IEenum, IEenum, Icommunication*);

    IEenum target;
    IEenum command;
    Icommunication* origin;
  };

  class LogMsg : public Imessage
  {
    public:
    LogMsg(IEenum);

    IEenum command;
    char str[4096];
    bool assert;
    void fString(const char*, ...); 
  };

  class SystemMsg : public Imessage
  {
    public:
    SystemMsg(IEenum); 
    
    IEenum command;
  };

}

#endif
