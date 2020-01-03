#pragma once

#include "common.h"
#include <libconfig.h>

typedef enum
{
    THEME_PRESET_LIGHT,
    THEME_PRESET_DARK,
} ThemePreset;

typedef struct
{
    bool visible;
    bool posType;       // false = absolute, true = relative
    int posStart[2];
    int posEnd[2];
    int size[2];         // width/height
    int imageSize[2];   // width/height for the actual image data
    int touchSize[2];
    int posFinal[2];
    uint32_t textSize[2];
    u32 font;
} ThemeLayoutObject;

typedef struct
{
    color_t textColor;
    color_t logoColor;
    color_t attentionTextColor;
    color_t frontWaveColor;
    color_t middleWaveColor;
    color_t backWaveColor;
    color_t backgroundColor;
    color_t highlightColor;
    color_t highlightGradientEdgeColor;
    color_t separatorColor;
    color_t borderColor;
    color_t borderTextColor;
    color_t progressBarColor;
    bool logoColor_set;
    bool enableWaveBlending;
    char buttonAText[32];
    char buttonBText[32];
    char buttonXText[32];
    char buttonYText[32];
    char buttonPText[32];
    char buttonMText[32];
    char labelStarOnText[32];
    char labelStarOffText[32];
    const uint8_t *hbmenuLogoImage;

    ThemeLayoutObject layoutObjects[ThemeLayoutId_Total];
} theme_t;

bool colorFromSetting(config_setting_t *rgba, color_t *col);
void themeStartup(ThemePreset preset);
void GetThemePathFromConfig(char* themePath, size_t size);
void SetThemePathToConfig(const char* themePath);

extern theme_t themeCurrent;

extern ThemePreset themeGlobalPreset;
