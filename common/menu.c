#include <time.h>
#include "common.h"

#include "switchicon_questionmark_bin.h"
#include "folder_icon_bin.h"

void launchMenuEntryTask(menuEntry_s* arg)
{
    menuEntry_s* me = arg;
    if (me->type == ENTRY_TYPE_FOLDER)
        menuScan(me->path);
        //changeDirTask(me->path);
    else
        launchMenuEntry(me);
}

typedef enum 
{
    IMAGE_MODE_RGB24,
    IMAGE_MODE_RGBA32
} ImageMode;

//Draws an RGB888 or RGBA8888 image.
static void drawImage(int x, int y, int width, int height, const uint8_t *image, ImageMode mode) {
    int tmpx, tmpy;
    int pos;
    color_t current_color;

    for (tmpx=0; tmpx<width; tmpx++) {
        for (tmpy=0; tmpy<height; tmpy++) {
            switch (mode) {
                case IMAGE_MODE_RGB24:
                    pos = ((tmpy*width) + tmpx) * 3;
                    current_color = MakeColor(image[pos+0], image[pos+1], image[pos+2], 255);
                    break;

                case IMAGE_MODE_RGBA32:
                    pos = ((tmpy*width) + tmpx) * 4;
                    current_color = MakeColor(image[pos+0], image[pos+1], image[pos+2], image[pos+3]);
                    break;
            }
            
            DrawPixel(x+tmpx, y+tmpy, current_color);
        }
    }
}

uint8_t *folder_icon_small;
uint8_t *switchicon_questionmark_small;

