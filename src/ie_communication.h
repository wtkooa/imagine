#ifndef IE_COMMUNICATION_H
#define IE_COMMUNICATION_H

//___|"ie_communication.h"|_____________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//_______________________________________________________________________________

#include <map>
#include <set>

#include "ie_enum.h"
#include "ie_messages.h"


namespace ie
{

  //FORWARD DECLARATION
  class Nexus;
  class Icommunication;


  class Icommunication
  {
    public:
    Icommunication();
    virtual ~Icommunication();

    void resetCommunication(void);

    void connectNexus(Nexus*);

    void txMsg(Imessage*);
    virtual void rxMsg(Imessage*) = 0;

    void addSubscriber(Icommunication*);
    void removeSubscriber(Icommunication*);

    void addSubscription(Icommunication*);
    void removeSubscription(Icommunication*);

    protected:
    Nexus* nexus;
    std::set<Icommunication*> subscribers;
    std::set<Icommunication*> subscriptions;

    void unsubscribe(void);
  };


  class Nexus : public Icommunication
  {
    public:

    void addConnection(IEenum, Icommunication*);
    void removeConnection(IEenum);

    void rxMsg(Imessage*);
    
    private:
    std::map<IEenum, Icommunication*> handles;

    void handleNexusMsg(NexusMsg*);

  };

}

#endif
