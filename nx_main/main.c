#include <switch.h>
#include <string.h>
#include <stdio.h>

#include "../common/common.h"
#include "nx_touch.h"

uint8_t* g_framebuf;
u32 g_framebuf_width;

#ifdef PERF_LOG
u64 g_tickdiff_vsync=0;
u64 g_tickdiff_frame=0;
#endif

#ifdef ENABLE_AUDIO
void audio_initialize(void);
void audio_exit(void);
#endif

char* getSavedTheme();

int main(int argc, char **argv)
{
    Result lastret=0;
    Result rc=0;
    char msg[256];

    #ifdef PERF_LOG
    u64 start_tick=0;
    #endif

    gfxInitDefault();

    appletSetScreenShotPermission(1);

    ColorSetId theme;
    
    rc = setsysInitialize();
    if (R_FAILED(rc)) fatalSimple(-5);

    setsysGetColorSetId(&theme);

    rc = plInitialize();
    if (R_FAILED(rc)) fatalSimple(-6);
    //char* savedThemeName = getSavedTheme(); 
    themeStartup((ThemePreset)theme);
    textInit();
    menuStartup();

    launchInit();
    if (!fontInitialize()) fatalSimple(-7);

    #ifdef ENABLE_AUDIO
    audio_initialize();
    #endif

    lastret = envGetLastLoadResult();

    if (R_FAILED(lastret)) {
        memset(msg, 0, sizeof(msg));
        snprintf(msg, sizeof(msg)-1, "%s\n0x%x", textGetString(StrId_LastLoadResult), lastret);

        menuCreateMsgBox(780, 300, msg);
    }

    #ifdef PERF_LOG
    gfxWaitForVsync();

    start_tick = svcGetSystemTick();
    gfxWaitForVsync();
    g_tickdiff_vsync = svcGetSystemTick() - start_tick;
    #endif

    while (appletMainLoop())
    {
        #ifdef PERF_LOG
        start_tick = svcGetSystemTick();
        #endif

        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        g_framebuf = gfxGetFramebuffer(&g_framebuf_width, NULL);
        memset(g_framebuf, 237, gfxGetFramebufferSize());
        if (!uiUpdate()) break;
        menuLoop();

        gfxFlushBuffers();

        #ifdef PERF_LOG
        g_tickdiff_frame = svcGetSystemTick() - start_tick;
        #endif

        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    #ifdef ENABLE_AUDIO
    audio_exit();
    #endif

    fontExit();
    launchExit();
    plExit();
    setsysExit();

    gfxExit();
    return 0;
}

//This is implemented here due to the hid code.
bool menuUpdate(void) {
    bool exitflag = 0;
    menu_s* menu = menuGetCurrent();
    u32 down = hidKeysDown(CONTROLLER_P1_AUTO);
    handleTouch(menu);

    if (down & KEY_Y)
    {
        launchMenuNetloaderTask();
    }
    else if (down & KEY_A)
    {
        if (menuIsMsgBoxOpen()) {
            menuCloseMsgBox();
        }
        else if (menu->nEntries > 0)
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
        launchMenuBackTask();
    }
    else if(down & KEY_MINUS){
        themeMenuStartup();
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



char* getSavedTheme(){
    char tmp_path[PATH_MAX];
    #ifdef __SWITCH__
    strcpy(tmp_path,"sdmc:");
    #else
    getcwd(tmp_path, PATH_MAX);
    #endif
    snprintf(tmp_path, sizeof(tmp_path)-1, "%s%s%s%s%s%s%s", DIRECTORY_SEPARATOR, "config", DIRECTORY_SEPARATOR, "nx-hbmenu" , DIRECTORY_SEPARATOR, "themes", DIRECTORY_SEPARATOR, "theme.saved");
    FILE* f;
    if((f= fopen(tmp_path, "rb"))==NULL) return NULL; //return null if FAILED TO OPEN FILE
    char* buffer;
    buffer = calloc(1, PATH_MAX+1) ;//calloc already contains zeros
    if( 1!=fread(buffer,PATH_MAX, 1, f) ) return NULL;// return null if FAILED TO READ
    fclose(f);
    return buffer;

}