static void drawEntry(menuEntry_s* me, int off_x, int is_active) {
    int x, y;
    int start_y = 720 - 100 - 145;//*(n % 2);
    int end_y = start_y + 140 + 32;
    int start_x = off_x;//(n / 2);
    int end_x = start_x + 140;

    const uint8_t *smallimg = NULL;
    const uint8_t *largeimg = NULL;
    char tmpstr[1024];

    int border_start_x, border_end_x;
    int border_start_y, border_end_y;
    color_t border_color = MakeColor(255, 255, 255, 255);
    
    int shadow_start_y, shadow_y;
    int shadow_inset;
    color_t shadow_color;
    uint8_t shadow_alpha_base = 80;
    int shadow_size = 4;

    if (is_active) {
        border_color = themeCurrent.highlightColor;
        border_start_x = start_x-5;
        border_end_x = end_x+5;
        border_start_y = start_y-5;
        border_end_y = end_y+5;
    }
    else {
        border_start_x = start_x-3;
        border_end_x = end_x+3;
        border_start_y = start_y-3;
        border_end_y = end_y+3;
    }

    //{
        for (x=border_start_x; x<border_end_x; x++) {
            //DrawPixelRaw(x, start_y  , border_color0);
            DrawPixelRaw(x, end_y    , border_color);
            DrawPixelRaw(x, start_y-1, border_color);
            DrawPixelRaw(x, end_y  +1, border_color);
            DrawPixelRaw(x, start_y-2, border_color);
            DrawPixelRaw(x, end_y  +2, border_color);
            DrawPixelRaw(x, start_y-3, border_color);
            DrawPixelRaw(x, end_y  +3, border_color);
            
            if (is_active) {
                DrawPixelRaw(x, start_y-3, border_color);
                DrawPixelRaw(x, end_y  +3, border_color);
                DrawPixelRaw(x, start_y-4, border_color);
                DrawPixelRaw(x, end_y  +4, border_color);
                DrawPixelRaw(x, start_y-5, border_color);
                DrawPixelRaw(x, end_y  +5, border_color);
                shadow_start_y = 6;
            }
            else {
                shadow_start_y = 4;
            }

            for (shadow_y=shadow_start_y; shadow_y <shadow_start_y+shadow_size; shadow_y++) {
                shadow_color = MakeColor(0, 0, 0, shadow_alpha_base * (1.0 - (float)(shadow_y - shadow_start_y) / ((float)shadow_size)));
                shadow_inset =(shadow_y-shadow_start_y);

                if (x >= border_start_x + shadow_inset && x <= border_end_x - shadow_inset) {
                    DrawPixel(x, end_y  +shadow_y, shadow_color);
                }
            }
        }

        for (y=border_start_y; y<border_end_y; y++) {
            DrawPixelRaw(start_x  , y, border_color);
            DrawPixelRaw(end_x    , y, border_color);
            DrawPixelRaw(start_x-1, y, border_color);
            DrawPixelRaw(end_x  +1, y, border_color);
            DrawPixelRaw(start_x-2, y, border_color);
            DrawPixelRaw(end_x  +2, y, border_color);
            DrawPixelRaw(start_x-3, y, border_color);
            DrawPixelRaw(end_x  +3, y, border_color);

            if (is_active) {
                DrawPixelRaw(start_x-4, y, border_color);
                DrawPixelRaw(end_x  +4, y, border_color);
                DrawPixelRaw(start_x-5, y, border_color);
                DrawPixelRaw(end_x  +5, y, border_color);
            }
        }
    //}

    for (x=start_x; x<end_x; x++) {
        for (y=start_y; y<end_y; y++) {
            DrawPixelRaw(x, y, MakeColor(255, 255, 255, 255));
        }
    }

    if (me->icon_gfx_small && me->icon_gfx) {
        smallimg = me->icon_gfx_small;
        largeimg = me->icon_gfx;
    }
    else if (me->type == ENTRY_TYPE_FOLDER) {
        smallimg = folder_icon_small;
        largeimg = folder_icon_bin;
    }
    else {
        smallimg = switchicon_questionmark_small;
        largeimg = switchicon_questionmark_bin;
    }

    if (smallimg) {
        drawImage(start_x, start_y + 32, 140, 140, smallimg, IMAGE_MODE_RGB24);
    }

    if (is_active && largeimg) {
        drawImage(220, 100, 256, 256, largeimg, IMAGE_MODE_RGB24);

        shadow_start_y = 100+256;
        border_start_x = 220;
        border_end_x = 220+256;

        for (x=border_start_x; x<border_end_x; x++) {
            for (shadow_y=shadow_start_y; shadow_y <shadow_start_y+shadow_size; shadow_y++) {
                shadow_color = MakeColor(0, 0, 0, shadow_alpha_base * (1.0 - (float)(shadow_y - shadow_start_y) / ((float)shadow_size)));
                shadow_inset =(shadow_y-shadow_start_y);

                if (x >= border_start_x + shadow_inset && x <= border_end_x - shadow_inset) {
                    DrawPixel(x, shadow_y, shadow_color);
                }
            }
        }
    }

    DrawTextTruncate(interuiregular14, start_x + 8, start_y + 8, MakeColor(64, 64, 64, 255), me->name, 140 - 32, "...");

    if (is_active) {
        start_x = 220;
        start_y = 135;

        DrawText(interuimedium30, start_x + 256 + 64, start_y, themeCurrent.textColor, me->name);

        if (me->type != ENTRY_TYPE_FOLDER) {
            memset(tmpstr, 0, sizeof(tmpstr));
            snprintf(tmpstr, sizeof(tmpstr)-1, "Author: %s", me->author);
            DrawText(interuiregular14, start_x + 256 + 64, start_y + 28 + 30, themeCurrent.textColor, tmpstr);
            memset(tmpstr, 0, sizeof(tmpstr));
            snprintf(tmpstr, sizeof(tmpstr)-1, "Version: %s", me->version);
            DrawText(interuiregular14, start_x + 256 + 64, start_y + 28 + 30 + 18 + 6, themeCurrent.textColor, tmpstr);  
                         
            drawImage(1280 - 126 - 30, 720 - 48, 32, 32, themeCurrent.buttonAImage, IMAGE_MODE_RGBA32);
            DrawText(interuiregular18, 1280 - 90 - 30, 720 - 47, themeCurrent.textColor, "Launch");
        }
        else {
            drawImage(1280 - 126 - 30, 720 - 48, 32, 32, themeCurrent.buttonAImage, IMAGE_MODE_RGBA32);
            DrawText(interuiregular18, 1280 - 90 - 30, 720 - 47, themeCurrent.textColor, "Open");
        }
    }
}

