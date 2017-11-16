//___|"ie_log.cpp"|_____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_log.h"

#include <ctime>

#include "ie_config.h"
#include "ie_const.h"

ie::Log::Log()
{
  channelOptions = IE_LOG_ALL;

  logFile = fopen(DEFAULT_IE_LOGFILE.c_str(), "w");
  bool failedToOpen = logFile == NULL;

  if (failedToOpen)
  {
    channelOptions = IE_LOG_ALL_STDOUT|IE_LOG_DATE|IE_LOG_TIME;
    warning("Log file %s failed to open for writing", DEFAULT_IE_LOGFILE);
  }
}

ie::Log::~Log()
{
  bool isOpen = logFile != NULL;

  if (isOpen)
  {
    fclose(logFile);
  }
}


void ie::Log::info(const char* fmt, ...)
{
  va_list args;

  bool useDate = IE_LOG_DATE & channelOptions;
  bool useTime = IE_LOG_TIME & channelOptions;
  bool useStdOut = IE_LOG_INFO_STDOUT & channelOptions;
  bool useLogFile = IE_LOG_INFO_LOGFILE & channelOptions;
  
  if (useStdOut)
  {

    time_t currentTime = time(NULL);
    struct tm * calendarTime = localtime(&currentTime);
    if (useDate)
    {
      char formattedDate [50];
      strftime(formattedDate, 50, "%F ", calendarTime);
      printf("%s ", formattedDate);
    }
    if (useTime)
    {
      char formattedTime [50];
      strftime(formattedTime, 50, "%X ", calendarTime);
      printf("%s ", formattedTime);
    }

    printf("INFO: ");

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
    fflush(stdout);
  }

  if (useLogFile)
  {

    time_t currentTime = time(NULL);
    struct tm * calendarTime = localtime(&currentTime);
    if (useDate)
    {
      char formattedDate [50];
      strftime(formattedDate, 50, "%F ", calendarTime);
      fprintf(logFile, "%s ", formattedDate);
    }
    if (useTime)
    {
      char formattedTime [50];
      strftime(formattedTime, 50, "%X ", calendarTime);
      fprintf(logFile, "%s ", formattedTime);
    }

    fprintf(logFile, "INFO: ");

    va_start(args, fmt);
    vfprintf(logFile, fmt, args);
    va_end(args);

    fprintf(logFile, "\n");
    fflush(logFile); 
  }
}


void ie::Log::warning(const char* fmt, ...)
{
  va_list args;

  bool useDate = IE_LOG_DATE & channelOptions;
  bool useTime = IE_LOG_TIME & channelOptions;
  bool useStdOut = IE_LOG_WARNING_STDOUT & channelOptions;
  bool useLogFile = IE_LOG_WARNING_LOGFILE & channelOptions;
  
  if (useStdOut)
  {

    time_t currentTime = time(NULL);
    struct tm * calendarTime = localtime(&currentTime);
    if (useDate)
    {
      char formattedDate [50];
      strftime(formattedDate, 50, "%F ", calendarTime);
      printf("%s ", formattedDate);
    }
    if (useTime)
    {
      char formattedTime [50];
      strftime(formattedTime, 50, "%X ", calendarTime);
      printf("%s ", formattedTime);
    }

    printf("WARNING: ");

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
    fflush(stdout);

  }

  if (useLogFile)
  {

    time_t currentTime = time(NULL);
    struct tm * calendarTime = localtime(&currentTime);
    if (useDate)
    {
      char formattedDate [50];
      strftime(formattedDate, 50, "%F ", calendarTime);
      fprintf(logFile, "%s ", formattedDate);
    }
    if (useTime)
    {
      char formattedTime [50];
      strftime(formattedTime, 50, "%X ", calendarTime);
      fprintf(logFile, "%s ", formattedTime);
    }

    fprintf(logFile, "WARNING: ");

    va_start(args, fmt);
    vfprintf(logFile, fmt, args);
    va_end(args);

    fprintf(logFile, "\n");
    fflush(logFile);
    
  }
}


