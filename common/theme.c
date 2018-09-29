#include "theme.h"
/*#include "button_a_light_bin.h"
#include "button_a_dark_bin.h"    
#include "button_b_light_bin.h"
#include "button_b_dark_bin.h"*/
#include "hbmenu_logo_light_bin.h"
#include "hbmenu_logo_dark_bin.h"

theme_t themeCurrent;
ThemePreset themeGlobalPreset;

bool colorFromSetting(config_setting_t *rgba, color_t *col) {
    if(rgba == NULL)
        return false;
    *col = MakeColor(config_setting_get_int_elem(rgba, 0), config_setting_get_int_elem(rgba, 1), config_setting_get_int_elem(rgba, 2), config_setting_get_int_elem(rgba, 3));
    return true;
}

void themeStartup(ThemePreset preset) {
    themeGlobalPreset = preset;
    theme_t themeLight = (theme_t) { 
        .textColor = MakeColor(0, 0, 0, 255),
        .frontWaveColor = MakeColor(100, 212, 250, 255),
        .middleWaveColor = MakeColor(100, 153, 255, 255),
        .backWaveColor = MakeColor(154, 171, 255, 255),
        .backgroundColor = MakeColor(233, 236, 241, 255),
        .highlightColor = MakeColor(91, 237, 224, 255),
        .separatorColor = MakeColor(219, 218, 219, 255),
        .borderColor = MakeColor(255,255,255,255),
        .borderTextColor = MakeColor(64,64,64,255),
        .enableWaveBlending = 0,
        .buttonAText = "\uE0E0",
        .buttonBText = "\uE0E1",
        .buttonPText = "\uE0EF",
        .buttonMText = "\uE0F0",
        .hbmenuLogoImage = hbmenu_logo_light_bin
    };
    
    theme_t themeDark = (theme_t) { 
        .textColor = MakeColor(255, 255, 255, 255),
        .frontWaveColor = MakeColor(96, 204, 204, 255),
        .middleWaveColor = MakeColor(66, 154, 159, 255),
        .backWaveColor = MakeColor(73, 103, 169, 255),
        .backgroundColor = MakeColor(45, 45, 50, 255),
        .highlightColor = MakeColor(91, 237, 224, 255),
        .separatorColor = MakeColor(219, 218, 219, 255),
        .borderColor = MakeColor(255,255,255,255),
        .borderTextColor = MakeColor(64,64,64,255),
        .enableWaveBlending = 0,
        .buttonAText = "\uE0A0",
        .buttonBText = "\uE0A1",
        .buttonPText = "\uE0B3",
        .buttonMText = "\uE0B4",
        .hbmenuLogoImage = hbmenu_logo_dark_bin
    };

    char themePath[PATH_MAX] = {0};
    GetThemePathFromConfig(themePath, PATH_MAX);

    theme_t *themeDefault;
    config_t cfg = {0};
    config_init(&cfg);
    config_setting_t *theme = NULL;
    color_t text, frontWave, middleWave, backWave, background, highlight, separator, borderColor, borderTextColor;
    int waveBlending;
    const char *AText, *BText, *PText, *MText;
    bool good_cfg = false;

    if(themePath[0]!=0)
        good_cfg = config_read_file(&cfg, themePath);

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
            if (!colorFromSetting(config_setting_lookup(theme, "separatorColor"), &separator))
                separator = themeDefault->separatorColor;
            if (!colorFromSetting(config_setting_lookup(theme, "borderColor"), &borderColor))
                borderColor = themeDefault->borderColor;
            if (!colorFromSetting(config_setting_lookup(theme, "borderTextColor"), &borderTextColor))
                borderTextColor = themeDefault->borderTextColor;
            if (!config_setting_lookup_int(theme, "enableWaveBlending", &waveBlending))
                waveBlending = themeDefault->enableWaveBlending;
            if (!config_setting_lookup_string(theme, "buttonAText", &AText))
                AText = themeDefault->buttonAText;
            if (!config_setting_lookup_string(theme, "buttonBText", &BText))
                BText = themeDefault->buttonBText;
            if (!config_setting_lookup_string(theme, "buttonPText", &PText))
                PText = themeDefault->buttonPText;
            if (!config_setting_lookup_string(theme, "buttonMText", &MText))
                MText = themeDefault->buttonMText;
            themeCurrent = (theme_t) { 
                .textColor = text,
                .frontWaveColor = frontWave,
                .middleWaveColor = middleWave,
                .backWaveColor = backWave,
                .backgroundColor = background,
                .highlightColor = highlight,
                .separatorColor = separator,
                .borderColor = borderColor,
                .borderTextColor = borderTextColor,
                .enableWaveBlending = waveBlending,
                .hbmenuLogoImage = themeDefault->hbmenuLogoImage
            };
            strncpy(themeCurrent.buttonAText, AText, sizeof(themeCurrent.buttonAText)-1);
            strncpy(themeCurrent.buttonBText, BText, sizeof(themeCurrent.buttonBText)-1);
            strncpy(themeCurrent.buttonPText, PText, sizeof(themeCurrent.buttonPText)-1);
            strncpy(themeCurrent.buttonMText, MText, sizeof(themeCurrent.buttonMText)-1);
        } else {
            themeCurrent = *themeDefault;
        }
    } else {
        themeCurrent = *themeDefault;
    }
    config_destroy(&cfg);
}

void GetThemePathFromConfig(char* themePath, size_t size) {
    const char* tmpThemePath = "";
    config_t cfg = {0};
    config_setting_t *settings = NULL;
    char tmp_path[PATH_MAX] = {0};

    #ifdef __SWITCH__
    tmp_path[0] = '/';
    #endif

    strncat(tmp_path, "config/nx-hbmenu/settings.cfg", sizeof(tmp_path)-2);
    bool good_cfg = config_read_file(&cfg, tmp_path);
    
    if(good_cfg) {
        settings = config_lookup(&cfg, "settings");
        if(settings != NULL) {
            if(config_setting_lookup_string(settings, "themePath", &tmpThemePath))
                strncpy(themePath, tmpThemePath, size-1);
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

    #ifdef __SWITCH__
    settingPath[0] = '/';
    #endif

    strncat(settingPath, "config/nx-hbmenu/settings.cfg", sizeof(settingPath)-2);
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