#include <switch.h>
#include <string.h>
#include <stdio.h>

#include "../common/common.h"
#include "nx_touch.h"

// Define the desired framebuffer resolution (here we set it to 720p).
#define FB_WIDTH  1280
#define FB_HEIGHT 720

Framebuffer g_framebufObj;

uint8_t* g_framebuf;
u32 g_framebuf_width;

bool menuUpdateErrorScreen(void);

u64 g_prev_keyheld = 0;
u64 g_curr_keyheld = 0;
u64 g_tick_prev_keyheld = 0;
u64 g_tick_keyheld_interval = 0;
u64 g_tick_keyheld_subsequent_interval = 0;
bool g_prev_elapsed = false;

void intializeKeyHeldInterval(u16, u16);

// These wait times are idenfied with trial and error
#define KEY_HELD_INITIAL_WAIT_TIME  400
#define KEY_HELD_SUBSEQUENT_WAIT_TIME  33

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

    ColorSetId theme;
    rc = setsysInitialize();
    if (R_FAILED(rc)) snprintf(errormsg, sizeof(errormsg)-1, "Error: setsysInitialize() failed: 0x%x.", rc);

    if (R_SUCCEEDED(rc)) setsysGetColorSetId(&theme);

    if (R_SUCCEEDED(rc)) {
        rc = plInitialize();
        if (R_FAILED(rc)) snprintf(errormsg, sizeof(errormsg)-1, "Error: plInitialize() failed: 0x%x.", rc);
    }

    if (R_SUCCEEDED(rc)) {
        rc = textInit();
        if (R_FAILED(rc)) {
            snprintf(errormsg, sizeof(errormsg)-1, "Error: textInit() failed: 0x%x.", rc);
        }
    }

    if (R_SUCCEEDED(rc)) menuStartupPath();

    if (R_SUCCEEDED(rc)) {
        rc = assetsInit();
        if (R_FAILED(rc)) {
            snprintf(errormsg, sizeof(errormsg)-1, "Error: assetsInit() failed: 0x%x.", rc);
        }
    }

    if (R_SUCCEEDED(rc)) themeStartup((ThemePreset)theme);

    if (R_SUCCEEDED(rc)) powerInit();

    if (R_SUCCEEDED(rc)) {
        rc = netloaderInit();
        if (R_FAILED(rc)) {
            snprintf(errormsg, sizeof(errormsg)-1, "Error: netloaderInit() failed: 0x%x.", rc);
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
            snprintf(msg, sizeof(msg)-1, "%s\n0x%x", textGetString(StrId_LastLoadResult), lastret);

            menuCreateMsgBox(780, 300, msg);
        }
    }

    if (errormsg[0]) error_screen = 1;

    if (!error_screen) {
        framebufferCreate(&g_framebufObj, nwindowGetDefault(), FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
        framebufferMakeLinear(&g_framebufObj);
    }
    else {
        consoleInit(NULL);
        printf("%s\n", errormsg);
        printf("Press the + button to exit.\n");
    }

    intializeKeyHeldInterval(KEY_HELD_INITIAL_WAIT_TIME, KEY_HELD_SUBSEQUENT_WAIT_TIME);

    while (appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        if (!error_screen) {
            if (!uiUpdate()) break;
            g_framebuf = framebufferBegin(&g_framebufObj, &g_framebuf_width);
            #ifdef PERF_LOG
            start_tick = svcGetSystemTick();
            #endif
            memset(g_framebuf, 237, g_framebuf_width * FB_HEIGHT);
            menuLoop();
        }
        else {
            if (menuUpdateErrorScreen()) break;
        }

        if (!error_screen) {
            framebufferEnd(&g_framebufObj);

            #ifdef PERF_LOG
            g_tickdiff_frame = svcGetSystemTick() - start_tick;
            #endif
        }
        else {
            consoleUpdate(NULL);
        }
    }

    if (!error_screen) {
        framebufferClose(&g_framebufObj);
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
    plExit();
    setsysExit();

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

/**
    Initializes the initial and susequent wait interval with which the keys is considered to be held for long

    @param msInitialInterval : The interval after which the key is considered as held for long
    @param msSubsequentInterval : Once the key is found to be held for long, this decides when the key held state
                                  should be set for susequent calls to keyHeldTimeElapsed()
    @return nothing
*/
void intializeKeyHeldInterval(u16 msInitialInterval, u16 msSubsequentInterval)
{
    g_tick_keyheld_interval = armNsToTicks(msInitialInterval * 1000000);
    g_tick_keyheld_subsequent_interval = armNsToTicks(msSubsequentInterval * 1000000);
}

/**
    Initializes the initial and susequent wait interval after which continous key press will be set

    @return true if the key is held for long, false if it is not held
    bar and "," as the half bar.
*/
bool keyHeldTimeElapsed(void)
{
    bool elapsed = false;
    u64 tick_current = svcGetSystemTick();
    if (g_tick_prev_keyheld == 0 || g_curr_keyheld == 0)
    {
        g_tick_prev_keyheld = tick_current;
    }
    else if (g_prev_keyheld != g_curr_keyheld)
    {
        g_tick_prev_keyheld = tick_current;
        g_prev_keyheld = g_curr_keyheld;
    }
    else
    {
        u64 tickdiff_keyheld = tick_current - g_tick_prev_keyheld;
        if (g_prev_elapsed && (tickdiff_keyheld > g_tick_keyheld_subsequent_interval))
        {
            elapsed = true;
        }
        else if (tickdiff_keyheld > g_tick_keyheld_interval)
        {
            g_tick_prev_keyheld = tick_current;
            elapsed = true;
        }
    }
    g_prev_elapsed = elapsed;
    return elapsed;
}

//This is implemented here due to the hid code.
bool menuUpdate(void) {
    bool exitflag = 0;
    menu_s* menu = menuGetCurrent();
    u64 down = menuGetKeysDown();

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
        if (down & KEY_DOWN) move-=7;
        if (down & KEY_UP) move+=7;

        u64 held = hidKeysHeld(CONTROLLER_P1_AUTO);
        g_curr_keyheld = held;

        if (keyHeldTimeElapsed())
        {
            if (held & KEY_LEFT) move--;
            if (held & KEY_RIGHT) move++;
            if (held & KEY_DOWN) move -= 7;
            if (held & KEY_UP) move += 7;
        }

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
