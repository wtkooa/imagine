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

#include "ie_communication.h"
#include "ie_config.h"
#include "ie_const.h"
#include "ie_messages.h"

namespace ie
{

  class Log : public Icommunication
  {
    public:
    Log();
    ~Log();

    void init(void);

    void setChannels(unsigned int);
    unsigned int getChannels(void);

    void info(LogMsg*);
    void warning(LogMsg*);
    void debug(const char*, ...);
    void assertion(std::string, bool);
    void exception(const char*, ...);

    void rxMsg(Imessage*);

    private:
    FILE* logFile;
    unsigned int channelOptions; 

    void handleLogMsg(LogMsg*);
    void handleSystemMsg(SystemMsg*);

  };

}

#endif
