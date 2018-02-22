#pragma once

#include "common.h"

typedef struct
{
    color_t textColor;
    color_t frontWaveColor;
    color_t middleWaveColor;
    color_t backWaveColor;
    color_t backgroundColor;
    color_t highlightColor;
    bool enableWaveBlending;
} theme_t;

typedef enum
{
    THEME_PRESET_LIGHT,
    THEME_PRESET_DARK,
} ThemePreset;

void themeStartup(ThemePreset preset);

theme_t themeCurrent;