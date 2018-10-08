#include <switch.h>
#include "../common/common.h"

static bool psmInitialized;


void powerGetDetails(uint32_t *batteryCharge, bool *isCharging) {
    ChargerType charger = ChargerType_None;

    *isCharging = false;
    *batteryCharge = 0;

    if (psmInitialized)
    {
        psmGetBatteryChargePercentage(batteryCharge);
        psmGetChargerType(&charger);
        *isCharging = (charger > ChargerType_None);
    }
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