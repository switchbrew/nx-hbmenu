#include <switch.h>
#include <string.h>
#include <stdio.h>

#include "../common/common.h"
#include "nx_graphics.h"
#include "nx_touch.h"

// Define the desired framebuffer resolution (here we set it to 720p).
#define FB_WIDTH  1280
#define FB_HEIGHT 720

uint8_t* g_framebuf;
u32 g_framebuf_width;

bool menuUpdateErrorScreen(void);

#ifdef PERF_LOG
u64 g_tickdiff_frame=0;
#endif

#ifdef ENABLE_AUDIO
void audio_initialize(void);
void audio_exit(void);
#endif

extern u32 __nx_applet_exit_mode;

int main(int argc, char **argv)
{
    bool error_screen=0;
    Result lastret=0;
    Result rc=0;
    char msg[256];
    char errormsg[256];//Can't use StrId for these error messages since it would be unavailable if textInit fails.

    #ifdef PERF_LOG
    u64 start_tick=0;
    #endif

    memset(errormsg, 0, sizeof(errormsg));

    appletLockExit();
    appletSetScreenShotPermission(AppletScreenShotPermission_Enable);

    ColorSetId theme = ColorSetId_Light;
    rc = setsysInitialize();
    if (R_SUCCEEDED(rc)) {
        setsysGetColorSetId(&theme);
        setsysExit();
    }

    if (R_SUCCEEDED(rc)) {
        rc = textInit();
        if (R_FAILED(rc)) {
            snprintf(errormsg, sizeof(errormsg)-1, "Error: textInit() failed: 2%03d-%04d", R_MODULE(rc), R_DESCRIPTION(rc));
        }
    }

    if (R_SUCCEEDED(rc)) menuStartupPath();

    if (R_SUCCEEDED(rc)) {
        rc = assetsInit();
        if (R_FAILED(rc)) {
            snprintf(errormsg, sizeof(errormsg)-1, "Error: assetsInit() failed: 2%03d-%04d", R_MODULE(rc), R_DESCRIPTION(rc));
        }
    }

    if (R_SUCCEEDED(rc)) themeStartup((ThemePreset)theme);

    if (R_SUCCEEDED(rc)) powerInit();

    if (R_SUCCEEDED(rc)) {
        rc = netloaderInit();
        if (R_FAILED(rc)) {
            snprintf(errormsg, sizeof(errormsg)-1, "Error: netloaderInit() failed: 2%03d-%04d", R_MODULE(rc), R_DESCRIPTION(rc));
        }
    }

    if (R_SUCCEEDED(rc) && !workerInit()) {
        rc = 1;
        snprintf(errormsg, sizeof(errormsg)-1, "Error: workerInit() failed.");
    }

    if (R_SUCCEEDED(rc) && !statusInit()) {
        rc = 1;
        snprintf(errormsg, sizeof(errormsg)-1, "Error: statusInit() failed.");
    }

    if (R_SUCCEEDED(rc)) menuStartup();

    if (R_SUCCEEDED(rc)) {
        if (!launchInit()) {
            rc = 2;
            snprintf(errormsg, sizeof(errormsg)-1, "Error: launchInit() failed.");
        }
    }

    if (R_SUCCEEDED(rc) && !fontInitialize()) {
        rc = 3;
        snprintf(errormsg, sizeof(errormsg)-1, "Error: fontInitialize() failed.");
    }

    #ifdef ENABLE_AUDIO
    if (R_SUCCEEDED(rc)) audio_initialize();
    #endif

    if (R_SUCCEEDED(rc)) {
        lastret = envGetLastLoadResult();

        if (R_FAILED(lastret)) {
            memset(msg, 0, sizeof(msg));
            snprintf(msg, sizeof(msg)-1, "%s\n2%03d-%04d", textGetString(StrId_LastLoadResult), R_MODULE(lastret), R_DESCRIPTION(lastret));

            menuCreateMsgBox(780, 300, msg);
        }
    }

    if (errormsg[0]) error_screen = 1;

    if (!error_screen) {
        graphicsInit(FB_WIDTH, FB_HEIGHT);
    }
    else {
        consoleInit(NULL);
        printf("%s\n", errormsg);
        printf("Press the + button to exit.\n");
    }

    while (appletMainLoop())
    {


        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        if (!error_screen) {
            if (!uiUpdate()) break;
            g_framebuf = graphicsFrameBegin(&g_framebuf_width);
            #ifdef PERF_LOG
            start_tick = armGetSystemTick();
            #endif
            memset(g_framebuf, 237, g_framebuf_width * FB_HEIGHT);
            menuLoop();
        }
        else {
            if (menuUpdateErrorScreen()) break;
        }

        if (!error_screen) {
            graphicsFrameEnd();

            #ifdef PERF_LOG
            g_tickdiff_frame = armGetSystemTick() - start_tick;
            #endif
        }
        else {
            consoleUpdate(NULL);
        }
    }

    if (!error_screen) {
        graphicsExit();
    }
    else {
        consoleExit(NULL);
        __nx_applet_exit_mode = 1;
    }

    #ifdef ENABLE_AUDIO
    audio_exit();
    #endif

    fontExit();
    launchExit();
    netloaderSignalExit();
    statusExit();
    workerExit();
    netloaderExit();
    powerExit();
    assetsExit();

    appletUnlockExit();

    return 0;
}

u64 menuGetKeysDown(void) {
    u64 down = 0;

    for (u32 controller=0; controller<8; controller++) {
        if (hidIsControllerConnected(controller)) down |= hidKeysDown(controller);
    }
    if (hidIsControllerConnected(CONTROLLER_HANDHELD)) down |= hidKeysDown(CONTROLLER_HANDHELD);

    return down;
}

//This is implemented here due to the hid code.
bool menuUpdate(void) {
    bool exitflag = 0;
    menu_s* menu = menuGetCurrent();
    u64 down = menuGetKeysDown();
    ThemeLayoutObject *layoutobj = &themeCurrent.layoutObjects[ThemeLayoutId_MenuListTiles];
    int entries_count = layoutobj->posEnd[0];

    handleTouch(menu);

    if (down & KEY_Y)
    {
        launchMenuNetloaderTask();
    }
    else if (down & KEY_X)
    {
        menuHandleXButton();
    }
    else if (down & KEY_A)
    {
        menuHandleAButton();
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
    else if (menu->nEntries > 0)
    {
        int move = 0;

        if (down & KEY_LEFT) move--;
        if (down & KEY_RIGHT) move++;
        if (down & KEY_DOWN) move-=entries_count;
        if (down & KEY_UP) move+=entries_count;

        int newEntry = menu->curEntry + move;
        if (newEntry < 0) newEntry = 0;
        if (newEntry >= menu->nEntries) newEntry = menu->nEntries-1;
        menu->curEntry = newEntry;
    }

    return exitflag;
}

bool menuUpdateErrorScreen(void) {
    bool exitflag = 0;
    u64 down = menuGetKeysDown();

    if (down & KEY_PLUS)
    {
        exitflag = 1;
    }

    return exitflag;
}
