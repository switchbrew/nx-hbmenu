#include "theme.h"

theme_t themeCurrent;
ThemePreset themeGlobalPreset;

bool colorFromSetting(config_setting_t *rgba, color_t *col) {
    if(rgba == NULL)
        return false;
    *col = MakeColor(config_setting_get_int_elem(rgba, 0), config_setting_get_int_elem(rgba, 1), config_setting_get_int_elem(rgba, 2), config_setting_get_int_elem(rgba, 3));
    return true;
}

bool intElemFromSetting(config_setting_t *setting, int *out, size_t count) {
    if (!setting || config_setting_length(setting) < count)
        return false;

    for (size_t i=0; i<count; i++) {
        out[i] = config_setting_get_int_elem(setting, i);
    }

    return true;
}

bool layoutObjectFromSetting(config_setting_t *layout_setting, ThemeLayoutObject *obj, bool ignore_cfg_visible) {
    int tmp=0;
    ThemeLayoutObject tmpobj={0};
    if (!layout_setting)
        return false;

    memcpy(tmpobj.posStart, obj->posStart, sizeof(obj->posStart));
    memcpy(tmpobj.posEnd, obj->posEnd, sizeof(obj->posEnd));
    memcpy(tmpobj.size, obj->size, sizeof(obj->size));

    if (config_setting_lookup_bool(layout_setting, "visible", &tmp)==CONFIG_TRUE)
        tmpobj.visible = tmp;
    else
        tmpobj.visible = obj->visible;
    if (config_setting_lookup_bool(layout_setting, "posType", &tmp)==CONFIG_TRUE)
        tmpobj.posType = tmp;
    else
        tmpobj.posType = obj->posType;

    intElemFromSetting(config_setting_lookup(layout_setting, "posStart"), tmpobj.posStart, 2);
    intElemFromSetting(config_setting_lookup(layout_setting, "posEnd"), tmpobj.posEnd, 2);
    intElemFromSetting(config_setting_lookup(layout_setting, "size"), tmpobj.size, 2);

    if (!tmpobj.posType && (tmpobj.posStart[0] < 0 || tmpobj.posStart[1] < 0 || tmpobj.posEnd[0] < 0 || tmpobj.posEnd[1] < 0))
        return false;
    if (tmpobj.size[0] < 0 || tmpobj.size[1] < 0)
        return false;

    obj->posStart[0] = tmpobj.posStart[0];
    obj->posStart[1] = tmpobj.posStart[1];
    obj->posEnd[0] = tmpobj.posEnd[0];
    obj->posEnd[1] = tmpobj.posEnd[1];

    if (!ignore_cfg_visible) obj->visible = tmpobj.visible;
    obj->posType = tmpobj.posType;
    obj->size[0] = tmpobj.size[0];
    obj->size[1] = tmpobj.size[1];

    return true;
}

bool assetObjectFromSetting(config_setting_t *asset_setting, AssetId id, ThemeLayoutObject *layoutobj) {
    int imageSize[2]={0};
    const char *path = NULL;
    char tmp_path[PATH_MAX];
    if (!asset_setting)
        return false;

    if (config_setting_lookup_string(asset_setting, "path", &path)==CONFIG_FALSE)
        return false;

    if (!intElemFromSetting(config_setting_lookup(asset_setting, "imageSize"), imageSize, 2))
        return false;

    if (imageSize[0] <= 0 || imageSize[1] <= 0 || imageSize[0] > 1280 || imageSize[1] > 720)
        return false;

    if (layoutobj && (imageSize[0] != layoutobj->imageSize[0] || imageSize[1] != layoutobj->imageSize[1]))
        return false;

    memset(tmp_path, 0, sizeof(tmp_path));
    snprintf(tmp_path, sizeof(tmp_path)-1, "theme:/%s", path);

    return assetsLoadFromTheme(id, tmp_path, imageSize);
}

