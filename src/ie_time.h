#pragma once

namespace ie
{

  class FrameClock
  {
    public:
    FrameClock();
    void measure(void);
    float getFrameDelta(void);
    float getFps(void);
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
