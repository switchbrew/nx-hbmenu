#include <time.h>
#include "common.h"

#include "invalid_icon_bin.h"
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

    for (tmpy=0; tmpy<height; tmpy++) {
        for (tmpx=0; tmpx<width; tmpx++) {
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
uint8_t *invalid_icon_small;
double timer;

static void drawEntry(menuEntry_s* me, int off_x, int is_active) {
    int x, y;
    int start_y = 720 - 100 - 145;//*(n % 2);
    int end_y = start_y + 140 + 32;
    int start_x = off_x;//(n / 2);
    int end_x = start_x + 140;
    int j;

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
    float highlight_multiplier;
    int shadow_size = 4;

    if (is_active) {
        highlight_multiplier = fmax(0.0, fabs(fmod(timer, 1.0) - 0.5) / 0.5);
        border_color = MakeColor(themeCurrent.highlightColor.r + (255 - themeCurrent.highlightColor.r) * highlight_multiplier, themeCurrent.highlightColor.g + (255 - themeCurrent.highlightColor.g) * highlight_multiplier, themeCurrent.highlightColor.b + (255 - themeCurrent.highlightColor.b) * highlight_multiplier, 255);
        border_start_x = start_x-6;
        border_end_x = end_x+6;
        border_start_y = start_y-5;
        border_end_y = end_y+5;
    }
    else {
        border_start_x = start_x-4;
        border_end_x = end_x+4;
        border_start_y = start_y-3;
        border_end_y = end_y+3;
    }

    //{
        for (x=border_start_x; x<border_end_x; x+=4) {
            Draw4PixelsRaw(x, end_y    , border_color);
            Draw4PixelsRaw(x, start_y-1, border_color);
            Draw4PixelsRaw(x, end_y  +1, border_color);
            Draw4PixelsRaw(x, start_y-2, border_color);
            Draw4PixelsRaw(x, end_y  +2, border_color);
            Draw4PixelsRaw(x, start_y-3, border_color);
            Draw4PixelsRaw(x, end_y  +3, border_color);
            
            if (is_active) {
                Draw4PixelsRaw(x, start_y-3, border_color);
                Draw4PixelsRaw(x, end_y  +3, border_color);
                Draw4PixelsRaw(x, start_y-4, border_color);
                Draw4PixelsRaw(x, end_y  +4, border_color);
                Draw4PixelsRaw(x, start_y-5, border_color);
                Draw4PixelsRaw(x, end_y  +5, border_color);
                shadow_start_y = 6;
            }
            else {
                shadow_start_y = 4;
            }

            for (shadow_y=shadow_start_y; shadow_y < shadow_start_y+shadow_size; shadow_y++) {
                shadow_color = MakeColor(0, 0, 0, shadow_alpha_base * (1.0 - (float)(shadow_y - shadow_start_y) / ((float)shadow_size)));
                shadow_inset =(shadow_y-shadow_start_y);

                if (x >= border_start_x + shadow_inset && x < border_end_x - shadow_inset) {
                    for (j=0; j<4; j++) DrawPixel(x+j, end_y  +shadow_y, shadow_color);
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
            DrawPixelRaw(start_x-4, y, border_color);

            if (is_active) {
                DrawPixelRaw(end_x  +4, y, border_color);
                DrawPixelRaw(start_x-5, y, border_color);
                DrawPixelRaw(end_x  +5, y, border_color);
                DrawPixelRaw(start_x-6, y, border_color);
            }
        }
    //}

    for (y=start_y; y<end_y; y++) {
        for (x=start_x; x<end_x; x+=4) {
            Draw4PixelsRaw(x, y, MakeColor(255, 255, 255, 255));
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
        smallimg = invalid_icon_small;
        largeimg = invalid_icon_bin;
    }

    if (smallimg) {
        drawImage(start_x, start_y + 32, 140, 140, smallimg, IMAGE_MODE_RGB24);
    }

    if (is_active && largeimg) {
        drawImage(117, 100, 256, 256, largeimg, IMAGE_MODE_RGB24);

        shadow_start_y = 100+256;
        border_start_x = 117;
        border_end_x = 117+256;

        for (shadow_y=shadow_start_y; shadow_y <shadow_start_y+shadow_size; shadow_y++) {
            for (x=border_start_x; x<border_end_x; x++) {
                shadow_color = MakeColor(0, 0, 0, shadow_alpha_base * (1.0 - (float)(shadow_y - shadow_start_y) / ((float)shadow_size)));
                shadow_inset =(shadow_y-shadow_start_y);

                if (x >= border_start_x + shadow_inset && x <= border_end_x - shadow_inset) {
                    DrawPixel(x, shadow_y, shadow_color);
                }
            }
        }
    }

    DrawTextTruncate(interuiregular14, start_x + 4, start_y + 4, MakeColor(64, 64, 64, 255), me->name, 140 - 32, "...");

    if (is_active) {
        start_x = 1280 - 790;
        start_y = 135;

        DrawTextTruncate(interuimedium30, start_x, start_y, themeCurrent.textColor, me->name, 1280 - start_x - 120 ,"...");

        if (me->type != ENTRY_TYPE_FOLDER) {
            memset(tmpstr, 0, sizeof(tmpstr));
            snprintf(tmpstr, sizeof(tmpstr)-1, "%s: %s", textGetString(StrId_AppInfo_Author), me->author);
            DrawText(interuiregular14, start_x, start_y + 28 + 30, themeCurrent.textColor, tmpstr);
            memset(tmpstr, 0, sizeof(tmpstr));
            snprintf(tmpstr, sizeof(tmpstr)-1, "%s: %s", textGetString(StrId_AppInfo_Version), me->version);
            DrawText(interuiregular14, start_x, start_y + 28 + 30 + 18 + 6, themeCurrent.textColor, tmpstr);  
        }
    }
}

color_t frontWaveGradient[720];

void computeFrontGradient(color_t baseColor, int height) {
    int y;
    int alpha;
    float dark_mult, dark_sub = 75;
    color_t color;

    for (y=0; y<720; y++) {
        alpha = y - (720 - height);

        if (alpha < 0)
            color = baseColor;
        else {
            dark_mult = clamp((float)(alpha - 50) / (float)height, 0.0, 1.0);
            color = MakeColor(baseColor.r - dark_sub * dark_mult, baseColor.g - dark_sub * dark_mult, baseColor.b - dark_sub * dark_mult, 255);
        }

        frontWaveGradient[y] = color;
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
    invalid_icon_small = downscaleIcon(invalid_icon_bin);
    computeFrontGradient(themeCurrent.frontWaveColor, 280);
}

color_t waveBlendAdd(color_t a, color_t b, float alpha) {
    return MakeColor(a.r+(b.r*alpha), a.g+b.g*alpha, a.b + b.b*alpha, 255);
}

void drawWave(int id, float timer, color_t color, int height, float phase, float speed) {
    int x, y;
    float wave_top_y, alpha, one_minus_alpha;
    color_t existing_color, new_color;
    
    height = 720 - height;

    for (x=0; x<1280; x++) {
        wave_top_y = approxSin(x*speed/1280.0+timer+phase) * 10.0 + height;

        for (y=wave_top_y; y<720; y++) {
            if (id != 2 && y > wave_top_y + 30) 
                break;

            alpha = y-wave_top_y;

            if (themeCurrent.enableWaveBlending) {
                existing_color = FetchPixelColor(x, y);
                new_color = waveBlendAdd(existing_color, color, clamp(alpha, 0.0, 1.0) * 0.3);
            }      
            else if (alpha >= 0.3) { 
                if (id == 2) {
                    new_color = frontWaveGradient[y];
                } 
                else { // no anti-aliasing
                    new_color = color;
                }
            }
            else { // anti-aliasing
                existing_color = FetchPixelColor(x, y);
                alpha = fabs(alpha);
                one_minus_alpha = (1.0 - alpha);
                new_color = MakeColor(color.r * one_minus_alpha + existing_color.r * alpha, color.g * one_minus_alpha + existing_color.g * alpha, color.b * one_minus_alpha + existing_color.b * alpha, 255);
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

void drawBackBtn(menu_s* menu, bool emptyDir) {
    int x_image = 1280 - 252 - 30 - 32;
    int x_text = 1280 - 216 - 30 - 32;
	
    if(emptyDir) {
        x_image = 1280 - 126 - 30 - 32;
         x_text = 1280 - 90 - 30 - 32;
    }
    
    #ifdef SWITCH
    if (strcmp( menu->dirname, "sdmc:/") != 0)
    #else
    if (strcmp( menu->dirname, "/") != 0)
    #endif
    {
        drawImage(x_image, 720 - 48, 32, 32, themeCurrent.buttonBImage, IMAGE_MODE_RGBA32);
        DrawText(interuiregular18, x_text, 720 - 47, themeCurrent.textColor, textGetString(StrId_Actions_Back));
    }
}

void menuLoop() {
    menuEntry_s* me;
    menu_s* menu = menuGetCurrent();
    int i;
    int x, y;

    for (y=0; y<450; y++) {
        for (x=0; x<1280; x+=4) {// don't draw bottom pixels as they are covered by the waves
            Draw4PixelsRaw(x, y, themeCurrent.backgroundColor);
        }
    }

    drawWave(0, timer, themeCurrent.backWaveColor, 295, 0.0, 3.0);
    drawWave(1, timer, themeCurrent.middleWaveColor, 290, 2.0, 3.5);
    drawWave(2, timer, themeCurrent.frontWaveColor, 280, 4.0, -2.5);
    timer += 0.05;

    drawImage(40, 20, 140, 60, themeCurrent.hbmenuLogoImage, IMAGE_MODE_RGBA32);
    DrawText(interuiregular14, 180, 46, themeCurrent.textColor, VERSION);
    DrawTextTruncate(interuiregular18, 40, 720 - 47, themeCurrent.textColor, menu->dirname, 918, "...");

    #ifdef PERF_LOG_DRAW//Seperate from the PERF_LOG define since this might affect perf.
    extern u64 g_tickdiff_vsync;
    extern u64 g_tickdiff_frame;

    char tmpstr[64];

    snprintf(tmpstr, sizeof(tmpstr)-1, "%lu", g_tickdiff_vsync);
    DrawText(interuiregular14, 180 + 256, 46, themeCurrent.textColor, tmpstr);

    snprintf(tmpstr, sizeof(tmpstr)-1, "%lu", g_tickdiff_frame);
    DrawText(interuiregular14, 180 + 256, 46 + 16, themeCurrent.textColor, tmpstr);
    #endif

    //drawTime();

    if (menu->nEntries==0)
    {
        DrawText(interuiregular14, 64, 128, themeCurrent.textColor, textGetString(StrId_NoAppsFound_Msg));
        drawBackBtn(menu, true);
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
        else {
            x = v = 0;
        }

        menuEntry_s *active_entry = NULL;

        // Draw menu entries
        for (me = menu->firstEntry, i = 0; me; me = me->next, i ++) {
            int entry_start_x = 29 + i * (140 + 30);

            int screen_width = 1280;
            if (entry_start_x >= (screen_width - x))
                break;

            int is_active = i==menu->curEntry;

            if (is_active)
                active_entry = me;

            drawEntry(me, entry_start_x + x, is_active);
        }

        if(active_entry != NULL) {
            if (active_entry->type != ENTRY_TYPE_FOLDER) {
                drawImage(1280 - 126 - 30 - 32, 720 - 48, 32, 32, themeCurrent.buttonAImage, IMAGE_MODE_RGBA32);
                DrawText(interuiregular18, 1280 - 90 - 30 - 32, 720 - 47, themeCurrent.textColor, textGetString(StrId_Actions_Launch));
            }
            else {
                drawImage(1280 - 126 - 30 - 32, 720 - 48, 32, 32, themeCurrent.buttonAImage, IMAGE_MODE_RGBA32);
                DrawText(interuiregular18, 1280 - 90 - 30 - 32, 720 - 47, themeCurrent.textColor, textGetString(StrId_Actions_Open));
            }
        }

        drawBackBtn(menu, false);
    }
}
