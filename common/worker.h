#pragma once
#include "common.h"

void workerInit(void);
void workerExit(void);
void workerSchedule(workerThreadFunc func, void* data);
