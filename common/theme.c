#include "theme.h"
/*#include "button_a_light_bin.h"
#include "button_a_dark_bin.h"    
#include "button_b_light_bin.h"
#include "button_b_dark_bin.h"*/
#include "hbmenu_logo_light_bin.h"
#include "hbmenu_logo_dark_bin.h"

bool colorFromSetting(config_setting_t *rgba, color_t *col) {
    if(rgba == NULL)
        return false;
    *col = MakeColor(config_setting_get_int_elem(rgba, 0), config_setting_get_int_elem(rgba, 1), config_setting_get_int_elem(rgba, 2), config_setting_get_int_elem(rgba, 3));
    return true;
}

void themeStartup(ThemePreset preset) {
    theme_t themeLight = (theme_t) { 
        .textColor = MakeColor(0, 0, 0, 255),
        .frontWaveColor = MakeColor(100, 212, 250, 255),
        .middleWaveColor = MakeColor(100, 153, 255, 255),
        .backWaveColor = MakeColor(154, 171, 255, 255),
        .backgroundColor = MakeColor(233, 236, 241, 255),
        .highlightColor = MakeColor(91, 237, 224, 255),
        .separatorColor = MakeColor(219, 218, 219, 255),
        .enableWaveBlending = 0,
        .buttonAText = "\uE0E0",
        .buttonBText = "\uE0E1",
        //.buttonAImage = button_a_light_bin,
        //.buttonBImage = button_b_light_bin,
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
        .enableWaveBlending = 0,
        .buttonAText = "\uE0A0",
        .buttonBText = "\uE0A1",
        //.buttonAImage = button_a_dark_bin,
        //.buttonBImage = button_b_dark_bin,
        .hbmenuLogoImage = hbmenu_logo_dark_bin
    };
    config_t *cfg = NULL;
    cfg = (config_t *) malloc(sizeof(config_t));
    config_init(cfg);
    config_setting_t *theme;
    color_t text, frontWave, middleWave, backWave, background, highlight, separator;
    int waveBlending;
    const char *AText, *BText;
    bool good_cfg=config_read_file(cfg, "/hbtheme.cfg");
    switch (preset) {
        case THEME_PRESET_LIGHT:
            if (good_cfg) {
                theme=config_lookup(cfg, "lightTheme");
                if (theme != NULL) {
                    if (!colorFromSetting(config_lookup(cfg, "lightTheme.textColor"), &text))
                        text = themeLight.textColor;
                    if (!colorFromSetting(config_lookup(cfg, "lightTheme.frontWaveColor"), &frontWave))
                        frontWave = themeLight.frontWaveColor;
                    if (!colorFromSetting(config_lookup(cfg, "lightTheme.middleWaveColor"), &middleWave))
                        middleWave = themeLight.middleWaveColor;
                    if (!colorFromSetting(config_lookup(cfg, "lightTheme.backWaveColor"), &backWave))
                        backWave = themeLight.backWaveColor;
                    if (!colorFromSetting(config_lookup(cfg, "lightTheme.backgroundColor"), &background))
                        background = themeLight.backgroundColor;
                    if (!colorFromSetting(config_lookup(cfg, "lightTheme.highlightColor"), &highlight))
                        highlight = themeLight.highlightColor;
                    if (!colorFromSetting(config_lookup(cfg, "lightTheme.separatorColor"), &separator))
                        separator = themeLight.separatorColor;
                } else {
                    themeCurrent=themeLight;
                }
            } else {
                themeCurrent=themeLight;
            }
            break;

        case THEME_PRESET_DARK:
            if (good_cfg) {
                theme=config_lookup(cfg, "darkTheme");
                if (theme != NULL) {
                    if (!colorFromSetting(config_lookup(cfg, "darkTheme.textColor"), &text))
                        text = themeDark.textColor;
                    if (!colorFromSetting(config_lookup(cfg, "darkTheme.frontWaveColor"), &frontWave))
                        frontWave = themeDark.frontWaveColor;
                    if (!colorFromSetting(config_lookup(cfg, "darkTheme.middleWaveColor"), &middleWave))
                        middleWave = themeDark.middleWaveColor;
                    if (!colorFromSetting(config_lookup(cfg, "darkTheme.backWaveColor"), &backWave))
                        backWave = themeDark.backWaveColor;
                    if (!colorFromSetting(config_lookup(cfg, "darkTheme.backgroundColor"), &background))
                        background = themeDark.backgroundColor;
                    if (!colorFromSetting(config_lookup(cfg, "darkTheme.highlightColor"), &highlight))
                        highlight = themeDark.highlightColor;
                    if (!colorFromSetting(config_lookup(cfg, "darkTheme.separatorColor"), &separator))
                        separator = themeDark.separatorColor;
                } else {
                    themeCurrent=themeDark;
                }
            } else {
                themeCurrent=themeDark;
            }
            break;
    }
    if (good_cfg){
        if (!config_setting_lookup_int(theme, "enableWaveBlending", &waveBlending))
            waveBlending = themeLight.enableWaveBlending;
        if (!config_setting_lookup_string(theme, "buttonAText", &AText))
            AText = themeLight.buttonAText;
        if (!config_setting_lookup_string(theme, "buttonBText", &BText))
            BText = themeDark.buttonBText;
        themeCurrent = (theme_t) { 
            .textColor = text,
            .frontWaveColor = frontWave,
            .middleWaveColor = middleWave,
            .backWaveColor = backWave,
            .backgroundColor = background,
            .highlightColor = highlight,
            .separatorColor = separator,
            .enableWaveBlending = waveBlending,
            .buttonAText = AText,
            .buttonBText = BText,
            //.buttonAImage = button_a_dark_bin,
            //.buttonBImage = button_b_dark_bin,
            .hbmenuLogoImage = hbmenu_logo_dark_bin
        };
    }
}
