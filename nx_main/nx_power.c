#include <switch.h>
#include "../common/common.h"

static bool powerInitialized;
static bool powerCacheInitialized;
static uint32_t powerCacheCharge;
static bool powerCacheIsCharging;
static PsmSession powerSession;

bool powerGetDetails(uint32_t *batteryCharge, bool *isCharging) {
    ChargerType charger = ChargerType_None;
    bool hwReadsSucceeded = false;
    bool use_cache = false;
    Result rc = 0;

    *isCharging = false;
    *batteryCharge = 0;

    if (powerInitialized) {
        if (powerCacheInitialized) {
            rc = psmWaitStateChangeEvent(&powerSession, 0);

            if (R_FAILED(rc)) use_cache = true;
        }

        rc = psmGetBatteryChargePercentage(batteryCharge);
        hwReadsSucceeded = R_SUCCEEDED(rc);
        if (use_cache) {
            *isCharging = powerCacheIsCharging;
        }
        else {
            rc = psmGetChargerType(&charger);
            hwReadsSucceeded &= R_SUCCEEDED(rc);
            *isCharging = (charger > ChargerType_None);
        }

        powerCacheCharge = *batteryCharge;
        powerCacheIsCharging = *isCharging;
        powerCacheInitialized = true;
    }

    return hwReadsSucceeded;
}

void powerInit(void) {
    uint32_t charge=0;
    bool isCharging=0;

    powerCacheInitialized = false;
    powerCacheCharge = 0;
    powerCacheIsCharging = false;

    if (!powerInitialized) {
        Result rc = psmInitialize();
        if (R_SUCCEEDED(rc)) {
            rc = psmBindStateChangeEvent(&powerSession, 1, 1, 1);

            if (R_FAILED(rc)) psmExit();
            if (R_SUCCEEDED(rc)) {
                powerInitialized = true;
                powerGetDetails(&charge, &isCharging);//Init the cache.
            }
        }
    }
}

void powerExit(void) {
    if (powerInitialized) {
        psmUnbindStateChangeEvent(&powerSession);
        psmExit();
        powerInitialized = false;
        powerCacheInitialized = false;
    }
}
