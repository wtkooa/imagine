#include "ie_const.h"

#include <glm/glm.hpp>

const float ie::MSECS_PER_SEC = 1000.0;
const int ie::DIFFUSE_TEXTURE_BINDING_POINT = 0;
const int ie::LOCK_TO_LOCAL_FRAMERATE = 1;
const int ie::VEC3_DIM = 3;
const int ie::VEC2_DIM = 2;
const int ie::STRIDE_VTN_BYTES = sizeof(float) * 8;
const void* ie::OFFSET_VOID = (void*)(0);
const void* ie::OFFSET_1VEC3 = (void*)(sizeof(float) * 3);
const void* ie::OFFSET_1VEC3_1VEC2 = (void*)(sizeof(float) * 6);
