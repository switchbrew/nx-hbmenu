#pragma once
#include "common.h"

bool thermalstatusInit(void);
void thermalstatusExit(void);
bool thermalstatusGetDetails(s32 *temperature);

