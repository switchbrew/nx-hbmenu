#include "theme.h"
#include "button_a_light_bin.h"
#include "button_a_dark_bin.h"
#include "hbmenu_logo_light_bin.h"
#include "hbmenu_logo_dark_bin.h"

void themeStartup(ThemePreset preset) {
    switch (preset) {
        case THEME_PRESET_LIGHT:
            themeCurrent = (theme_t) { 
                textColor: MakeColor(0, 0, 0, 255),
                frontWaveColor: MakeColor(100, 212, 250, 255),
                middleWaveColor: MakeColor(100, 153, 255, 255),
                backWaveColor: MakeColor(154, 171, 255, 255),
                backgroundColor: MakeColor(233, 236, 241, 255),
                highlightColor: MakeColor(129, 137, 236, 255),
                enableWaveBlending: 0,
                buttonAImage: button_a_light_bin,
                hbmenuLogoImage: hbmenu_logo_light_bin
            };
            break;

        case THEME_PRESET_DARK:
            themeCurrent = (theme_t) { 
                textColor: MakeColor(255, 255, 255, 255),
                frontWaveColor: MakeColor(96, 204, 204, 255),
                middleWaveColor: MakeColor(66, 154, 159, 255),
                backWaveColor: MakeColor(73, 103, 169, 255),
                backgroundColor: MakeColor(45, 45, 50, 255),
                highlightColor: MakeColor(129, 137, 236, 255),
                enableWaveBlending: 0,
                buttonAImage: button_a_dark_bin,
                hbmenuLogoImage: hbmenu_logo_dark_bin
            };
            break;
    }
}