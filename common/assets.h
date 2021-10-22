#pragma once
#include "common.h"

typedef enum {
    AssetId_battery_icon,
    AssetId_charging_icon,
    AssetId_folder_icon,
    AssetId_invalid_icon,
    AssetId_hbmenu_logo_dark,
    AssetId_hbmenu_logo_light,
    AssetId_theme_icon_dark,
    AssetId_theme_icon_light,
    AssetId_airplane_icon,
    AssetId_wifi_none_icon,
    AssetId_wifi1_icon,
    AssetId_wifi2_icon,
    AssetId_wifi3_icon,
    AssetId_eth_icon,
    AssetId_eth_none_icon,
    AssetId_background_image,

    AssetId_Max,
} AssetId;

typedef struct {
    bool initialized;
    u8 *buffer;
    size_t size;
    ImageMode imageMode;
    size_t pixSize;
    size_t imageSize[2];
    char path[PATH_MAX];
} assetsDataEntry;

Result assetsInit(void);
void assetsExit(void);
void assetsClearTheme(void);
bool assetsPhysfsReadFile(const char *path, u8 **data_buf, size_t *filesize, bool nul_term);
bool assetsLoadData(AssetId id, const char *path, int *imageSize);
void assetsGetData(AssetId id, assetsDataEntry **out);
u8 *assetsGetDataBuffer(AssetId id);

bool assetsLoadJpgFromMemory(u8 *indata, size_t indata_size, u8 *outdata, ImageMode imageMode, size_t width, size_t height);

