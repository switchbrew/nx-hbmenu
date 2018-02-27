#include <switch.h>
#include <string.h>
#include <stdio.h>

#include "../common/common.h"

uint8_t* g_framebuf;
u32 g_framebuf_width;

int main(int argc, char **argv)
{
    gfxInitDefault();

    appletSetScreenShotPermission(1);

    ColorSetId theme;
    setsysInitialize();
    setsysGetColorSetId(&theme);
    themeStartup((ThemePreset)theme);
    textInit();
    menuStartup();

    launchInit();

    while (appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        g_framebuf = gfxGetFramebuffer(&g_framebuf_width, NULL);
        memset(g_framebuf, 237, gfxGetFramebufferSize());
        if (!uiUpdate()) break;
        menuLoop();

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    launchExit();
    setsysExit();

    gfxExit();
    return 0;
}

void launchMenuEntryTask(menuEntry_s* arg);

//This is implemented here due to the hid code.
bool menuUpdate(void) {
    bool exitflag = 0;
    menu_s* menu = menuGetCurrent();
    u32 down = hidKeysDown(CONTROLLER_P1_AUTO);

    if (down & KEY_A)
    {
        if (menu->nEntries > 0)
        {
            int i;
            menuEntry_s* me;
            for (i = 0, me = menu->firstEntry; i != menu->curEntry; i ++, me = me->next);
            launchMenuEntryTask(me);
            //workerSchedule(launchMenuEntryTask, me);
        }
    }
    else if (down & KEY_B)
    {
        if (strcmp( menu->dirname, "sdmc:/") != 0)
        {
            //workerSchedule(changeDirTask, "..");
            menuScan("..");
        }
    }
    else if (down & KEY_PLUS)
    {
        exitflag = 1;
    }
    /*else if (down & KEY_Y)
    {
        workerSchedule(netloaderTask, NULL);
    }*/
    else if (menu->nEntries > 0)
    {
        int move = 0;

        if (down & KEY_LEFT) move--;
        if (down & KEY_RIGHT) move++;
        if (down & KEY_DOWN) move-=7;
        if (down & KEY_UP) move+=7;

        int newEntry = menu->curEntry + move;
        if (newEntry < 0) newEntry = 0;
        if (newEntry >= menu->nEntries) newEntry = menu->nEntries-1;
        menu->curEntry = newEntry;
    }

    return exitflag;
}
