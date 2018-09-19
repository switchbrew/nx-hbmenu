#pragma once

#include "common.h"
#include <libconfig.h>

typedef struct
{
    color_t textColor;
    color_t frontWaveColor;
    color_t middleWaveColor;
    color_t backWaveColor;
    color_t backgroundColor;
    color_t highlightColor;
    color_t separatorColor;
    color_t activeColor;
    bool enableWaveBlending;
    const char *buttonAText;
    const char *buttonBText;
    //const uint8_t *buttonAImage;
    //const uint8_t *buttonBImage;
    const uint8_t *hbmenuLogoImage;
} theme_t;

typedef enum
{
    THEME_PRESET_LIGHT,
    THEME_PRESET_DARK,
} ThemePreset;

bool colorFromSetting(config_setting_t *rgba, color_t *col);

void themeStartup(ThemePreset preset);

theme_t themeCurrent;

ThemePreset globalPreset;
