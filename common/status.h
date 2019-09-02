#pragma once
#include "common.h"

bool statusInit(void);
void statusExit(void);
bool statusGet(bool *netstatusFlag, AssetId *netstatusAssetId);

