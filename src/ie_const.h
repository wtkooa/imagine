#ifndef IE_CONST_H
#define IE_CONST_H

//___|"ie_const.h"|_____________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include <glm/glm.hpp>

namespace ie
{
  extern const float MSECS_PER_SEC;
  extern const int DIFFUSE_TEXTURE_BINDING_POINT;
  extern const int LOCK_TO_LOCAL_FRAMERATE;
  extern const int GLSL_VERSION_450;
  extern const int VEC3_DIM;
  extern const int VEC2_DIM;
  extern const int STRIDE_VN_BYTES;
  extern const int STRIDE_VTN_BYTES;
  extern const void* OFFSET_VOID;
  extern const void* OFFSET_1VEC3;
  extern const void* OFFSET_1VEC3_1VEC2;
  extern const glm::vec3 UP_VECTOR;
}

#endif
