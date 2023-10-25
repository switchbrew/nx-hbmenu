#include "../common/common.h"

bool netstatusGetDetails(AssetId *id) {
    Result rc=0;
    NifmInternetConnectionType contype;
    u32 wifiStrength=0;
    NifmInternetConnectionStatus connectionStatus;

    rc = nifmGetInternetConnectionStatus(&contype, &wifiStrength, &connectionStatus);
    if (R_FAILED(rc)) {
        *id = AssetId_airplane_icon;
        return true;
    }

    if (contype == NifmInternetConnectionType_Ethernet) {
        if (connectionStatus != NifmInternetConnectionStatus_Connected)
            *id = AssetId_eth_none_icon;
        else
            *id = AssetId_eth_icon;
        return true;
    }

    if (wifiStrength==0) {
        *id = AssetId_wifi_none_icon;
        return true;
    }

    if (wifiStrength==3)
        *id = AssetId_wifi3_icon;
    else if (wifiStrength==2)
        *id = AssetId_wifi2_icon;
    else
        *id = AssetId_wifi1_icon;

    return true;
}
