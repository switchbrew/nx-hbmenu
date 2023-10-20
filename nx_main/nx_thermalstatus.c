#include "../common/common.h"

static TsSession g_tsInternalSession;

bool thermalstatusInit(void) {
    if (R_FAILED(tsInitialize()))
        return false;

    if (hosversionAtLeast(17,0,0) && R_FAILED(tsOpenSession(&g_tsInternalSession, TsDeviceCode_LocationInternal)))
        return false;

    return true;
}

void thermalstatusExit(void) {
    if (hosversionAtLeast(17,0,0))
        tsSessionClose(&g_tsInternalSession);
    tsExit();
}

bool thermalstatusGetDetails(s32 *temperature) {
    if (hosversionAtLeast(17,0,0)) {
        float temp_float;
        if (R_SUCCEEDED(tsSessionGetTemperature(&g_tsInternalSession, &temp_float))) {
            *temperature = (int)temp_float;
            return true;
        } else {
            return false;
        }
    } else {
        return R_SUCCEEDED(tsGetTemperature(TsLocation_Internal, temperature));
    }
}

