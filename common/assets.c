#include "common.h"

#include <minizip/unzip.h>
#include <png.h>

#define GENASSET(_p, _mode, _w, _h) {{.path = _p, .imageMode = _mode, .imageSize = {_w, _h}}, {}}

static bool g_assetsInitialized = 0;
assetsDataEntry g_assetsDataList[AssetId_Max][2] = {
    GENASSET("battery_icon.bin", IMAGE_MODE_RGBA32, 24, 24),
    GENASSET("charging_icon.bin", IMAGE_MODE_RGBA32, 24, 24),
    GENASSET("folder_icon.bin", IMAGE_MODE_RGB24, 256, 256),
    GENASSET("invalid_icon.bin", IMAGE_MODE_RGB24, 256, 256),
    GENASSET("hbmenu_logo_dark.bin", IMAGE_MODE_RGBA32, 140, 60),
    GENASSET("hbmenu_logo_light.bin", IMAGE_MODE_RGBA32, 140, 60),
    GENASSET("theme_icon_dark.bin", IMAGE_MODE_RGB24, 256, 256),
    GENASSET("theme_icon_light.bin", IMAGE_MODE_RGB24, 256, 256),
    GENASSET("airplane_icon.bin", IMAGE_MODE_RGBA32, 24, 24),
    GENASSET("wifi_none_icon.bin", IMAGE_MODE_RGBA32, 24, 24),
    GENASSET("wifi1_icon.bin", IMAGE_MODE_RGBA32, 24, 24),
    GENASSET("wifi2_icon.bin", IMAGE_MODE_RGBA32, 24, 24),
    GENASSET("wifi3_icon.bin", IMAGE_MODE_RGBA32, 24, 24),
    GENASSET("eth_icon.bin", IMAGE_MODE_RGBA32, 24, 24),
    GENASSET("eth_none_icon.bin", IMAGE_MODE_RGBA32, 24, 24),
    GENASSET("", IMAGE_MODE_RGB24, 1280, 720),
};

static void assetsClearEntry(assetsDataEntry *entry) {
    free(entry->buffer);

    entry->size = 0;
    entry->buffer = NULL;

    memset(entry, 0, sizeof(*entry));
}

static void assetsSetPixelSize(assetsDataEntry *entry) {
    switch (entry->imageMode) {
        case IMAGE_MODE_RGB24:
            entry->pixSize = 3;
            break;

        case IMAGE_MODE_RGBA32:
            entry->pixSize = 4;
            break;
    }
}

static int assetsLoadFile(unzFile zipf, assetsDataEntry *entry) {
    int ret;
    int filesize=0;
    unz_file_info file_info;
    u8* buffer = NULL;

    assetsSetPixelSize(entry);

    ret = unzLocateFile(zipf, entry->path, 0);

    if (ret==UNZ_OK) ret = unzOpenCurrentFile(zipf);

    if (ret==UNZ_OK) {
        ret = unzGetCurrentFileInfo(zipf, &file_info, NULL, 0, NULL, 0, NULL, 0);

        filesize = file_info.uncompressed_size;
        if (filesize != entry->imageSize[0] * entry->imageSize[1] * entry->pixSize) ret = -10;

        if (ret==UNZ_OK) {
            buffer = (u8*)malloc(filesize);
            if (buffer) {
                memset(buffer, 0, filesize);
            } else {
                ret = -11;
            }
        }

        if (ret==UNZ_OK) {
            ret = unzReadCurrentFile(zipf, buffer, filesize);
            if(ret < filesize) {
                ret = -12;
            } else {
                ret = UNZ_OK;
            }
        }

        if (ret!=UNZ_OK && buffer!=NULL) free(buffer);

        unzCloseCurrentFile(zipf);
    }

    if (ret==UNZ_OK) {
        entry->buffer = buffer;
        entry->size = filesize;
    }

    return ret;
}

Result assetsInit(void) {
    int ret=0;
    int i, stopi;
    unzFile zipf;
    assetsDataEntry *entry = NULL;
    char tmp_path[PATH_MAX];

    if (g_assetsInitialized) return 0;

    #ifdef __SWITCH__
    Result rc = romfsInit();
    if (R_FAILED(rc)) return rc;
    #endif

    memset(tmp_path, 0, sizeof(tmp_path));

    #ifdef __SWITCH__
    strncpy(tmp_path, "romfs:/assets.zip", sizeof(tmp_path)-1);
    #else
    snprintf(tmp_path, sizeof(tmp_path)-1, "%s/romfs/assets.zip", menuGetRootBasePath());
    #endif

    zipf = unzOpen(tmp_path);
    if(zipf==NULL) {
        #ifdef __SWITCH__
        romfsExit();
        #endif

        return 0x80;
    }

    for (i=0; i<AssetId_Max; i++) {
        stopi = i;
        entry = &g_assetsDataList[i][0];
        if (entry->path[0]) {
            ret = assetsLoadFile(zipf, entry);
            if (ret!=UNZ_OK) break;
            entry->initialized = true;
        }
    }

    if (ret!=UNZ_OK) {
        for (i=0; i<stopi; i++) {
            assetsClearEntry(&g_assetsDataList[i][0]);
        }
    }

    if (ret==UNZ_OK) g_assetsInitialized = 1;

    unzClose(zipf);

    #ifdef __SWITCH__
    romfsExit();
    #endif

    return ret;
}

