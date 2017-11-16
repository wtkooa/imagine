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

#include <glm/vec3.hpp>

namespace ie
{
  extern const float MSECS_PER_SEC;
  extern const short DIFFUSE_TEXTURE_BINDING_POINT;
  extern const short LOCK_TO_LOCAL_FRAMERATE;
  extern const short GLSL_VERSION_450;
  extern const short VEC2_DIM;
  extern const short UVEC2_DIM; 
  extern const short VEC3_DIM;
  extern const short STRIDE_VN_BYTES;
  extern const short STRIDE_VTN_BYTES;
  extern const short STRIDE_VTNCB_BYTES;
  extern const void* OFFSET_VOID;
  extern const void* OFFSET_1VEC3;
  extern const void* OFFSET_1VEC3_1VEC2;
  extern const void* OFFSET_2VEC3_1VEC2;
  extern const void* OFFSET_3VEC3_1VEC2;
  extern const glm::vec3 UP_VECTOR;


  //BIT FLAGS
  extern const unsigned int IE_LOG_NONE;
  extern const unsigned int IE_LOG_INFO_STDOUT;
  extern const unsigned int IE_LOG_INFO_LOGFILE;
  extern const unsigned int IE_LOG_WARNING_STDOUT;
  extern const unsigned int IE_LOG_WARNING_LOGFILE;
  extern const unsigned int IE_LOG_DEBUG_STDOUT;
  extern const unsigned int IE_LOG_DEBUG_LOGFILE;
  extern const unsigned int IE_LOG_ASSERTION_STDOUT;
  extern const unsigned int IE_LOG_ASSERTION_LOGFILE;
  extern const unsigned int IE_LOG_EXCEPTION_STDOUT;
  extern const unsigned int IE_LOG_EXCEPTION_LOGFILE;
  extern const unsigned int IE_LOG_DATE; 
  extern const unsigned int IE_LOG_TIME;
  extern const unsigned int IE_LOG_ALL_STDOUT;
  extern const unsigned int IE_LOG_ALL_LOGFILE;
  extern const unsigned int IE_LOG_ALL;
}

#endif