void themeStartup(ThemePreset preset) {
    themeGlobalPreset = preset;
    theme_t themeLight = (theme_t) {
        .textColor = MakeColor(0, 0, 0, 255),
        .attentionTextColor = MakeColor(255, 0, 0, 255),
        .frontWaveColor = MakeColor(100, 212, 250, 255),
        .middleWaveColor = MakeColor(100, 153, 255, 255),
        .backWaveColor = MakeColor(154, 171, 255, 255),
        .backgroundColor = MakeColor(233, 236, 241, 255),
        .highlightColor = MakeColor(91, 237, 224, 255),
        .highlightGradientEdgeColor = MakeColor(255,255,255,255),
        .separatorColor = MakeColor(219, 218, 219, 255),
        .borderColor = MakeColor(255,255,255,255),
        .borderTextColor = MakeColor(64,64,64,255),
        .progressBarColor = MakeColor(0,224,0,255),
        .enableWaveBlending = 0,
        .buttonAText = "\uE0E0",
        .buttonBText = "\uE0E1",
        .buttonXText = "\uE0E2",
        .buttonYText = "\uE0E3",
        .buttonPText = "\uE0EF",
        .buttonMText = "\uE0F0",
        .labelStarOnText = "\u2605",
        .labelStarOffText = "\u2606",
    };

    theme_t themeDark = (theme_t) {
        .textColor = MakeColor(255, 255, 255, 255),
        .attentionTextColor = MakeColor(255, 0, 0, 255),
        .frontWaveColor = MakeColor(96, 204, 204, 255),
        .middleWaveColor = MakeColor(66, 154, 159, 255),
        .backWaveColor = MakeColor(73, 103, 169, 255),
        .backgroundColor = MakeColor(45, 45, 50, 255),
        .highlightColor = MakeColor(91, 237, 224, 255),
        .highlightGradientEdgeColor = MakeColor(255,255,255,255),
        .separatorColor = MakeColor(219, 218, 219, 255),
        .borderColor = MakeColor(255,255,255,255),
        .borderTextColor = MakeColor(64,64,64,255),
        .progressBarColor = MakeColor(0,224,0,255),
        .enableWaveBlending = 0,
        .buttonAText = "\uE0A0",
        .buttonBText = "\uE0A1",
        .buttonXText = "\uE0A2",
        .buttonYText = "\uE0A3",
        .buttonPText = "\uE0B3",
        .buttonMText = "\uE0B4",
        .labelStarOnText = "\u2605",
        .labelStarOffText = "\u2606",
    };

    theme_t themeCommon = {
        .layoutObjects = {
            [ThemeLayoutId_Logo] = {
                .visible = true,
                .posType = false,
                .posStart = {40, 20},
            },

            [ThemeLayoutId_HbmenuVersion] = {
                .visible = true,
                .posType = false,
                .posStart = {184, 46 + 18},
                .font = interuiregular14,
            },

            [ThemeLayoutId_LoaderInfo] = {
                .visible = true,
                .posType = true,
                .posStart = {43, 46 + 18 + 20},
                .posEnd = {184},
                .font = interuiregular14,
            },

            [ThemeLayoutId_AttentionText] = {
                .visible = true,
                .posType = true,
                .posStart = {-32, 46 + 18},
                .font = interuimedium30,
            },

            [ThemeLayoutId_LogInfo] = {
                .visible = true,
                .posType = false,
                .posStart = {180 + 256, 46 + 16 + 18},
                .font = interuiregular14,
            },

            [ThemeLayoutId_InfoMsg] = {
                .visible = true,
                .posType = false,
                .posStart = {64, 128 + 18},
                .font = interuiregular14,
            },

            [ThemeLayoutId_MenuPath] = {
                .visible = true,
                .posType = false,
                .posStart = {40, 720 - 47 + 24},
                .size = {380},
                .font = interuiregular18,
            },

            [ThemeLayoutId_MenuTypeMsg] = {
                .visible = true,
                .posType = false,
                .posStart = {1180, 30 + 26 + 32 + 20},
                .font = interuiregular18,
            },

            [ThemeLayoutId_MsgBoxSeparator] = {
                .visible = true,
                .posType = true,
                .posStart = {0, -80},
            },

            [ThemeLayoutId_MsgBoxBottomText] = {
                .visible = true,
                .posType = true,
                .posStart = {0, -29},
            },

            // ThemeLayoutId_BackgroundImage is not set with the defaults.

            [ThemeLayoutId_BackWave] = {
                .visible = true,
                .posType = true,
                .size = {0, 295},
            },

            [ThemeLayoutId_MiddleWave] = {
                .visible = true,
                .posType = true,
                .size = {0, 290},
            },

            [ThemeLayoutId_FrontWave] = {
                .visible = true,
                .posType = true,
                .size = {0, 280},
            },

            [ThemeLayoutId_ButtonA] = {
                .visible = true,
                .posType = false,
                .posStart = {1280 - 126 - 30 - 32, 720 - 47 + 24},
                .touchSize = {36, 25},
                .font = fontscale7,
            },

            [ThemeLayoutId_ButtonAText] = {
                .visible = true,
                .posType = false,
                .posStart = {1280 - 90 - 30 - 32, 720 - 47 + 24},
                .touchSize = {0, 25},
                .font = interuiregular18,
            },

            [ThemeLayoutId_ButtonB] = {
                .visible = true,
                .posType = true,
                .posStart = {-36, 0},
                .posEnd = {0},
                .touchSize = {36, 25},
                .font = fontscale7,
            },

            [ThemeLayoutId_ButtonBText] = {
                .visible = true,
                .posType = true,
                .posStart = {-90, 0},
                .touchSize = {0, 32},
                .font = interuiregular18,
            },

            [ThemeLayoutId_ButtonY] = {
                .visible = true,
                .posType = true,
                .posStart = {-36, 0},
                .font = fontscale7,
            },

            [ThemeLayoutId_ButtonYText] = {
                .visible = true,
                .posType = true,
                .posStart = {-32, 0},
                .font = interuiregular18,
            },

            [ThemeLayoutId_ButtonM] = {
                .visible = true,
                .posType = true,
                .posStart = {-36, 0},
                .font = fontscale7,
            },

            [ThemeLayoutId_ButtonMText] = {
                .visible = true,
                .posType = true,
                .posStart = {-32, 0},
                .font = interuiregular18,
            },

            [ThemeLayoutId_ButtonX] = {
                .visible = true,
                .posType = true,
                .posStart = {-36, 0},
                .touchSize = {36, 25},
                .font = fontscale7,
            },

            [ThemeLayoutId_ButtonXText] = {
                .visible = true,
                .posType = true,
                .posStart = {-40 + 8, 0},
                .touchSize = {0, 25},
                .font = interuiregular18,
            },

            [ThemeLayoutId_NetworkIcon] = {
                .visible = true,
                .posType = true,
                .posStart = {0, 0 + 47 + 10 + 3},
            },

            [ThemeLayoutId_BatteryCharge] = {
                .visible = true,
                .posType = false,
                .posStart = {1180 - 10 - 24 - 8, 0 + 47 + 10 + 21 + 4},
                .font = interuiregular14,
            },

            [ThemeLayoutId_BatteryIcon] = {
                .visible = true,
                .posType = false,
                .posStart = {1180 - 8 - 24 - 8, 0 + 47 + 10 + 6},
            },

            [ThemeLayoutId_ChargingIcon] = {
                .visible = true,
                .posType = false,
                .posStart = {1180 - 20, 0 + 47 + 10 + 6},
            },

            [ThemeLayoutId_Status] = {
                .visible = true,
                .posType = false,
                .posStart = {1180, 0 + 47 + 10},
                .font = interuimedium20,
            },

            [ThemeLayoutId_Temperature] = {
                .visible = true,
                .posType = false,
                .posStart = {1180 + 4, 0 + 47 + 10 + + 21 + 6},
                .font = interuiregular14,
            },

            [ThemeLayoutId_MenuList] = {
                .visible = true,
                .posType = false,
                .posStart = {29, 720 - 100 - 145},
                .posEnd = {140 + 30, 0},
                .size = {140, 140 + 32},
            },

            [ThemeLayoutId_MenuListTiles] = {
                .visible = true,
                .posType = true,
                .posEnd = {7, 0},
                .size = {0, 0},
            },

            [ThemeLayoutId_MenuListIcon] = {
                .visible = true,
                .posType = true,
                .posStart = {0, 32},
                .size = {140, 140},
                .imageSize = {256, 256},
            },

            [ThemeLayoutId_MenuListName] = {
                .visible = true,
                .posType = true,
                .posStart = {4, 4 + 18},
                .size = {140 - 32, 0},
                .font = interuiregular14,
            },

            [ThemeLayoutId_MenuActiveEntryIcon] = {
                .visible = true,
                .posType = false,
                .posStart = {117, 100+10},
                .size = {256, 256},
                .imageSize = {256, 256},
            },

            [ThemeLayoutId_MenuActiveEntryName] = {
                .visible = true,
                .posType = false,
                .posStart = {1280 - 790, 135+10 + 39},
                .size = {790 - 120, 0},
                .font = interuimedium30,
            },

            [ThemeLayoutId_MenuActiveEntryAuthor] = {
                .visible = true,
                .posType = false,
                .posStart = {1280 - 790, 135+10 + 28 + 30 + 18},
                .font = interuiregular14,
            },

            [ThemeLayoutId_MenuActiveEntryVersion] = {
                .visible = true,
                .posType = false,
                .posStart = {1280 - 790, 135+10 + 28 + 30 + 18 + 6 + 18},
                .font = interuiregular14,
            },
        },
    };

    char themePath[PATH_MAX] = {0};
    GetThemePathFromConfig(themePath, PATH_MAX);

    theme_t *themeDefault;
    config_t cfg = {0};
    config_init(&cfg);
    config_setting_t *theme = NULL, *layout = NULL, *assets = NULL;
    color_t text, logoColor={0}, attentionText, frontWave, middleWave, backWave, background, highlight, highlightGradientEdgeColor, separator, borderColor, borderTextColor, progressBarColor;
    int waveBlending;
    const char *AText, *BText, *XText, *YText, *PText, *MText, *starOnText, *starOffText;
    bool logoColor_set = false;
    bool good_cfg = false;
    bool is_romfs = false;

    assetsClearTheme();

    if(themePath[0]!=0) {
        const char* cfg_path = themePath;
        #ifdef __SWITCH__
        const char* ext = getExtension(themePath);
        if (strcasecmp(ext, ".romfs")==0) {
            if (R_FAILED(romfsMountFromFsdev(themePath, 0, "theme")))
                cfg_path = NULL;
            else {
                is_romfs = true;
                cfg_path = "theme:/theme.cfg";
            }
        }
        #endif

        if (cfg_path) good_cfg = config_read_file(&cfg, cfg_path);
    }

    switch (preset) {
        case THEME_PRESET_LIGHT:
        default:
            themeDefault = &themeLight;
            if (good_cfg)
                theme = config_lookup(&cfg, "lightTheme");
            break;

        case THEME_PRESET_DARK:
            themeDefault = &themeDark;
            if (good_cfg)
                theme = config_lookup(&cfg, "darkTheme");
            break;
    }

    if (good_cfg) {
        if (theme != NULL) {
            if (!colorFromSetting(config_setting_lookup(theme, "textColor"), &text))
                text = themeDefault->textColor;
            if (colorFromSetting(config_setting_lookup(theme, "logoColor"), &logoColor))
                logoColor_set = true;
            if (!colorFromSetting(config_setting_lookup(theme, "attentionTextColor"), &attentionText))
                attentionText = themeDefault->attentionTextColor;
            if (!colorFromSetting(config_setting_lookup(theme, "frontWaveColor"), &frontWave))
                frontWave = themeDefault->frontWaveColor;
            if (!colorFromSetting(config_setting_lookup(theme, "middleWaveColor"), &middleWave))
                middleWave = themeDefault->middleWaveColor;
            if (!colorFromSetting(config_setting_lookup(theme, "backWaveColor"), &backWave))
                backWave = themeDefault->backWaveColor;
            if (!colorFromSetting(config_setting_lookup(theme, "backgroundColor"), &background))
                background = themeDefault->backgroundColor;
            if (!colorFromSetting(config_setting_lookup(theme, "highlightColor"), &highlight))
                highlight = themeDefault->highlightColor;
            if (!colorFromSetting(config_setting_lookup(theme, "highlightGradientEdgeColor"), &highlightGradientEdgeColor))
                highlightGradientEdgeColor = themeDefault->highlightGradientEdgeColor;
            if (!colorFromSetting(config_setting_lookup(theme, "separatorColor"), &separator))
                separator = themeDefault->separatorColor;
            if (!colorFromSetting(config_setting_lookup(theme, "borderColor"), &borderColor))
                borderColor = themeDefault->borderColor;
            if (!colorFromSetting(config_setting_lookup(theme, "borderTextColor"), &borderTextColor))
                borderTextColor = themeDefault->borderTextColor;
            if (!colorFromSetting(config_setting_lookup(theme, "progressBarColor"), &progressBarColor))
                progressBarColor = themeDefault->progressBarColor;
            if (!config_setting_lookup_int(theme, "enableWaveBlending", &waveBlending))
                waveBlending = themeDefault->enableWaveBlending;
            if (!config_setting_lookup_string(theme, "buttonAText", &AText))
                AText = themeDefault->buttonAText;
            if (!config_setting_lookup_string(theme, "buttonBText", &BText))
                BText = themeDefault->buttonBText;
            if (!config_setting_lookup_string(theme, "buttonXText", &XText))
                XText = themeDefault->buttonXText;
            if (!config_setting_lookup_string(theme, "buttonYText", &YText))
                YText = themeDefault->buttonYText;
            if (!config_setting_lookup_string(theme, "buttonPText", &PText))
                PText = themeDefault->buttonPText;
            if (!config_setting_lookup_string(theme, "buttonMText", &MText))
                MText = themeDefault->buttonMText;
            if (!config_setting_lookup_string(theme, "labelStarOnText", &starOnText))
                starOnText = themeDefault->labelStarOnText;
            if (!config_setting_lookup_string(theme, "labelStarOffText", &starOffText))
                starOffText = themeDefault->labelStarOffText;
            themeCurrent = (theme_t) {
                .textColor = text,
                .logoColor = logoColor,
                .attentionTextColor = attentionText,
                .frontWaveColor = frontWave,
                .middleWaveColor = middleWave,
                .backWaveColor = backWave,
                .backgroundColor = background,
                .highlightColor = highlight,
                .highlightGradientEdgeColor = highlightGradientEdgeColor,
                .separatorColor = separator,
                .borderColor = borderColor,
                .borderTextColor = borderTextColor,
                .progressBarColor = progressBarColor,
                .logoColor_set = logoColor_set,
                .enableWaveBlending = waveBlending,
            };
            strncpy(themeCurrent.buttonAText, AText, sizeof(themeCurrent.buttonAText));
            themeCurrent.buttonAText[sizeof(themeCurrent.buttonAText)-1] = 0;
            strncpy(themeCurrent.buttonBText, BText, sizeof(themeCurrent.buttonBText));
            themeCurrent.buttonBText[sizeof(themeCurrent.buttonBText)-1] = 0;
            strncpy(themeCurrent.buttonXText, XText, sizeof(themeCurrent.buttonXText));
            themeCurrent.buttonXText[sizeof(themeCurrent.buttonXText)-1] = 0;
            strncpy(themeCurrent.buttonYText, YText, sizeof(themeCurrent.buttonYText));
            themeCurrent.buttonYText[sizeof(themeCurrent.buttonYText)-1] = 0;
            strncpy(themeCurrent.buttonPText, PText, sizeof(themeCurrent.buttonPText));
            themeCurrent.buttonPText[sizeof(themeCurrent.buttonPText)-1] = 0;
            strncpy(themeCurrent.buttonMText, MText, sizeof(themeCurrent.buttonMText));
            themeCurrent.buttonMText[sizeof(themeCurrent.buttonMText)-1] = 0;
            strncpy(themeCurrent.labelStarOnText, starOnText, sizeof(themeCurrent.labelStarOnText));
            themeCurrent.labelStarOnText[sizeof(themeCurrent.labelStarOnText)-1] = 0;
            strncpy(themeCurrent.labelStarOffText, starOffText, sizeof(themeCurrent.labelStarOffText));
            themeCurrent.labelStarOffText[sizeof(themeCurrent.labelStarOffText)-1] = 0;
        } else {
            themeCurrent = *themeDefault;
        }

        memcpy(themeCurrent.layoutObjects, themeCommon.layoutObjects, sizeof(themeCommon.layoutObjects));

        layout = config_lookup(&cfg, "layout");

        if (layout != NULL) {
            layoutObjectFromSetting(config_setting_lookup(layout, "logo"), &themeCurrent.layoutObjects[ThemeLayoutId_Logo], true);
            layoutObjectFromSetting(config_setting_lookup(layout, "hbmenuVersion"), &themeCurrent.layoutObjects[ThemeLayoutId_HbmenuVersion], true);
            layoutObjectFromSetting(config_setting_lookup(layout, "loaderInfo"), &themeCurrent.layoutObjects[ThemeLayoutId_LoaderInfo], true);
            layoutObjectFromSetting(config_setting_lookup(layout, "attentionText"), &themeCurrent.layoutObjects[ThemeLayoutId_AttentionText], true);
            layoutObjectFromSetting(config_setting_lookup(layout, "logInfo"), &themeCurrent.layoutObjects[ThemeLayoutId_LogInfo], true);
            layoutObjectFromSetting(config_setting_lookup(layout, "infoMsg"), &themeCurrent.layoutObjects[ThemeLayoutId_InfoMsg], true);
            layoutObjectFromSetting(config_setting_lookup(layout, "menuPath"), &themeCurrent.layoutObjects[ThemeLayoutId_MenuPath], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "menuTypeMsg"), &themeCurrent.layoutObjects[ThemeLayoutId_MenuTypeMsg], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "msgBoxSeparator"), &themeCurrent.layoutObjects[ThemeLayoutId_MsgBoxSeparator], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "msgBoxBottomText"), &themeCurrent.layoutObjects[ThemeLayoutId_MsgBoxBottomText], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "backgroundImage"), &themeCurrent.layoutObjects[ThemeLayoutId_BackgroundImage], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "backWave"), &themeCurrent.layoutObjects[ThemeLayoutId_BackWave], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "middleWave"), &themeCurrent.layoutObjects[ThemeLayoutId_MiddleWave], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "frontWave"), &themeCurrent.layoutObjects[ThemeLayoutId_FrontWave], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "buttonA"), &themeCurrent.layoutObjects[ThemeLayoutId_ButtonA], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "buttonAText"), &themeCurrent.layoutObjects[ThemeLayoutId_ButtonAText], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "buttonB"), &themeCurrent.layoutObjects[ThemeLayoutId_ButtonB], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "buttonBText"), &themeCurrent.layoutObjects[ThemeLayoutId_ButtonBText], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "buttonY"), &themeCurrent.layoutObjects[ThemeLayoutId_ButtonY], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "buttonYText"), &themeCurrent.layoutObjects[ThemeLayoutId_ButtonYText], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "buttonM"), &themeCurrent.layoutObjects[ThemeLayoutId_ButtonM], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "buttonMText"), &themeCurrent.layoutObjects[ThemeLayoutId_ButtonMText], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "buttonX"), &themeCurrent.layoutObjects[ThemeLayoutId_ButtonX], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "buttonXText"), &themeCurrent.layoutObjects[ThemeLayoutId_ButtonXText], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "networkIcon"), &themeCurrent.layoutObjects[ThemeLayoutId_NetworkIcon], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "batteryCharge"), &themeCurrent.layoutObjects[ThemeLayoutId_BatteryCharge], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "batteryIcon"), &themeCurrent.layoutObjects[ThemeLayoutId_BatteryIcon], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "chargingIcon"), &themeCurrent.layoutObjects[ThemeLayoutId_ChargingIcon], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "status"), &themeCurrent.layoutObjects[ThemeLayoutId_Status], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "temperature"), &themeCurrent.layoutObjects[ThemeLayoutId_Temperature], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "menuList"), &themeCurrent.layoutObjects[ThemeLayoutId_MenuList], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "menuListTiles"), &themeCurrent.layoutObjects[ThemeLayoutId_MenuListTiles], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "menuListIcon"), &themeCurrent.layoutObjects[ThemeLayoutId_MenuListIcon], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "menuListName"), &themeCurrent.layoutObjects[ThemeLayoutId_MenuListName], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "menuActiveEntryIcon"), &themeCurrent.layoutObjects[ThemeLayoutId_MenuActiveEntryIcon], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "menuActiveEntryName"), &themeCurrent.layoutObjects[ThemeLayoutId_MenuActiveEntryName], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "menuActiveEntryAuthor"), &themeCurrent.layoutObjects[ThemeLayoutId_MenuActiveEntryAuthor], false);
            layoutObjectFromSetting(config_setting_lookup(layout, "menuActiveEntryVersion"), &themeCurrent.layoutObjects[ThemeLayoutId_MenuActiveEntryVersion], false);
        }

        if (is_romfs) assets = config_lookup(&cfg, "assets");
        if (is_romfs && assets) {
            assetObjectFromSetting(config_setting_lookup(assets, "battery_icon"), AssetId_battery_icon, NULL);
            assetObjectFromSetting(config_setting_lookup(assets, "charging_icon"), AssetId_charging_icon, NULL);
            assetObjectFromSetting(config_setting_lookup(assets, "folder_icon"), AssetId_folder_icon, &themeCurrent.layoutObjects[ThemeLayoutId_MenuActiveEntryIcon]);
            assetObjectFromSetting(config_setting_lookup(assets, "invalid_icon"), AssetId_invalid_icon, &themeCurrent.layoutObjects[ThemeLayoutId_MenuActiveEntryIcon]);
            assetObjectFromSetting(config_setting_lookup(assets, "theme_icon_dark"), AssetId_theme_icon_dark, &themeCurrent.layoutObjects[ThemeLayoutId_MenuActiveEntryIcon]);
            assetObjectFromSetting(config_setting_lookup(assets, "theme_icon_light"), AssetId_theme_icon_light, &themeCurrent.layoutObjects[ThemeLayoutId_MenuActiveEntryIcon]);
            assetObjectFromSetting(config_setting_lookup(assets, "airplane_icon"), AssetId_airplane_icon, NULL);
            assetObjectFromSetting(config_setting_lookup(assets, "wifi_none_icon"), AssetId_wifi_none_icon, NULL);
            assetObjectFromSetting(config_setting_lookup(assets, "wifi1_icon"), AssetId_wifi1_icon, NULL);
            assetObjectFromSetting(config_setting_lookup(assets, "wifi2_icon"), AssetId_wifi2_icon, NULL);
            assetObjectFromSetting(config_setting_lookup(assets, "wifi3_icon"), AssetId_wifi3_icon, NULL);
            assetObjectFromSetting(config_setting_lookup(assets, "eth_icon"), AssetId_eth_icon, NULL);
            assetObjectFromSetting(config_setting_lookup(assets, "eth_none_icon"), AssetId_eth_none_icon, NULL);
            assetObjectFromSetting(config_setting_lookup(assets, "background_image"), AssetId_background_image, NULL);
        }
    } else {
        themeCurrent = *themeDefault;
        memcpy(themeCurrent.layoutObjects, themeCommon.layoutObjects, sizeof(themeCommon.layoutObjects));
    }

    ThemeLayoutObject *layoutobj = &themeCurrent.layoutObjects[ThemeLayoutId_MenuListTiles];
    if (layoutobj->posEnd[0] < 1) layoutobj->posEnd[0] = 1;

    layoutobj = &themeCurrent.layoutObjects[ThemeLayoutId_MenuListIcon];
    if (layoutobj->size[0] <= 0 || layoutobj->size[1] <= 0 || layoutobj->size[0] > layoutobj->imageSize[0] || layoutobj->size[1] > layoutobj->imageSize[1]) {
        layoutobj->size[0] = layoutobj->imageSize[0];
        layoutobj->size[1] = layoutobj->imageSize[1];
    }

    layoutobj = &themeCurrent.layoutObjects[ThemeLayoutId_MenuActiveEntryIcon];
    if (layoutobj->size[0] <= 0 || layoutobj->size[1] <= 0 || layoutobj->size[0] > layoutobj->imageSize[0] || layoutobj->size[1] > layoutobj->imageSize[1]) {
        layoutobj->size[0] = layoutobj->imageSize[0];
        layoutobj->size[1] = layoutobj->imageSize[1];
    }

    config_destroy(&cfg);

    #ifdef __SWITCH__
    if (is_romfs) romfsUnmount("theme");
    #endif
}