void assetsExit(void) {
    if (!g_assetsInitialized) return;
    g_assetsInitialized = 0;

    for (int i=0; i<AssetId_Max; i++) {
        assetsClearEntry(&g_assetsDataList[i][0]);
    }

    assetsClearTheme();
}

void assetsClearTheme(void) {
    for (int i=0; i<AssetId_Max; i++) {
        assetsClearEntry(&g_assetsDataList[i][1]);
    }
}

bool assetsLoadJpgFromMemory(u8 *indata, size_t indata_size, u8 *outdata, ImageMode imageMode, size_t width, size_t height) {
    int w,h,samp;

    tjhandle _jpegDecompressor = tjInitDecompress();

    if (_jpegDecompressor == NULL) {
        return false;
    }

    if (tjDecompressHeader2(_jpegDecompressor, indata, indata_size, &w, &h, &samp) == -1) {
        tjDestroy(_jpegDecompressor);
        return false;
    }

    if (w != width || h != height ) {
        tjDestroy(_jpegDecompressor);
        return false;
    }

    if (tjDecompress2(_jpegDecompressor, indata, indata_size, outdata, w, 0, h, imageMode == IMAGE_MODE_RGB24 ? TJPF_RGB : TJPF_RGBA, TJFLAG_ACCURATEDCT) == -1) {
        tjDestroy(_jpegDecompressor);
        return false;
    }

    tjDestroy(_jpegDecompressor);

    return true;
}

bool assetsLoadPngFromMemory(u8 *indata, size_t indata_size, u8 *outdata, ImageMode imageMode, size_t width, size_t height) {
    png_image image;
    bool ret=true;

    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_memory(&image, indata, indata_size) != 0) {
        if (image.width != width || image.height != height)
            ret = false;

        if (ret) image.format = imageMode == IMAGE_MODE_RGB24 ? PNG_FORMAT_RGB : PNG_FORMAT_RGBA;

        if (ret && png_image_finish_read(&image, NULL, outdata, 0, NULL) == 0)
            ret = false;
    }
    else
        ret = false;

    png_image_free(&image);

    return ret;
}

bool assetsLoadFromTheme(AssetId id, const char *path, int *imageSize) {
    if (id < 0 || id >= AssetId_Max) return false;

    assetsDataEntry *entry = &g_assetsDataList[id][1];
    if (entry->initialized) return false;

    memset(entry, 0, sizeof(*entry));

    entry->imageSize[0] = imageSize[0];
    entry->imageSize[1] = imageSize[1];

    entry->imageMode = g_assetsDataList[id][0].imageMode;
    assetsSetPixelSize(entry);
    entry->size = entry->imageSize[0] * entry->imageSize[1] * entry->pixSize;

    strncpy(entry->path, path, sizeof(entry->path)-1);

    const char* ext = getExtension(entry->path);
    bool ret=true;
    if (ext==NULL) ret = false;

    u8 *data_buf = NULL;
    struct stat st;

    if (ret && stat(path, &st)==-1) ret = false;

    if (ret) {
        entry->buffer = (u8*)malloc(entry->size);
        if (entry->buffer) memset(entry->buffer, 0, entry->size);
        else ret = false;
    }

    if (ret) {
        data_buf = (u8*)malloc(st.st_size);
        if (data_buf) memset(data_buf, 0, st.st_size);
        else ret = false;
    }

    if (ret) {
        FILE *f = fopen(entry->path, "rb");
        if (f==NULL) ret = false;
        else {
            ret = fread(data_buf, st.st_size, 1, f) == 1;
            fclose(f);
        }
    }

    if (ret) {
        if (strcasecmp(ext, ".bin")==0) {
            if (st.st_size != entry->size) ret = false;

            if (ret) memcpy(entry->buffer, data_buf, entry->size);
        }
        else if (strcasecmp(ext, ".jpg")==0 || strcasecmp(ext, ".jpeg")==0)
            ret = assetsLoadJpgFromMemory(data_buf, st.st_size, entry->buffer, entry->imageMode, entry->imageSize[0], entry->imageSize[1]);
        else if (strcasecmp(ext, ".png")==0)
            ret = assetsLoadPngFromMemory(data_buf, st.st_size, entry->buffer, entry->imageMode, entry->imageSize[0], entry->imageSize[1]);
        else
            ret = false; // File extension not recognized.
    }

    if (ret) entry->initialized = true;
    else assetsClearEntry(entry);

    free(data_buf);

    return ret;
}

void assetsGetData(AssetId id, assetsDataEntry **out) {
    if (out) *out = NULL;
    if (id < 0 || id >= AssetId_Max) return;

    u32 pos = g_assetsDataList[id][1].initialized ? 1 : 0;
    assetsDataEntry *entry = &g_assetsDataList[id][pos];
    if (entry->initialized) *out = entry;
}

u8 *assetsGetDataBuffer(AssetId id) {
    assetsDataEntry *entry = NULL;

    assetsGetData(id, &entry);
    return entry ? entry->buffer : NULL;
}

