#include "common.h"

static inline const ffnt_page_t* FontGetPage(const ffnt_header_t* font, uint32_t page_id)
{
    //__builtin_printf("GetPage %u\n", (unsigned int)page_id);
    if (page_id >= font->npages)
        return NULL;
    ffnt_pageentry_t* ent = &((ffnt_pageentry_t*)(font+1))[page_id];
    if (ent->size == 0)
        return NULL;
    return (const ffnt_page_t*)((const uint8_t*)font + ent->offset);
}

static inline bool FontLoadGlyph(glyph_t* glyph, const ffnt_header_t* font, uint32_t codepoint)
{
    //__builtin_printf("LoadGlyph %u\n", (unsigned int)codepoint);
    const ffnt_page_t* page = FontGetPage(font, codepoint >> 8);
    if (!page)
        return false;

    codepoint &= 0xFF;
    uint32_t off = page->hdr.pos[codepoint];
    if (off == ~(uint32_t)0)
        return false;

    //__builtin_printf("%c %u\n", (char)codepoint, (unsigned int)off);
    glyph->width   = page->hdr.widths[codepoint];
    glyph->height  = page->hdr.heights[codepoint];
    glyph->advance = page->hdr.advances[codepoint];
    glyph->posX    = page->hdr.posX[codepoint];
    glyph->posY    = page->hdr.posY[codepoint];
    glyph->data    = &page->data[off];
    return true;
}

static void DrawGlyph(uint32_t x, uint32_t y, color_t clr, const glyph_t* glyph)
{
    uint32_t i, j;
    const uint8_t* data = glyph->data;
    x += glyph->posX;
    y += glyph->posY;
    //__builtin_printf("DrawGlyph %u %u %08X\n", (unsigned int)x, (unsigned int)y, (unsigned int)clr.abgr);
    for (j = 0; j < glyph->height; j ++)
    {
        for (i = 0; i < glyph->width; i ++)
        {
            clr.a = *data++;
            if (!clr.a) continue;
            DrawPixel(x+i, y+j, clr);
        }
    }
}

static inline uint8_t DecodeByte(const char** ptr)
{
    uint8_t c = (uint8_t)**ptr;
    *ptr += 1;
    return c;
}

// UTF-8 code adapted from http://www.json.org/JSON_checker/utf8_decode.c

static inline int8_t DecodeUTF8Cont(const char** ptr)
{
    int c = DecodeByte(ptr);
    return ((c & 0xC0) == 0x80) ? (c & 0x3F) : -1;
}

static inline uint32_t DecodeUTF8(const char** ptr)
{
    uint32_t r;
    uint8_t c;
    int8_t c1, c2, c3;

    c = DecodeByte(ptr);
    if ((c & 0x80) == 0)
        return c;
    if ((c & 0xE0) == 0xC0)
    {
        c1 = DecodeUTF8Cont(ptr);
        if (c1 >= 0)
        {
            r = ((c & 0x1F) << 6) | c1;
            if (r >= 0x80)
                return r;
        }
    } else if ((c & 0xF0) == 0xE0)
    {
        c1 = DecodeUTF8Cont(ptr);
        if (c1 >= 0)
        {
            c2 = DecodeUTF8Cont(ptr);
            if (c2 >= 0)
            {
                r = ((c & 0x0F) << 12) | (c1 << 6) | c2;
                if (r >= 0x800 && (r < 0xD800 || r >= 0xE000))
                    return r;
            }
        }
    } else if ((c & 0xF8) == 0xF0)
    {
        c1 = DecodeUTF8Cont(ptr);
        if (c1 >= 0)
        {
            c2 = DecodeUTF8Cont(ptr);
            if (c2 >= 0)
            {
                c3 = DecodeUTF8Cont(ptr);
                if (c3 >= 0)
                {
                    r = ((c & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
                    if (r >= 0x10000 && r < 0x110000)
                        return r;
                }
            }
        }
    }
    return 0xFFFD;
}

static void DrawText_(const ffnt_header_t* font, uint32_t x, uint32_t y, color_t clr, const char* text, uint32_t max_width, const char* end_text)
{
    //__builtin_printf("DrawText %u %u %08X %s\n", (unsigned int)x, (unsigned int)y, (unsigned int)clr.abgr, text);
    y += font->baseline;
    uint32_t origX = x;
    while (*text)
    {
        if (max_width && x-origX >= max_width) {
            text = end_text;
            max_width = 0;
        }

        glyph_t glyph;
        uint32_t codepoint = DecodeUTF8(&text);

        if (codepoint == '\n')
        {
            if (max_width) {
                text = end_text;
                max_width = 0;
                continue;
            }

            x = origX;
            y += font->height;
            continue;
        }

        if (!FontLoadGlyph(&glyph, font, codepoint))
        {
            if (!FontLoadGlyph(&glyph, font, '?'))
                continue;
        }

        DrawGlyph(x, y, clr, &glyph);
        x += glyph.advance;
    }
}

void DrawText(const ffnt_header_t* font, uint32_t x, uint32_t y, color_t clr, const char* text)
{
    DrawText_(font, x, y, clr, text, 0, NULL);
}

void DrawTextTruncate(const ffnt_header_t* font, uint32_t x, uint32_t y, color_t clr, const char* text, uint32_t max_width, const char* end_text)
{
    DrawText_(font, x, y, clr, text, max_width, end_text);
}
