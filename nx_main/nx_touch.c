#include "nx_touch.h"

#define TAP_MOVEMENT_GAP 20
#define LISTING_START_Y 475
#define LISTING_END_Y 647

struct touchInfo_s touchInfo;

void touchInit() {
    touchInfo.firstTouch = NULL;
    touchInfo.prevTouch = NULL;
    touchInfo.isTap = true;
    touchInfo.initMenuXPos = 0;
    touchInfo.initMenuIndex = 0;
}

void handleTap(menu_s* menu, int px) {
    int i = 0;
    menuEntry_s *me = NULL;

    for (me = menu->firstEntry, i = 0; me; me = me->next, i ++) {
        int entry_start_x = 29 + i * (140 + 30);

        int screen_width = 1280;
        if (entry_start_x >= (screen_width - menu->xPos))
            break;

        if (px >= (entry_start_x + menu->xPos) && px <= (entry_start_x + menu->xPos) + 140 ) {
            launchMenuEntryTask(me);
            break;
        }
    }
}

void handleTouch(menu_s* menu) {
    touchPosition currentTouch;
    u32 touches = hidTouchCount();

    // On touch start.
    if (touches == 1 && touchInfo.firstTouch == NULL) {
        hidTouchRead(&currentTouch, 0);

        touchInfo.firstTouch = malloc(sizeof(touchPosition));
        touchInfo.prevTouch = malloc(sizeof(touchPosition));
        touchInfo.firstTouch->px = currentTouch.px;
        touchInfo.firstTouch->py = currentTouch.py;
        touchInfo.isTap = true;
        touchInfo.initMenuXPos = menu->xPos;
        touchInfo.initMenuIndex = menu->curEntry;
    }
    // On touch moving.
    else if (touches >= 1 && touchInfo.firstTouch != NULL) {
        hidTouchRead(&currentTouch, 0);

        touchInfo.prevTouch->px = currentTouch.px;
        touchInfo.prevTouch->py = currentTouch.py;

        if (touchInfo.isTap && (abs(touchInfo.firstTouch->px - currentTouch.px) > TAP_MOVEMENT_GAP || abs(touchInfo.firstTouch->py - currentTouch.py) > TAP_MOVEMENT_GAP)) {
            touchInfo.isTap = false;
        }
        if (!menuIsMsgBoxOpen() && touchInfo.firstTouch->py > LISTING_START_Y && touchInfo.firstTouch->py < LISTING_END_Y && !touchInfo.isTap && menu->nEntries > 7) {
            menu->xPos = touchInfo.initMenuXPos + (currentTouch.px - touchInfo.firstTouch->px);
            menu->curEntry = touchInfo.initMenuIndex + ((int) (touchInfo.firstTouch->px - currentTouch.px) / 170);

            if (menu->curEntry < 0)
                menu->curEntry = 0;

            if (menu->curEntry >= menu->nEntries - 6)
                menu->curEntry = menu->nEntries - 7;
        }
    }
    // On touch end.
    else if (touchInfo.firstTouch != NULL) {
        if (menuIsMsgBoxOpen()) {
            MessageBox currMsgBox = menuGetCurrentMsgBox();
            int start_x = 1280 / 2 - currMsgBox.width / 2;
            int start_y = (720 / 2 - currMsgBox.height / 2) + (currMsgBox.height - 80);
            int end_x = start_x + currMsgBox.width;
            int end_y = start_y + 80;

            if (touchInfo.firstTouch->px > start_x && touchInfo.firstTouch->px < end_x && touchInfo.firstTouch->py > start_y && touchInfo.firstTouch->py < end_y && touchInfo.isTap) {
                menuCloseMsgBox();
            }
        } else {
            if (touchInfo.firstTouch->py > LISTING_START_Y && touchInfo.firstTouch->py < LISTING_END_Y && touchInfo.isTap) {
                handleTap(menu, touchInfo.prevTouch->px);
            }
        }

        free(touchInfo.firstTouch);
        touchInfo.firstTouch = NULL;

        free(touchInfo.prevTouch);
        touchInfo.prevTouch = NULL;
    }
}