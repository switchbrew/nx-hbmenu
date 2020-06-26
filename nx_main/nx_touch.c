#include "nx_touch.h"

#define TAP_MOVEMENT_GAP 20
#define VERTICAL_SWIPE_HORIZONTAL_PLAY 250
#define VERTICAL_SWIPE_MINIMUM_DISTANCE 300
#define HORIZONTAL_SWIPE_VERTICAL_PLAY 250
#define HORIZONTAL_SWIPE_MINIMUM_DISTANCE 300

#define distance(x1, y1, x2, y2) (int) sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)))

struct touchInfo_s touchInfo;

void touchInit() {
    touchInfo.gestureInProgress = false;
    touchInfo.isTap = true;
    touchInfo.initMenuXPos = 0;
    touchInfo.initMenuIndex = 0;
    touchInfo.lastSlideSpeed = 0;
}

void handleTappingOnApp(menu_s* menu, int px) {
    int i = 0;
    menuEntry_s *me = NULL;
    ThemeLayoutObject *layoutobj = &themeCurrent.layoutObjects[ThemeLayoutId_MenuList];

    for (me = menu->firstEntry, i = 0; me; me = me->next, i ++) {
        int entry_start_x = layoutobj->posStart[0] + i * layoutobj->posEnd[0];

        int screen_width = 1280;
        if (entry_start_x >= (screen_width - menu->xPos))
            break;

        if (px >= (entry_start_x + menu->xPos) && px <= (entry_start_x + menu->xPos) + layoutobj->size[0]) {
            launchMenuEntryTask(me);
            break;
        }
    }
}

void handleTappingOnOpenLaunch(menu_s* menu) {
    if (menu->nEntries > 0)
    {
        int i;
        menuEntry_s* me;
        for (i = 0, me = menu->firstEntry; i != menu->curEntry; i ++, me = me->next);
        launchMenuEntryTask(me);
    }
}

static inline bool checkInsideTextLayoutObject(ThemeLayoutId id, int x, int y) {
    ThemeLayoutObject *layoutobj = &themeCurrent.layoutObjects[id];
    if (!layoutobj->visible) return false;

    return x > layoutobj->posFinal[0] && x < layoutobj->posFinal[0]+(layoutobj->touchSize[0] ? layoutobj->touchSize[0] : layoutobj->textSize[0]) && y > layoutobj->posFinal[1]-layoutobj->touchSize[1] && y < layoutobj->posFinal[1];
}

