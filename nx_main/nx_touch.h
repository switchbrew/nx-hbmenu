#pragma once

#include <switch.h>

struct touchInfo_s {
    touchPosition* firstTouch;
    touchPosition* prevTouch;
    bool isTap;
    int initMenuXPos;
    int initMenuIndex;
};