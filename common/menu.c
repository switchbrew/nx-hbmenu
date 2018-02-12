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

static void drawEntry(menuEntry_s* me, int n, int is_active) {
    int x, y;
    int start_y = 96 + 304 - 32;//*(n % 2);
    int end_y = start_y + 288;
    int start_x = 64 + (256+16)*n;//(n / 2);
    int end_x = /*1280 - 64*/start_x + 256;
    uint8_t *imageptr = NULL;
    char tmpstr[1024];

    color_t border_color0 = MakeColor(255, 255, 255, 255);
    color_t border_color1 = MakeColor(255, 255, 255, 255);

    if (is_active) {
        border_color0 = MakeColor(171, 224, 245, 255);
        border_color1 = MakeColor(189, 228, 242, 255);
    }

    //{
        for (x=(start_x-3); x<(end_x+3); x++) {
            //DrawPixelRaw(x, start_y  , border_color0);
            //DrawPixelRaw(x, end_y    , border_color0);
            DrawPixelRaw(x, start_y-1, border_color0);
            DrawPixelRaw(x, end_y  +1, border_color0);
            DrawPixelRaw(x, start_y-2, border_color0);
            DrawPixelRaw(x, end_y  +2, border_color0);
            DrawPixelRaw(x, start_y-3, border_color1);
            DrawPixelRaw(x, end_y  +3, border_color1);
        }

        for (y=(start_y-3); y<(end_y+3); y++) {
            DrawPixelRaw(start_x  , y, border_color0);
            DrawPixelRaw(end_x    , y, border_color0);
            DrawPixelRaw(start_x-1, y, border_color0);
            DrawPixelRaw(end_x  +1, y, border_color0);
            DrawPixelRaw(start_x-2, y, border_color0);
            DrawPixelRaw(end_x  +2, y, border_color0);
            DrawPixelRaw(start_x-3, y, border_color1);
            //DrawPixelRaw(end_x  +3, y, border_color1);
        }
    //}

    for (x=start_x; x<end_x; x++) {
        for (y=start_y; y<end_y; y++) {
            DrawPixelRaw(x, y, MakeColor(255, 255, 255, 255));
        }
    }

    if (me->icon_gfx)
        imageptr = me->icon_gfx;
    else if (me->type == ENTRY_TYPE_FOLDER)
        imageptr = (uint8_t*)folder_icon_bin;
    else
        imageptr = (uint8_t*)switchicon_questionmark_bin;

    if (imageptr) drawImage(start_x, start_y+32, 256, 256, imageptr);

    DrawTextTruncate(tahoma12, start_x + 8, start_y + 8, MakeColor(64, 64, 64, 255), me->name, 256 - 32, "...");

    if (is_active) {
        start_x = 64;
        start_y = 96 + 32;

        memset(tmpstr, 0, sizeof(tmpstr));
        snprintf(tmpstr, sizeof(tmpstr)-1, "Name: %s\nAuthor: %s\nVersion: %s", me->name, me->author, me->version);
        DrawText(tahoma12, start_x, start_y, MakeColor(64, 64, 64, 255), tmpstr);
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
}

void menuLoop() {
    menuEntry_s* me;
    menu_s* menu = menuGetCurrent();
    int i;
    int cnt=0;
    int x, y;

    for (x=0; x<1280; x++) {
        for (y=0; y<122; y++) {
            DrawPixelRaw(x, y, MakeColor(237+8, 237+8, 237+8, 237+8));
        }
    }

    DrawText(tahoma24, 64, 64, MakeColor(64, 64, 64, 255), "The Homebrew Launcher");
    DrawText(tahoma12, 64 + 256 + 128 + 128, 64 + 16, MakeColor(64, 64, 64, 255), "v1.0.0");
    DrawText(tahoma12, 64 + 256 + 128 + 128, 64 + 16 + 16, MakeColor(64, 64, 64, 255), menu->dirname);

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
