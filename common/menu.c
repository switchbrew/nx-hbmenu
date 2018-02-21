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

//Draws a RGB888 image.
static void drawImage(int x, int y, int width, int height, const uint8_t *image) {
    int tmpx, tmpy;
    int pos;

    for (tmpx=0; tmpx<width; tmpx++) {
        for (tmpy=0; tmpy<height; tmpy++) {
            pos = ((tmpy*width) + tmpx) * 3;
            DrawPixelRaw(x+tmpx, y+tmpy, MakeColor(image[pos+0], image[pos+1], image[pos+2], 255));
        }
    }
}

uint8_t *folder_icon_small;
uint8_t *switchicon_questionmark_small;

static void drawEntry(menuEntry_s* me, int n, int is_active) {
    int x, y;
    int start_y = 720 - 100 - 140;//*(n % 2);
    int end_y = start_y + 140 + 32;
    int start_x = 64 + (140 + 40) * n;//(n / 2);
    int end_x = start_x + 140;
    const uint8_t *smallimg = NULL;
    const uint8_t *largeimg = NULL;
    char tmpstr[1024];

    color_t border_color = MakeColor(255, 255, 255, 255);

    if (is_active) {
        border_color = MakeColor(73, 103, 169, 255);
    }

    //{
        for (x=(start_x-5); x<(end_x+5); x++) {
            if (!is_active) {
                if (x < start_x-3)
                    continue;
                else if (x >= end_x+3)
                    break;
            }

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
            }
        }

        for (y=(start_y-5); y<(end_y+5); y++) {
            if (!is_active) {
                if (y < start_y-3)
                    continue;
                else if (y >= end_y+3)
                    break;
            }
            
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
        drawImage(start_x, start_y + 32, 140, 140, smallimg);
    }

    if (is_active && largeimg) {
        drawImage(220, 100, 256, 256, largeimg);
    }

    DrawTextTruncate(tahoma12, start_x + 8, start_y + 8, MakeColor(64, 64, 64, 255), me->name, 256 - 32, "...");

    if (is_active) {
        start_x = 220;
        start_y = 135;

        DrawText(tahoma24, start_x + 256 + 64, start_y, MakeColor(255, 255, 255, 255), me->name);
        memset(tmpstr, 0, sizeof(tmpstr));
        snprintf(tmpstr, sizeof(tmpstr)-1, "Author: %s", me->author);
        DrawText(tahoma12, start_x + 256 + 64, start_y + 28 + 30, MakeColor(255, 255, 255, 255), tmpstr);
        memset(tmpstr, 0, sizeof(tmpstr));
        snprintf(tmpstr, sizeof(tmpstr)-1, "Version: %s", me->version);
        DrawText(tahoma12, start_x + 256 + 64, start_y + 28 + 30 + 18 + 6, MakeColor(255, 255, 255, 255), tmpstr);
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

const int ENABLE_WAVE_BLENDING = 0;
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

            if (ENABLE_WAVE_BLENDING || alpha < 1.0) {
                new_color = waveBlendAdd(existing_color, color, clamp(alpha, 0.0, 1.0) * 0.3);
            }
            else {
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

    DrawText(tahoma24, 1280 - (9 * 16) - 30, 30, MakeColor(255, 255, 255, 255), timeString);
	
}

void menuLoop() {
    menuEntry_s* me;
    menu_s* menu = menuGetCurrent();
    int i;
    int cnt=0;
    int x, y;

    for (x=0; x<1280; x++) {
        for (y=0; y<720; y++) {
            DrawPixelRaw(x, y, MakeColor(45, 55, 66, 255));
        }
    }

    drawWave(timer, MakeColor(73, 103, 169, 255), 320.0, 0.0, 3.0);
    drawWave(timer, MakeColor(66, 154, 159, 255), 300.0, 2.0, 3.5);
    drawWave(timer, MakeColor(96, 204, 204, 255), 280.0, 4.0, -2.5);
    timer += 0.05;

    DrawText(tahoma24, 40, 30, MakeColor(255, 255, 255, 255), "hbl");
    DrawText(tahoma12, 40 + 46, 30 + 16, MakeColor(255, 255, 255, 255), "v1.0.0");
    DrawText(tahoma12, 40, 720 - 32 - 16, MakeColor(255, 255, 255, 255), menu->dirname);

	drawTime();

    if (menu->nEntries==0)
    {
        DrawText(tahoma12, 64, 96 + 32, MakeColor(64, 64, 64, 255), textGetString(StrId_NoAppsFound_Msg));
    } else
    {
        // Draw menu entries
        for (me = menu->firstEntry, i = 0; me; me = me->next, i ++) {
            if ((i < menu->curEntry && menu->curEntry-i < 4) || i>=menu->curEntry) {
                drawEntry(me, cnt, i==menu->curEntry);
                cnt++;
                if (cnt==4) break;
            }
        }
    }
}
