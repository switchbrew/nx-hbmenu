#pragma once

#include <switch.h>
#include "../common/common.h"

struct touchInfo_s {
    bool gestureInProgress;
    HidTouchState firstTouch;
    HidTouchState prevTouch;
    bool isTap;
    int initMenuXPos;
    int initMenuIndex;
    int lastSlideSpeed;
};

void touchInit(void);
void handleTouch(menu_s* menu);