void menuStartup() {
    const char *path;

    #ifdef SWITCH
    path = "sdmc:/switch";
    #else
    path = "switch";
    #endif

    menuScan(path);

    folder_icon_small = downscaleIcon(folder_icon_bin);
    switchicon_questionmark_small = downscaleIcon(switchicon_questionmark_bin);
}

color_t waveBlendAdd(color_t a, color_t b, float alpha) {
    return MakeColor(a.r+(b.r*alpha), a.g+b.g*alpha, a.b + b.b*alpha, 255);
}

double timer;

void drawWave(float timer, color_t color, float height, float phase, float speed) {
    int x, y;
    float wave_top_y, alpha;
    float dark_mult, dark_sub = 75;
    color_t existing_color, new_color;
    
    height = 720 - height;

    for (x=0; x<1280; x++) {
        wave_top_y = approxSin(x*speed/1280.0+timer+phase) * 10.0 + height;

        for (y=wave_top_y; y<720; y++) {
            alpha = y-wave_top_y;
            existing_color = FetchPixelColor(x, y);

            if (themeCurrent.enableWaveBlending) {
                new_color = waveBlendAdd(existing_color, color, clamp(alpha, 0.0, 1.0) * 0.3);
            }
            else if (alpha < 0.3) { // anti-aliasing
                alpha = fabs(alpha);
                new_color = MakeColor(color.r * (1.0 - alpha) + existing_color.r * alpha, color.g * (1.0 - alpha) + existing_color.g * alpha, color.b * (1.0 - alpha) + existing_color.b * alpha, 255);
            }
            else { // darken closer to bottom of the waves
                dark_mult = clamp((alpha - 50) / height, 0.0, 1.0);
                new_color = MakeColor(color.r - dark_sub * dark_mult, color.g - dark_sub * dark_mult, color.b - dark_sub * dark_mult, 255);
            }

            DrawPixelRaw(x, y, new_color);
        }
    }
}

void drawTime() {

    char timeString[9];
	
    time_t unixTime = time(NULL);
    struct tm* timeStruct = gmtime((const time_t *)&unixTime);
	
    int hours = timeStruct->tm_hour;
    int minutes = timeStruct->tm_min;
    int seconds = timeStruct->tm_sec;
	
    sprintf(timeString, "%02d:%02d:%02d", hours, minutes, seconds);

    DrawText(interuimedium20, 1280 - (9 * 16) - 30, 30, MakeColor(255, 255, 255, 255), timeString);
	
}

void menuLoop() {
    menuEntry_s* me;
    menu_s* menu = menuGetCurrent();
    int i;
    int x, y;

    for (x=0; x<1280; x++) {
        for (y=0; y<720; y++) {
            DrawPixelRaw(x, y, themeCurrent.backgroundColor);
        }
    }

    drawWave(timer, themeCurrent.backWaveColor, 295.0, 0.0, 3.0);
    drawWave(timer, themeCurrent.middleWaveColor, 290.0, 2.0, 3.5);
    drawWave(timer, themeCurrent.frontWaveColor, 280.0, 4.0, -2.5);
    timer += 0.05;

    drawImage(40, 20, 140, 60, themeCurrent.hbmenuLogoImage, IMAGE_MODE_RGBA32);
    DrawText(interuiregular14, 180, 46, themeCurrent.textColor, "v2.0.0");
    DrawText(interuiregular18, 40, 720 - 47, themeCurrent.textColor, menu->dirname);

    //drawTime();

    if (menu->nEntries==0)
    {
        DrawText(interuiregular14, 64, 128, themeCurrent.textColor, textGetString(StrId_NoAppsFound_Msg));
    }
    else
    {
        static int x = 0;
        static int v = 0;

        if (menu->nEntries > 7) {
            int wanted_x = clamp(-menu->curEntry * (140 + 30), -(menu->nEntries - 7) * (140 + 30), 0);
            x += v;
            v += (wanted_x - x) / 3;
            v /= 2;
        }

        // Draw menu entries
        for (me = menu->firstEntry, i = 0; me; me = me->next, i ++) {
            int entry_start_x = 29 + i * (140 + 30);

            int screen_width = 1280;
            if (entry_start_x >= (screen_width - x))
                break;

            drawEntry(me, entry_start_x + x, i==menu->curEntry);
        }
    }
}
