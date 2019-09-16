#include "status.h"

static bool s_statusInitialized = 0;
static thrd_t s_statusThread;
static cnd_t s_statusCdn;
static mtx_t s_statusMtx;
static mtx_t s_statusAccessMtx;
static bool s_statusExit;

static bool s_statusReady;
static bool s_statusNetFlag;
static AssetId s_statusNetAssetId;
static bool s_statusTemperatureFlag;
static s32 s_statusTemperature;

// This uses netstatusGetDetails from a dedicated thread, since nifmGetInternetConnectionStatus can block for a few seconds.

static int statusThreadProc(void* unused)
{
    mtx_lock(&s_statusMtx);

    struct timespec timeout = {0};
    bool tmpflag=0;
    bool thermalflag=0;
    bool thermal_initialized=0;
    AssetId tmpid;
    s32 temperature;

    thermal_initialized = thermalstatusInit();

    clock_gettime(CLOCK_MONOTONIC, &timeout);
    timeout.tv_sec++;

    for (;;)
    {
        cnd_timedwait(&s_statusCdn, &s_statusMtx, &timeout);

        if (s_statusExit)
            break;

        tmpflag = netstatusGetDetails(&tmpid);
        if (thermal_initialized) thermalflag = thermalstatusGetDetails(&temperature);

        mtx_lock(&s_statusAccessMtx);

        s_statusNetFlag = tmpflag;
        s_statusNetAssetId = tmpid;

        s_statusTemperatureFlag = thermalflag;
        s_statusTemperature = temperature;

        s_statusReady = 1;

        mtx_unlock(&s_statusAccessMtx);

        clock_gettime(CLOCK_MONOTONIC, &timeout);
        timeout.tv_sec++;
    }

    mtx_unlock(&s_statusMtx);

    if (thermal_initialized) thermalstatusExit();

    return 0;
}

bool statusGet(bool *netstatusFlag, AssetId *netstatusAssetId, bool *temperatureFlag, s32 *temperature) {
    if (!s_statusReady) return 0;

    mtx_lock(&s_statusAccessMtx);

    *netstatusFlag = s_statusNetFlag;
    *netstatusAssetId = s_statusNetAssetId;

    *temperatureFlag = s_statusTemperatureFlag;
    *temperature = s_statusTemperature;

    mtx_unlock(&s_statusAccessMtx);

    return 1;
}

bool statusInit(void)
{
    if (s_statusInitialized) return 1;

    if (cnd_init(&s_statusCdn) != thrd_success) return 0;
    if (mtx_init(&s_statusMtx, mtx_plain) != thrd_success) {
        cnd_destroy(&s_statusCdn);
        return 0;
    }

    if (mtx_init(&s_statusAccessMtx, mtx_plain) != thrd_success) {
        mtx_destroy(&s_statusMtx);
        cnd_destroy(&s_statusCdn);
        return 0;
    }

    if (thrd_create(&s_statusThread, statusThreadProc, 0) != thrd_success) {
        mtx_destroy(&s_statusAccessMtx);
        mtx_destroy(&s_statusMtx);
        cnd_destroy(&s_statusCdn);
        return 0;
    }

    s_statusInitialized = 1;
    return 1;
}

void statusExit(void)
{
    int res=0;

    if (!s_statusInitialized) return;
    s_statusInitialized = 0;

    mtx_lock(&s_statusMtx);
    s_statusExit = true;
    cnd_signal(&s_statusCdn);
    mtx_unlock(&s_statusMtx);

    thrd_join(s_statusThread, &res);

    s_statusReady = 0;

    mtx_destroy(&s_statusAccessMtx);
    mtx_destroy(&s_statusMtx);
    cnd_destroy(&s_statusCdn);
}

