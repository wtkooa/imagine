#ifndef IE_ENUM_H
#define IE_ENUM_H

//___|"ie_enum.h"|______________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

namespace ie
{
  
  enum IEenum
  {
    IE_NULL_ATTRIB, IE_ELEMENT_ATTRIB, IE_POSITION_ATTRIB,
    IE_NORMAL_ATTRIB, IE_MATERIAL_ATTRIB, IE_COLOR_ATTRIB,
    IE_DATA1_ATTRIB, IE_DATA2_ATTRIB,

    IE_NULL_FORMAT, IE_P_FORMAT, IE_PN_FORMAT,
    IE_PMN_FORMAT, IE_PMNCD_FORMAT, IE_PMNCDD_FORMAT,

    IE_NONE_NODE, IE_PLAYER_NODE, IE_CAMERA_NODE, IE_STATIC_NODE,
    IE_TERRAIN_NODE, IE_WATER_NODE,

    IE_NONE_RENDER, IE_PLAYER_RENDER, IE_CAMERA_RENDER,
    IE_MATERIAL_RENDER, IE_TEXTURE_RENDER, IE_TERRAIN_RENDER,

    IE_NONE_SHADER, IE_STATIC_SHADER, IE_TERRAIN_SHADER,

    IE_FIRST_PERSON_MODE, IE_THIRD_PERSON_MODE,
    IE_THIRD_PERSON_RPG_MODE, IE_FLYAROUND_MODE,
    IE_EDITING_MODE
  };

}

#endif
