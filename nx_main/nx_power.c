#include <switch.h>
#include "../common/common.h"

static bool psmInitialized;

bool powerGetDetails(uint32_t *batteryCharge, bool *isCharging) {
    ChargerType charger = ChargerType_None;
    bool hwReadsSucceeded = false;
    Result rc = 0;

    *isCharging = false;
    *batteryCharge = 0;

    if (psmInitialized)
    {
        rc = psmGetBatteryChargePercentage(batteryCharge);
        hwReadsSucceeded = R_SUCCEEDED(rc);
        rc = psmGetChargerType(&charger);
        hwReadsSucceeded &= R_SUCCEEDED(rc);
        *isCharging = (charger > ChargerType_None);
    }

    return hwReadsSucceeded;
}

void powerInit(void) {
    if (!psmInitialized)
    {
        Result rc = psmInitialize();
        if (R_SUCCEEDED(rc))
        {
            psmInitialized = true;
        }
    }
}

void powerExit(void) {
    if (psmInitialized)
    {
        psmExit();
        psmInitialized = false;
    }
}