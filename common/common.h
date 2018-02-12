#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <malloc.h>
#ifdef SWITCH
#include <switch.h>
#endif

#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

#define M_TAU (2*M_PI)

typedef union {
    uint32_t abgr;
    struct {
        uint8_t r,g,b,a;
    };
} color_t;

#include "font.h"
#include "nacp.h"
#include "menu.h"
#include "text.h"
#include "ui.h"
#include "launch.h"
#include "nro.h"
#include "nanojpeg.h"

void menuStartup();
void menuLoop();

static inline uint8_t BlendColor(uint32_t src, uint32_t dst, uint8_t alpha)
{
    uint8_t one_minus_alpha = (uint8_t)255 - alpha;
    return (dst*alpha + src*one_minus_alpha)/(uint8_t)255;
}

static inline color_t MakeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    color_t clr;
    clr.r = r;
    clr.g = g;
    clr.b = b;
    clr.a = a;
    return clr;
}

#ifdef SWITCH
extern uint8_t* g_framebuf;
extern u32 g_framebuf_width;
static inline void DrawPixel(uint32_t x, uint32_t y, color_t clr)
{
    if (x >= 1280 || y >= 720)
        return;
    u32 off = (y * g_framebuf_width + x)*4;
    g_framebuf[off] = BlendColor(g_framebuf[off], clr.r, clr.a); off++;
    g_framebuf[off] = BlendColor(g_framebuf[off], clr.g, clr.a); off++;
    g_framebuf[off] = BlendColor(g_framebuf[off], clr.b, clr.a); off++;
    g_framebuf[off] = 0xff;
}
static inline void DrawPixelRaw(uint32_t x, uint32_t y, color_t clr)
{
    if (x >= 1280 || y >= 720)
        return;
    u32 off = (y * g_framebuf_width + x)*4;
    g_framebuf[off] = clr.r; off++;
    g_framebuf[off] = clr.g; off++;
    g_framebuf[off] = clr.b; off++;
    g_framebuf[off] = 0xff;
}
#else
extern color_t pixels[720][1280];
static inline void DrawPixel(uint32_t x, uint32_t y, color_t clr)
{
    if (x >= 1280 || y >= 720)
        return;
    pixels[y][x].r = BlendColor(pixels[y][x].r, clr.r, clr.a);
    pixels[y][x].g = BlendColor(pixels[y][x].g, clr.g, clr.a);
    pixels[y][x].b = BlendColor(pixels[y][x].b, clr.b, clr.a);
    pixels[y][x].a = 0xff;
}
static inline void DrawPixelRaw(uint32_t x, uint32_t y, color_t clr)
{
    if (x >= 1280 || y >= 720)
        return;
    pixels[y][x].r = clr.r;
    pixels[y][x].g = clr.g;
    pixels[y][x].b = clr.b;
    pixels[y][x].a = 0xff;
}
#endif

void DrawPixel(uint32_t x, uint32_t y, color_t clr);
void DrawText(const ffnt_header_t* font, uint32_t x, uint32_t y, color_t clr, const char* text);
void DrawTextTruncate(const ffnt_header_t* font, uint32_t x, uint32_t y, color_t clr, const char* text, uint32_t max_width, const char* end_text);
