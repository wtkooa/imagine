//___|"ie_communication.cpp"|___________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//_______________________________________________________________________________

#include "ie_communication.h"

#include "ie_enum.h"

ie::Icommunication::Icommunication()
{
  resetCommunication();
}

ie::Icommunication::~Icommunication()
{
  unsubscribe();
}


void ie::Icommunication::resetCommunication(void)
{
  nexus = NULL;
  subscribers.clear();
}


void ie::Icommunication::connectNexus(Nexus* nexus) {this->nexus = nexus;}


void ie::Icommunication::txMsg(Imessage* msg)
{
  for (auto it = subscribers.begin(); it != subscribers.end(); it++)
  {
    (*it)->rxMsg(msg);
  }
}


void ie::Icommunication::addSubscriber(Icommunication* sub)
{
  subscribers.insert(sub);
}


void ie::Icommunication::removeSubscriber(Icommunication* sub)
{
  subscribers.erase(sub);
}


void ie::Icommunication::addSubscription(Icommunication* sub)
{
  subscriptions.insert(sub);
}


void ie::Icommunication::removeSubscription(Icommunication* sub)
{
  subscriptions.erase(sub);
}


void ie::Icommunication::unsubscribe(void)
{
  for (auto it = subscriptions.begin(); it != subscriptions.end(); it++)
  {
    (*it)->removeSubscriber(this);
  }
  for (auto it = subscribers.begin(); it != subscribers.end(); it++)
  {
    (*it)->removeSubscription(this);
  }
}


void ie::Nexus::addConnection(IEenum handle, Icommunication* icom)
{
  handles[handle] = icom; 
}


void ie::Nexus::removeConnection(IEenum handle)
{
  handles.erase(handle);
}


void ie::Nexus::rxMsg(Imessage* msg)
{
  if (msg-> type == IE_NEXUS_CONNECTION_MSG)
  {
    handleNexusMsg(static_cast<NexusMsg*>(msg));
  }
}


void ie::Nexus::handleNexusMsg(NexusMsg* msg)
{
  Icommunication* target = handles[msg->target]; 
  Icommunication* origin = msg->origin;

  if (msg->command == IE_NEXUS_OUT_CONNECTION_CMD)
  {
    origin->addSubscriber(target);
    target->addSubscription(origin);
  }
  else if (msg->command == IE_NEXUS_IN_CONNECTION_CMD)
  {
    target->addSubscriber(origin);
    origin->addSubscription(target);
  }
  else if (msg->command == IE_NEXUS_BI_CONNECTION_CMD)
  {
    origin->addSubscriber(target);
    target->addSubscription(origin);
    target->addSubscriber(origin);
    origin->addSubscription(target);
  }
}
