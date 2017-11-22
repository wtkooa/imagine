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

#include "ie_communication.h"
#include "ie_config.h"
#include "ie_const.h"
#include "ie_enum.h"
#include "ie_messages.h"

ie::Log::Log()
{
  channelOptions = IE_LOG_ALL;

  logFile = fopen(DEFAULT_IE_LOGFILE.c_str(), "w");
  bool failedToOpen = logFile == NULL;

  if (failedToOpen)
  {
    channelOptions = IE_LOG_ALL_STDOUT|IE_LOG_DATE|IE_LOG_TIME;
    LogMsg warning(IE_LOG_WARNING_CMD);
    warning.fString("Log file '%s' failed to open for writing", DEFAULT_IE_LOGFILE);
    handleLogMsg(&warning);
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


void ie::Log::init(void)
{
  nexus->addConnection(IE_LOG_ICOM_HANDLE, this);
  NexusMsg msg(IE_ENGINE_ICOM_HANDLE, IE_NEXUS_IN_CONNECTION_CMD, this);
  nexus->rxMsg(&msg);
}


void ie::Log::info(LogMsg* msg)
{
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

    printf("%s", msg->str);

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

    fprintf(logFile, "%s", msg->str);

    fprintf(logFile, "\n");
    fflush(logFile); 
  }
}


void ie::Log::warning(LogMsg* msg)
{
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

    printf("%s", msg->str);

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

    fprintf(logFile, "%s", msg->str);

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


void ie::Log::rxMsg(Imessage* msg)
{
  switch (msg->type)
  {
    case IE_LOG_MSG:
      handleLogMsg(static_cast<LogMsg*>(msg));
    break;
    
    case IE_SYSTEM_MSG:
      handleSystemMsg(static_cast<SystemMsg*>(msg));
    break;
  }
}


void ie::Log::handleLogMsg(LogMsg* msg)
{
  switch (msg->command)
  {
    case IE_LOG_INFO_CMD:
      info(msg);
    break;

    case IE_LOG_WARNING_CMD:
      warning(msg); 
    break;
  }
}


void ie::Log::handleSystemMsg(SystemMsg* msg)
{
  LogMsg sysInfo(IE_LOG_INFO_CMD);
  switch (msg->command)
  {
    case IE_SYSTEM_SHUTDOWN_CMD:
     sysInfo.fString("Engine System Shutdown Triggered");
     info(&sysInfo);
    break;
  }
}
