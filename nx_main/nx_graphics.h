#pragma once

#include <switch.h>

void graphicsInit(u32 width, u32 height);
void graphicsExit(void);

void* graphicsFrameBegin(u32* out_stride);
void graphicsFrameEnd(void);
