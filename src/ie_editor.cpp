//___|"ie_editor.cpp"|__________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_editor.h"

#include "ie_communication.h"
#include "ie_log.h"
#include "ie_messages.h"

ie::Editor::Editor()
{
  reset();
}


void ie::Editor::reset(void)
{
  manager = NULL;
}


void ie::Editor::init(void)
{
  nexus->addConnection(IE_EDITOR_ICOM_HANDLE, this);
  NexusMsg msg(IE_LOG_ICOM_HANDLE, IE_NEXUS_OUT_CONNECTION_CMD, this);
  nexus->rxMsg(&msg); 

  terrain.connectNexus(nexus);
  terrain.init();
}


void ie::Editor::setLoadDestination(AssetManager* am)
{
  manager = am;
  terrain.setLoadDestination(am);
}


void ie::Editor::rxMsg(Imessage*)
{
  //TODO populate
}


void ie::Editor::quit(void)
{
  terrain.quit();

  //log->info("Editor Shutdown");
}
