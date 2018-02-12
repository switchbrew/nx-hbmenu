typedef struct {
    uint8_t magic[4]; // 'fFNT'
    int version;  // 1
    uint16_t npages;
    uint8_t height;
    uint8_t baseline;
} ffnt_header_t;

typedef struct {
    uint32_t size, offset;
} ffnt_pageentry_t;

typedef struct {
    uint32_t pos[0x100];
    uint8_t widths[0x100];
    uint8_t heights[0x100];
    int8_t advances[0x100];
    int8_t posX[0x100];
    int8_t posY[0x100];
} ffnt_pagehdr_t;

typedef struct {
    ffnt_pagehdr_t hdr;
    uint8_t data[];
} ffnt_page_t;

typedef struct {
    uint8_t width, height;
    int8_t posX, posY, advance;
    const uint8_t* data;
} glyph_t;

extern const ffnt_header_t tahoma24_nxfnt;
extern const ffnt_header_t tahoma12_nxfnt;
#define tahoma24 &tahoma24_nxfnt
#define tahoma12 &tahoma12_nxfnt
