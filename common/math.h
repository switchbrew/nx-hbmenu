#pragma once

#include "common.h"

float approxSin(float x);

inline float clamp(float x, float min, float max) {
    return x<min ? min : x>max ? max : x;
}

inline float nxfabs(float x) {
    return x < 0 ? -x : x;
}