void handleTouch(menu_s* menu) {
    ThemeLayoutObject *layoutobj = NULL;
    touchPosition currentTouch;
    u32 touches = hidTouchCount();

    layoutobj = &themeCurrent.layoutObjects[ThemeLayoutId_MenuListTiles];
    int entries_count = layoutobj->posEnd[0];
    layoutobj = &themeCurrent.layoutObjects[ThemeLayoutId_MenuList];

    // On touch start.
    if (touches == 1 && !touchInfo.gestureInProgress) {
        hidTouchRead(&currentTouch, 0);

        touchInfo.gestureInProgress = true;
        touchInfo.firstTouch = currentTouch;
        touchInfo.prevTouch = currentTouch;
        touchInfo.isTap = true;
        touchInfo.initMenuXPos = menu->xPos;
        touchInfo.initMenuIndex = menu->curEntry;
        touchInfo.lastSlideSpeed = 0;
        menu->slideSpeed = 0;
    }
    // On touch moving.
    else if (touches >= 1 && touchInfo.gestureInProgress) {
        hidTouchRead(&currentTouch, 0);

        touchInfo.lastSlideSpeed = ((int)(currentTouch.px - touchInfo.prevTouch.px));

        touchInfo.prevTouch = currentTouch;

        if (touchInfo.isTap && (abs(touchInfo.firstTouch.px - currentTouch.px) > TAP_MOVEMENT_GAP || abs(touchInfo.firstTouch.py - currentTouch.py) > TAP_MOVEMENT_GAP)) {
            touchInfo.isTap = false;
        }
        if (!menuIsMsgBoxOpen() && touchInfo.firstTouch.py > layoutobj->posStart[1] && touchInfo.firstTouch.py < layoutobj->posStart[1]+layoutobj->size[1] && !touchInfo.isTap && menu->nEntries > entries_count) {

            if (!touchInfo.isTap) {
                menu->slideSpeed = touchInfo.lastSlideSpeed;
            }
        }
    }
    // On touch end.
    else if (touchInfo.gestureInProgress) {
        int x1 = touchInfo.firstTouch.px;
        int y1 = touchInfo.firstTouch.py;
        int x2 = touchInfo.prevTouch.px;
        int y2 = touchInfo.prevTouch.py;

        if (!touchInfo.isTap) {
            menu->slideSpeed = touchInfo.lastSlideSpeed;
        }

        bool netloader_active = menuIsNetloaderActive();

        if (menuIsMsgBoxOpen() && !netloader_active) {
            layoutobj = &themeCurrent.layoutObjects[ThemeLayoutId_MsgBoxSeparator];
            MessageBox currMsgBox = menuGetCurrentMsgBox();
            int start_x = 1280 / 2 - currMsgBox.width / 2;
            int start_y = (720 / 2 - currMsgBox.height / 2) + currMsgBox.height;
            int end_x = start_x + currMsgBox.width;
            int end_y = start_y;
            start_y+= layoutobj->posStart[1];

            if (x1 > start_x && x1 < end_x && y1 > start_y && y1 < end_y && touchInfo.isTap) {
                menuCloseMsgBox();
            }
        } else if (touchInfo.isTap && !netloader_active) {
            // App Icons
            if (y1 > layoutobj->posStart[1] && y1 < layoutobj->posStart[1]+layoutobj->size[1]) {
                handleTappingOnApp(menu, touchInfo.prevTouch.px);
            }
            // Bottom Buttons
            else {
                // Back Button
                if (checkInsideTextLayoutObject(ThemeLayoutId_ButtonB, x1, y1) || checkInsideTextLayoutObject(ThemeLayoutId_ButtonBText, x1, y1)) {
                    launchMenuBackTask();
                }
                // Open/Launch Button
                else if (menu->nEntries != 0 && (checkInsideTextLayoutObject(ThemeLayoutId_ButtonA, x1, y1) || checkInsideTextLayoutObject(ThemeLayoutId_ButtonAText, x1, y1))) {
                    handleTappingOnOpenLaunch(menu);
                }
                // Star Button
                else if (menu->nEntries != 0) {
                    int i;
                    menuEntry_s* me;
                    for (i = 0, me = menu->firstEntry; i != menu->curEntry; i ++, me = me->next);
                    if (me->type != ENTRY_TYPE_THEME && (checkInsideTextLayoutObject(ThemeLayoutId_ButtonX, x1, y1) || checkInsideTextLayoutObject(ThemeLayoutId_ButtonXText, x1, y1))) {
                        menuHandleXButton();
                    }
                }
            }
        }
        // Vertical Swipe
        else if (abs(x1 - x2) < VERTICAL_SWIPE_HORIZONTAL_PLAY && distance(x1, y1, x2, y2) > VERTICAL_SWIPE_MINIMUM_DISTANCE) {
            // Swipe up to go back
            if (y1 - y2 > 0) {
                launchMenuBackTask();
            }
            // Swipe down to go into netloader
            else if (y1 - y2 < 0) {
                launchMenuNetloaderTask();
            }
        }
        // Horizontal Swipe
        else if (y1 < layoutobj->posStart[1] && y2 < layoutobj->posStart[1]) {
            if (abs(y1 - y2) < HORIZONTAL_SWIPE_VERTICAL_PLAY && distance(x1, y1, x2, y2) > HORIZONTAL_SWIPE_MINIMUM_DISTANCE) {
                // Swipe left to go into theme-menu
                if (x1 - x2 > 0) {
                    themeMenuStartup();
                }
            }
        }

        touchInfo.gestureInProgress = false;
    }
}
