#pragma once

#include <switch.h>
#include "../common/common.h"

struct touchInfo_s {
    bool gestureInProgress;
    touchPosition firstTouch;
    touchPosition prevTouch;
    bool isTap;
    int initMenuXPos;
    int initMenuIndex;
    int lastSlideSpeed;
};

void handleTouch(menu_s* menu);