void ie::Log::debug(const char* fmt, ...)
{
  va_list args;

  bool useDate = IE_LOG_DATE & channelOptions;
  bool useTime = IE_LOG_TIME & channelOptions;
  bool useStdOut = IE_LOG_DEBUG_STDOUT & channelOptions;
  bool useLogFile = IE_LOG_DEBUG_LOGFILE & channelOptions;
  
  if (useStdOut)
  {

    time_t currentTime = time(NULL);
    struct tm * calendarTime = localtime(&currentTime);
    if (useDate)
    {
      char formattedDate [50];
      strftime(formattedDate, 50, "%F ", calendarTime);
      printf("%s ", formattedDate);
    }
    if (useTime)
    {
      char formattedTime [50];
      strftime(formattedTime, 50, "%X ", calendarTime);
      printf("%s ", formattedTime);
    }

    printf("DEBUG: ");

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
    fflush(stdout);

  }

  if (useLogFile)
  {

    time_t currentTime = time(NULL);
    struct tm * calendarTime = localtime(&currentTime);
    if (useDate)
    {
      char formattedDate [50];
      strftime(formattedDate, 50, "%F ", calendarTime);
      fprintf(logFile, "%s ", formattedDate);
    }
    if (useTime)
    {
      char formattedTime [50];
      strftime(formattedTime, 50, "%X ", calendarTime);
      fprintf(logFile, "%s ", formattedTime);
    }

    fprintf(logFile, "DEBUG: ");

    va_start(args, fmt);
    vfprintf(logFile, fmt, args);
    va_end(args);

    fprintf(logFile, "\n");
    fflush(logFile);
    
  }
}


void ie::Log::exception(const char* fmt, ...)
{
  va_list args;

  bool useDate = IE_LOG_DATE & channelOptions;
  bool useTime = IE_LOG_TIME & channelOptions;
  bool useStdOut = IE_LOG_EXCEPTION_STDOUT & channelOptions;
  bool useLogFile = IE_LOG_EXCEPTION_LOGFILE & channelOptions;
  
  if (useStdOut)
  {

    time_t currentTime = time(NULL);
    struct tm * calendarTime = localtime(&currentTime);
    if (useDate)
    {
      char formattedDate [50];
      strftime(formattedDate, 50, "%F ", calendarTime);
      printf("%s ", formattedDate);
    }
    if (useTime)
    {
      char formattedTime [50];
      strftime(formattedTime, 50, "%X ", calendarTime);
      printf("%s ", formattedTime);
    }

    printf("EXCEPTION: ");

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
    fflush(stdout);

  }

  if (useLogFile)
  {

    time_t currentTime = time(NULL);
    struct tm * calendarTime = localtime(&currentTime);
    if (useDate)
    {
      char formattedDate [50];
      strftime(formattedDate, 50, "%F ", calendarTime);
      fprintf(logFile, "%s ", formattedDate);
    }
    if (useTime)
    {
      char formattedTime [50];
      strftime(formattedTime, 50, "%X ", calendarTime);
      fprintf(logFile, "%s ", formattedTime);
    }

    fprintf(logFile, "EXCEPTION: ");

    va_start(args, fmt);
    vfprintf(logFile, fmt, args);
    va_end(args);

    fprintf(logFile, "\n");
    fflush(logFile);
    
  }
}


void ie::Log::assertion(std::string msg, bool test)
{

  bool useDate = IE_LOG_DATE & channelOptions;
  bool useTime = IE_LOG_TIME & channelOptions;
  bool useStdOut = IE_LOG_EXCEPTION_STDOUT & channelOptions;
  bool useLogFile = IE_LOG_EXCEPTION_LOGFILE & channelOptions;
  
  if (useStdOut)
  {

    time_t currentTime = time(NULL);
    struct tm * calendarTime = localtime(&currentTime);
    if (useDate)
    {
      char formattedDate [50];
      strftime(formattedDate, 50, "%F ", calendarTime);
      printf("%s ", formattedDate);
    }
    if (useTime)
    {
      char formattedTime [50];
      strftime(formattedTime, 50, "%X ", calendarTime);
      printf("%s ", formattedTime);
    }

    printf("ASSERTION: %s: ", msg.c_str());

    if (test) {printf("SUCCESSFUL");}
    else {printf("FAILURE");}

    printf("\n");
    fflush(stdout);

  }

  if (useLogFile)
  {

    time_t currentTime = time(NULL);
    struct tm * calendarTime = localtime(&currentTime);
    if (useDate)
    {
      char formattedDate [50];
      strftime(formattedDate, 50, "%F ", calendarTime);
      fprintf(logFile, "%s ", formattedDate);
    }
    if (useTime)
    {
      char formattedTime [50];
      strftime(formattedTime, 50, "%X ", calendarTime);
      fprintf(logFile, "%s ", formattedTime);
    }

    fprintf(logFile, "ASSERTION: %s: ", msg.c_str());

    if (test) {fprintf(logFile, "SUCCESSFUL");}
    else {fprintf(logFile, "FAILURE");}

    fprintf(logFile, "\n");
    fflush(logFile);
    
  }
}
