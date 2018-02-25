#pragma once

#include "common.h"

float approxSin(float x);

inline float clamp(float x, float min, float max) {
    return fmin(fmax(x, min), max);
}