//___|"ie_const.cpp"|___________________________________________________________
//
// Project: Imagine: 3D Environment Engine
// Version: 0.1.0
// Author: David Lipps
// License: MIT License
//
// Copyright (c) 2017 David E Lipps
//______________________________________________________________________________

#include "ie_const.h"

#include <glm/vec3.hpp>

const float ie::MSECS_PER_SEC = 1000.0;
const short ie::DIFFUSE_TEXTURE_BINDING_POINT = 0;
const short ie::LOCK_TO_LOCAL_FRAMERATE = 1;
const short ie::GLSL_VERSION_450 = 450;
const short ie::VEC2_DIM = 2;
const short ie::UVEC2_DIM = 2;
const short ie::VEC3_DIM = 3;
const short ie::STRIDE_VN_BYTES = sizeof(float) * 6;
const short ie::STRIDE_VTN_BYTES = sizeof(float) * 8;
const short ie::STRIDE_VTNCB_BYTES = (sizeof(float) * 11) +
                                   (sizeof(unsigned int) * 2);
const void* ie::OFFSET_VOID = (void*)(0);
const void* ie::OFFSET_1VEC3 = (void*)(sizeof(float) * 3);
const void* ie::OFFSET_1VEC3_1VEC2 = (void*)(sizeof(float) * 5);
const void* ie::OFFSET_2VEC3_1VEC2 = (void*)(sizeof(float) * 8);
const void* ie::OFFSET_3VEC3_1VEC2 = (void*)(sizeof(float) * 11);
const glm::vec3 ie::UP_VECTOR = glm::vec3(0.0f, 1.0f, 0.0f);

//BIT FLAGS
const unsigned int ie::IE_LOG_NONE = 0;
const unsigned int ie::IE_LOG_INFO_STDOUT = 0x1;
const unsigned int ie::IE_LOG_INFO_LOGFILE = 0x2;
const unsigned int ie::IE_LOG_WARNING_STDOUT = 0x4;
const unsigned int ie::IE_LOG_WARNING_LOGFILE = 0x8;
const unsigned int ie::IE_LOG_DEBUG_STDOUT = 0x10;
const unsigned int ie::IE_LOG_DEBUG_LOGFILE = 0x20;
const unsigned int ie::IE_LOG_ASSERTION_STDOUT = 0x40;
const unsigned int ie::IE_LOG_ASSERTION_LOGFILE = 0x81;
const unsigned int ie::IE_LOG_EXCEPTION_STDOUT = 0x100;
const unsigned int ie::IE_LOG_EXCEPTION_LOGFILE = 0x200;
const unsigned int ie::IE_LOG_DATE = 0x400; 
const unsigned int ie::IE_LOG_TIME = 0x800;
const unsigned int ie::IE_LOG_ALL_STDOUT = 0x155;
const unsigned int ie::IE_LOG_ALL_LOGFILE = 0x2AA;
const unsigned int ie::IE_LOG_ALL = 0xFFF;
