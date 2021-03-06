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