void GetThemePathFromConfig(char* themePath, size_t size) {
    const char* tmpThemePath = "";
    config_t cfg = {0};
    config_setting_t *settings = NULL;
    char tmp_path[PATH_MAX+1] = {0};
    char tmp_path_theme[PATH_MAX+1] = {0};

    snprintf(tmp_path, sizeof(tmp_path)-1, "%s/config/nx-hbmenu/settings.cfg", menuGetRootBasePath());
    snprintf(tmp_path_theme, sizeof(tmp_path_theme)-1, "%s/config/nx-hbmenu/themes/", menuGetRootBasePath());
    bool good_cfg = config_read_file(&cfg, tmp_path);

    if(good_cfg) {
        settings = config_lookup(&cfg, "settings");
        if(settings != NULL) {
            if(config_setting_lookup_string(settings, "themePath", &tmpThemePath))
                snprintf(themePath, size-1, "%s%s", tmp_path_theme, tmpThemePath);
        }
    }

    config_destroy(&cfg);
}

void SetThemePathToConfig(const char* themePath) {
    config_t cfg = {0};
    config_init(&cfg);

    char settingPath[PATH_MAX] = {0};
    config_setting_t *root = NULL,
                     *group = NULL,
                     *settings = NULL;

    themePath = getSlash(themePath);
    if(themePath[0] == '/') themePath++;

    #ifdef __SWITCH__
    settingPath[0] = '/';
    #endif

    snprintf(settingPath, sizeof(settingPath)-1, "%s/config/nx-hbmenu/settings.cfg", menuGetRootBasePath());
    bool good_cfg = config_read_file(&cfg, settingPath);

    if(good_cfg) {
        group = config_lookup(&cfg, "settings");
        if(group != NULL)
            settings = config_setting_lookup(group, "themePath");
        if(settings != NULL)
            config_setting_set_string(settings, themePath);
    } else {
        root = config_root_setting(&cfg);
        if(root != NULL)
            group = config_setting_add(root, "settings", CONFIG_TYPE_GROUP);
        if(group != NULL)
            settings = config_setting_add(group, "themePath", CONFIG_TYPE_STRING);
        if(settings != NULL)
            config_setting_set_string(settings, themePath);
    }

    if(!config_write_file(&cfg, settingPath)) {
        menuCreateMsgBox(780, 300, textGetString(StrId_ThemeNotApplied));
    }

    config_destroy(&cfg);
}
