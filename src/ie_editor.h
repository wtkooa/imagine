#ifndef IE_EDITOR_H
#define IE_EDITOR_H

//___|"ie_editor.h"|____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_asset_manager.h"
#include "ie_communication.h"
#include "ie_log.h"
#include "ie_messages.h"
#include "ie_terrain_editor.h"

namespace ie
{

  class Editor : public Icommunication
  {
    public:
    Editor();

    void reset(void);
    void init(void);
    void quit(void);

    void rxMsg(Imessage*);

    void setLoadDestination(AssetManager*);

    TerrainEditor terrain;

    private:
    AssetManager* manager; 

  };

}

#endif
