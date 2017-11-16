#ifndef IE_LOG_H
#define IE_LOG_H

//___|"ie_log.h"|_______________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <cstdarg>
#include <string>
#include <fstream>

#include "ie_config.h"
#include "ie_const.h"

namespace ie
{

  class Log
  {
    public:
    Log();
    ~Log();

    void setChannels(unsigned int);
    unsigned int getChannels(void);

    void info(const char*, ...);
    void warning(const char*, ...);
    void debug(const char*, ...);
    void assertion(std::string, bool);
    void exception(const char*, ...);


    private:
    FILE* logFile;

    unsigned int channelOptions; 
  };

}

#endif
