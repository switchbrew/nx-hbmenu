#include "theme.h"
/*#include "button_a_light_bin.h"
#include "button_a_dark_bin.h"    
#include "button_b_light_bin.h"
#include "button_b_dark_bin.h"*/
#include "hbmenu_logo_light_bin.h"
#include "hbmenu_logo_dark_bin.h"

color_t colorFromSetting(config_setting_t *rgba) {
	if(rgba==NULL)
		return MakeColor(128,0,128,255);
	return MakeColor(config_setting_get_int_elem(rgba, 0), config_setting_get_int_elem(rgba, 1), config_setting_get_int_elem(rgba, 2), config_setting_get_int_elem(rgba, 3));
}
void themeStartup(ThemePreset preset) {
    config_t *cfg = NULL;
    cfg = (config_t *) malloc(sizeof(config_t));
    config_init(cfg);
    config_setting_t *theme;
    color_t text, frontWave, middleWave, backWave, background, highlight, separator;
	int waveBlending;
	const char *AText, *BText;
	bool good_cfg=false;
    switch (preset) {
        case THEME_PRESET_LIGHT:
        	if (config_read_file(cfg, "/hbtheme.cfg")) {
        		theme=config_lookup(cfg, "lightTheme");
        		if (theme != NULL) {
        			text = colorFromSetting(config_lookup(cfg, "lightTheme.textColor"));
        			frontWave = colorFromSetting(config_lookup(cfg, "lightTheme.frontWaveColor"));
        			middleWave = colorFromSetting(config_lookup(cfg, "lightTheme.middleWaveColor"));
        			backWave = colorFromSetting(config_lookup(cfg, "lightTheme.backWaveColor"));
        			background = colorFromSetting(config_lookup(cfg, "lightTheme.backgroundColor"));
        			highlight = colorFromSetting(config_lookup(cfg, "lightTheme.highlightColor"));
        			separator = colorFromSetting(config_lookup(cfg, "lightTheme.separatorColor"));
        			good_cfg = config_setting_lookup_int(theme, "enableWaveBlending", &waveBlending);
        			good_cfg = good_cfg && config_setting_lookup_string(theme, "buttonAText", &AText);
        			good_cfg = good_cfg && config_setting_lookup_string(theme, "buttonBText", &BText);
        		}
        	}
        	if (good_cfg) {
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
		            //.buttonAImage = button_a_light_bin,
		            //.buttonBImage = button_b_light_bin,
		            .hbmenuLogoImage = hbmenu_logo_light_bin
		        };
        	} else {
		        themeCurrent = (theme_t) { 
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
            }
            break;

        case THEME_PRESET_DARK:
        	if (config_read_file(cfg, "/hbtheme.cfg")) {
        		theme=config_lookup(cfg, "darkTheme");
        		if (theme != NULL) {
        			text = colorFromSetting(config_lookup(cfg, "darkTheme.textColor"));
        			frontWave = colorFromSetting(config_lookup(cfg, "darkTheme.frontWaveColor"));
        			middleWave = colorFromSetting(config_lookup(cfg, "darkTheme.middleWaveColor"));
        			backWave = colorFromSetting(config_lookup(cfg, "darkTheme.backWaveColor"));
        			background = colorFromSetting(config_lookup(cfg, "darkTheme.backgroundColor"));
        			highlight = colorFromSetting(config_lookup(cfg, "darkTheme.highlightColor"));
        			separator = colorFromSetting(config_lookup(cfg, "darkTheme.separatorColor"));
        			good_cfg = config_setting_lookup_int(theme, "enableWaveBlending", &waveBlending);
        			good_cfg = good_cfg && config_setting_lookup_string(theme, "buttonAText", &AText);
        			good_cfg = good_cfg && config_setting_lookup_string(theme, "buttonBText", &BText);
        		}
        	}
        	if (good_cfg) {
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
        	} else {
		        themeCurrent = (theme_t) { 
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
            }
            break;
    }
}
