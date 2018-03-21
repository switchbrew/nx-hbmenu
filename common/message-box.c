#include "common.h"

typedef struct
{
    uint32_t width;
    uint32_t height;
    color_t *bg;
    char *text;
} MessageBox;

MessageBox currMsgBox;

void drawMsgBoxBgToBuff(color_t *buff, int width, int height) {
    int x, y;
    int off;
    int circle_center_x, circle_center_y;
    int corner_size = 0;
    float rad, alpha;
    color_t base_color = MakeColor(255, 255, 255, 255);
    color_t color;

    for (y=0; y<height; y++) {
        for (x=0; x<width; x++) {
            if (corner_size > 0) {
                if (x<corner_size && y<corner_size) { // top left corner
                    circle_center_x = corner_size-1;
                    circle_center_y = corner_size-1;
                }
                else if (x>width-corner_size && y<corner_size) { // top right corner
                    circle_center_x = width-corner_size;
                    circle_center_y = corner_size-1;
                }
                else if (x<corner_size && y>height-corner_size) { // bottom left corner
                    circle_center_x = corner_size-1;
                    circle_center_y = height-corner_size;
                }
                else if (x>width-corner_size && y>height-corner_size) { // bottom right corner
                    circle_center_x = width-corner_size;
                    circle_center_y = height-corner_size;
                }
                else {
                    circle_center_x = -1;
                    circle_center_y = -1;
                }

                if (circle_center_x == -1 && circle_center_y == -1) {
                    color = base_color;
                }
                else {
                    rad = sqrt(pow(circle_center_x - x, 2) + pow(circle_center_y - y, 2));
                    alpha = (float)corner_size - rad;

                    if (rad < corner_size) {
                        if (alpha < 1.0) {
                            color = MakeColor(base_color.r, base_color.g, base_color.b, base_color.a * alpha);
                        }
                        else
                            color = base_color;
                    }
                    else
                        color = MakeColor(0, 0, 0, 0);
                }
            }
            else
                color = base_color;

            if (y == height - 80) {
                color = themeCurrent.seperatorColor;
            }

            off = (y * width + x);
            *((uint32_t *)&buff[off]) = color.r | (color.g<<8) | (color.b<<16) | (color.a<<24);
        }
    }
}

void menuDrawMsgBox() {
    if (!menuIsMsgBoxOpen())
        return;

    int off;
    int x, y;
    int start_x = 1280 / 2 - currMsgBox.width / 2;
    int start_y = 720 / 2 - currMsgBox.height / 2;
    int end_x = start_x + currMsgBox.width;
    uint32_t text_width, text_height;
    color_t curr_color;

    color_t border_color;
    int sep_start_y = currMsgBox.height - 80;
    int border_thickness = 6; 

    int shadow_start_y, shadow_y;
    int shadow_inset;
    int shadow_size = 4;
    float highlight_multiplier = highlight_multiplier = fmax(0.0, fabs(fmod(menuTimer, 1.0) - 0.5) / 0.5);
    color_t shadow_color;
    uint8_t shadow_alpha_base = 80;

    border_color = MakeColor(themeCurrent.highlightColor.r + (255 - themeCurrent.highlightColor.r) * highlight_multiplier, themeCurrent.highlightColor.g + (255 - themeCurrent.highlightColor.g) * highlight_multiplier, themeCurrent.highlightColor.b + (255 - themeCurrent.highlightColor.b) * highlight_multiplier, 255);

    // Darken the background
    for (y=0; y<720; y++) {
        for (x=0; x<1280; x++) {
            DrawPixel(x, y, MakeColor(0, 0, 0, 100));
        }
    }

    // Draw the message box background
    for (y=0; y<currMsgBox.height; y++) {
        for (x=0; x<currMsgBox.width; x++) {
            off = (y * currMsgBox.width + x);
            curr_color = currMsgBox.bg[off];

            if (((x<border_thickness || x>=currMsgBox.width-border_thickness) && y>sep_start_y) || 
                (y>sep_start_y && y<=sep_start_y+border_thickness) || (y>=currMsgBox.height-border_thickness)) {
                curr_color = border_color;
            }

            DrawPixel(start_x+x, start_y+y, curr_color);
        }
    }

    GetTextDimensions(interuimedium20, currMsgBox.text, &text_width, &text_height);
    
    if (text_width < currMsgBox.width && text_height < sep_start_y) {
        DrawText(interuiregular18, start_x + (currMsgBox.width - text_width) / 2, start_y + (currMsgBox.height - text_height - 80) / 2, MakeColor(0, 0, 0, 255), currMsgBox.text);
    }

    DrawText(interuimedium20, start_x + 365, start_y + 245, MakeColor(0, 0, 0, 255), textGetString(StrId_MsgBox_OK));

    shadow_start_y = start_y + currMsgBox.height;

    for (shadow_y=shadow_start_y; shadow_y <shadow_start_y+shadow_size; shadow_y++) {
        for (x=start_x; x<end_x; x++) {
            shadow_color = MakeColor(0, 0, 0, shadow_alpha_base * (1.0 - (float)(shadow_y - shadow_start_y) / ((float)shadow_size)));
            shadow_inset =(shadow_y-shadow_start_y);

            if (x >= start_x + shadow_inset && x <= end_x - shadow_inset) {
                DrawPixel(x, shadow_y, shadow_color);
            }
        }
    }
}

void menuCreateMsgBox(int width, int height, const char *text) {
    if (menuIsMsgBoxOpen())
        return;

    char *new_text = strdup(text);
    if (new_text==NULL)
        return;

    currMsgBox = (MessageBox) { width, height, NULL, new_text };

    currMsgBox.bg = malloc(currMsgBox.width*currMsgBox.height*4);

    if (currMsgBox.bg) {
        drawMsgBoxBgToBuff(currMsgBox.bg, currMsgBox.width, currMsgBox.height);
    }
}

bool menuIsMsgBoxOpen() {
    return currMsgBox.width != 0 || currMsgBox.height != 0 || currMsgBox.bg || currMsgBox.text;
}

void menuCloseMsgBox() {
    if (currMsgBox.bg) {
        free(currMsgBox.bg);
        currMsgBox.bg = NULL;
    }

    currMsgBox.width = currMsgBox.height = 0;

    if (currMsgBox.text) {
        free(currMsgBox.text);
        currMsgBox.text = NULL;
    }
}
