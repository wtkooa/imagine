#include "ie_time.h"

#include <iostream>

#include <SDL2/SDL.h>

#include "ie_const.h"

//FrameClock
ie::FrameClock::FrameClock(void)
{
  frameStart = SDL_GetPerformanceCounter();
  frameDelta = 1;
  fps = 0;
  frameEnd = SDL_GetPerformanceCounter();
}

void ie::FrameClock::measure(void)
{
  frameEnd = SDL_GetPerformanceCounter();
  frameDelta = ((frameEnd - frameStart) * ie::MSECS_PER_SEC) / SDL_GetPerformanceFrequency();
  frameStart = SDL_GetPerformanceCounter();
  fps = ie::MSECS_PER_SEC / frameDelta;
}

float ie::FrameClock::getFrameDelta(void) {return frameDelta;}
float ie::FrameClock::getFps(void) {return fps;}


//Stopwatch
void ie::Stopwatch::start(void) {startTime = SDL_GetPerformanceCounter();}

void ie::Stopwatch::stop(void)
{
  stopTime = SDL_GetPerformanceCounter();
  timeDelta = ((stopTime - startTime) * ie::MSECS_PER_SEC) / SDL_GetPerformanceFrequency();
}

void ie::Stopwatch::readOut(void)
{
  std::cout << timeDelta << " msec" << std::endl;
}