#ifndef IE_TIME_H
#define IE_TIME_H

//___|"ie_time.h"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_messages.h"

namespace ie
{

  class FrameClock
  {
    public:
    FrameClock();
    void measure(void);
    float getFrameDelta(void);
    float getFps(void);
    ie::TimeStatusMessage sendTimeStatusMessage(void);
    private:
    unsigned long frameStart;
    unsigned long frameEnd;
    float frameDelta;     
    float fps;
  };


  class Stopwatch
  {
    public:
    void start(void);
    void stop(void);  
    void readOut(void);
    private:
    unsigned long startTime;
    unsigned long stopTime;
    float timeDelta; 
  };

}

#endif
