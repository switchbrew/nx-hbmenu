#include "common.h"

void menuEntryInit(menuEntry_s* me, MenuEntryType type) {
    memset(me, 0, sizeof(*me));
    me->type = type;
}

void menuEntryFree(menuEntry_s* me) {
    me->icon_size = 0;
    if (me->icon) {
        free(me->icon);
        me->icon = NULL;
    }

    if (me->icon_gfx) {
        free(me->icon_gfx);
        me->icon_gfx = NULL;
    }

    if (me->icon_gfx_small) {
        free(me->icon_gfx_small);
        me->icon_gfx_small = NULL;
    }

    if (me->nacp) {
        free(me->nacp);
        me->nacp = NULL;
    }
}

bool fileExists(const char* path) {
    struct stat st;
    return stat(path, &st)==0 && S_ISREG(st.st_mode);
}

static bool menuEntryLoadEmbeddedIcon(menuEntry_s* me) {
    NroHeader header;
    NroAssetHeader asset_header;

    FILE* f = fopen(me->path, "rb");
    if (!f) return false;

    fseek(f, sizeof(NroStart), SEEK_SET);
    if (fread(&header, sizeof(header), 1, f) != 1)
    {
        fclose(f);
        return false;
    }

    fseek(f, header.size, SEEK_SET);

    if (fread(&asset_header, sizeof(asset_header), 1, f) != 1
        || asset_header.magic != NROASSETHEADER_MAGIC
        || asset_header.version > NROASSETHEADER_VERSION
        || asset_header.icon.offset == 0
        || asset_header.icon.size == 0)
    {
        fclose(f);
        return false;
    }

    me->icon_size = asset_header.icon.size;
    me->icon = (uint8_t*)malloc(me->icon_size);
    if (me->icon == NULL) {
        fclose(f);
        return false;
    }
    memset(me->icon, 0, me->icon_size);

    fseek(f, header.size + asset_header.icon.offset, SEEK_SET);
    bool ok = fread(me->icon, me->icon_size, 1, f) == 1;
    fclose(f);
    return ok;
}

static bool menuEntryLoadEmbeddedNacp(menuEntry_s* me) {
    NroHeader header;
    NroAssetHeader asset_header;

    FILE* f = fopen(me->path, "rb");
    if (!f) return false;

    fseek(f, sizeof(NroStart), SEEK_SET);
    if (fread(&header, sizeof(header), 1, f) != 1)
    {
        fclose(f);
        return false;
    }

    fseek(f, header.size, SEEK_SET);

    if (fread(&asset_header, sizeof(asset_header), 1, f) != 1
        || asset_header.magic != NROASSETHEADER_MAGIC
        || asset_header.version > NROASSETHEADER_VERSION
        || asset_header.nacp.offset == 0
        || asset_header.nacp.size == 0)
    {
        fclose(f);
        return false;
    }

    if (asset_header.nacp.size < sizeof(NacpStruct))
    {
        fclose(f);
        return false;
    }

    me->nacp = (NacpStruct*)malloc(sizeof(NacpStruct));
    if (me->nacp == NULL) {
        fclose(f);
        return false;
    }

    fseek(f, header.size + asset_header.nacp.offset, SEEK_SET);
    bool ok = fread(me->nacp, sizeof(NacpStruct), 1, f) == 1;
    fclose(f);
    return ok;
}

/*static void fixSpaceNewLine(char* buf) {
    char *outp = buf, *inp = buf;
    char lastc = 0;
    do
    {
        char c = *inp++;
        if (c == ' ' && lastc == ' ')
            outp[-1] = '\n';
        else
            *outp++ = c;
        lastc = c;
    } while (lastc);
}*/

bool menuEntryLoad(menuEntry_s* me, const char* name, bool shortcut) {
    static char tempbuf[PATH_MAX+1];
    //bool isOldAppFolder = false;

    tempbuf[PATH_MAX] = 0;
    strcpy(me->name, name);
    
    if (me->type == ENTRY_TYPE_FOLDER)
    {
        //Check for <dirpath>/<dirname>.nro
        snprintf(tempbuf, sizeof(tempbuf)-1, "%.*s/%.*s.nro", (int)sizeof(tempbuf)/2, me->path, (int)sizeof(tempbuf)/2-7, name);
        bool found = fileExists(tempbuf);

        //Use the first .nro found in the directory, if there's only 1 NRO in the directory. Only used for paths starting with "sdmc:/switch/".
        if (!found && strncmp(me->path, "sdmc:/switch/", 13)==0) {
            DIR* dir;
            struct dirent* dp;
            u32 nro_count=0;

            dir = opendir(me->path);
            if (dir) {
                while ((dp = readdir(dir))) {
                    if (dp->d_name[0]=='.')//Check this here so that it's consistent with menuScan().
                        continue;

                    const char* ext = getExtension(dp->d_name);
                    if (strcasecmp(ext, ".nro")==0) {
                        nro_count++;
                        if (nro_count>1) {
                            found = 0;
                            break;
                        }

                        snprintf(tempbuf, sizeof(tempbuf)-1, "%.*s/%.*s", (int)sizeof(tempbuf)/2, me->path, (int)sizeof(tempbuf)/2-7, dp->d_name);
                        found = fileExists(tempbuf);
                    }
                }
                closedir(dir);
            }
        }

        if (found)
        {
            //isOldAppFolder = true;
            shortcut = false;
            me->type = ENTRY_TYPE_FILE;
            strcpy(me->path, tempbuf);
        }
    }

    if (me->type == ENTRY_TYPE_FILE)
    {
        //strcpy(me->name, name);//This is already done before both if statements
        strcpy(me->author, textGetString(StrId_DefaultPublisher));
        strcpy(me->version, "1.0.0");

        //shortcut_s sc;

        /*if (shortcut)
        {
            if (R_FAILED(shortcutCreate(&sc, me->path)))
                return false;
            if (!fileExists(sc.executable))
            {
                shortcutFree(&sc);
                return false;
            }
            strcpy(me->path, "sdmc:");
            strcat(me->path, sc.executable);
        }*/

        bool iconLoaded = false;

        // Load the icon
        /*if (shortcut)
        {
            FILE* f = sc.icon ? fopen(sc.icon, "rb") : NULL;
            if (f)
            {
                iconLoaded = fread(&me->smdh, sizeof(smdh_s), 1, f) == 1;
                fclose(f);
            }
        }*/

        if (!iconLoaded) do
        {
            // Attempt loading external icon
            /*strcpy(tempbuf, me->path);
            char* ext = getExtension(tempbuf);

            strcpy(ext, ".jpg");
            iconLoaded = menuEntryLoadExternalIcon(me, tempbuf);
            if (iconLoaded) break;

            if (isOldAppFolder)
            {
                char* slash = getSlash(tempbuf);

                strcpy(slash, "/icon.jpg");
                iconLoaded = menuEntryLoadExternalIcon(me, tempbuf);
                if (iconLoaded) break;
            }*/

            // Attempt loading the embedded icon
            if (!shortcut)
                iconLoaded = menuEntryLoadEmbeddedIcon(me);
        } while (0);

        if (iconLoaded)
        {
            menuEntryParseIcon(me);
        }

        bool nacpLoaded = false;

        nacpLoaded = menuEntryLoadEmbeddedNacp(me);

        if (nacpLoaded)
        {
            menuEntryParseNacp(me);

            // Fix description for some applications using multiple spaces to indicate newline
            //fixSpaceNewLine(me->description);
        }

        // Metadata overrides for shortcuts
        /*if (shortcut)
        {
            if (sc.name) strncpy(me->name, sc.name, ENTRY_NAMELENGTH);
            if (sc.description) strncpy(me->description, sc.description, ENTRY_DESCLENGTH);
            if (sc.author) strncpy(me->author, sc.author, ENTRY_AUTHORLENGTH);
        }*/

        // Load the descriptor
        /*if (shortcut && sc.descriptor && fileExists(sc.descriptor))
            descriptorLoad(&me->descriptor, sc.descriptor);
        else
        {
            strcpy(tempbuf, me->path);
            strcpy(getExtension(tempbuf), ".xml");
            bool found = fileExists(tempbuf);
            if (!found && isOldAppFolder)
            {
                strcpy(tempbuf, me->path);
                strcpy(getSlash(tempbuf), "/descriptor.xml");
                found = fileExists(tempbuf);
            }
            if (found)
                descriptorLoad(&me->descriptor, tempbuf);
        }*/

        // Initialize the argument data
        argData_s* ad = &me->args;
        ad->dst = (char*)&ad->buf[1];
        launchAddArg(ad, me->path);

        // Load the argument(s) from the shortcut
        /*if (shortcut && sc.arg && *sc.arg)
            launchAddArgsFromString(ad, sc.arg);*/

        /*if (shortcut)
            shortcutFree(&sc);*/
    }
    
    if (me->type == ENTRY_TYPE_THEME) {
        config_t cfg = {0};
        config_init(&cfg);
        config_setting_t *themeInfo;
        const char *name,
                   *author = textGetString(StrId_DefaultPublisher),
                   *version = "1.0.0";
                   
        if (config_read_file(&cfg, me->path)) {
            themeInfo = config_lookup(&cfg, "themeInfo");
            if (themeInfo != NULL) {
                if(config_setting_lookup_string(themeInfo, "name", &name))
                    strncpy(me->name, name, sizeof(me->name)-1);
                config_setting_lookup_string(themeInfo, "author", &author);
                config_setting_lookup_string(themeInfo, "version", &version);
            }
        }

        strncpy(me->author, author, sizeof(me->author)-1);
        strncpy(me->version, version, sizeof(me->version)-1);
        config_destroy(&cfg);
    }

    return true;
}

void menuEntryParseIcon(menuEntry_s* me) {
    if (me->icon_size==0 || me->icon==NULL) return;

    int w,h,samp;
    size_t imagesize = 256*256*3;
    me->icon_gfx = (uint8_t*)malloc(imagesize);

    if (me->icon_gfx == NULL) return;

    tjhandle _jpegDecompressor = tjInitDecompress();

    if (_jpegDecompressor == NULL) {
        free(me->icon_gfx);
        me->icon_gfx = NULL;
        return;
    }

    if (tjDecompressHeader2(_jpegDecompressor, me->icon, me->icon_size, &w, &h, &samp) == -1) {
        free(me->icon_gfx);
        me->icon_gfx = NULL;
        tjDestroy(_jpegDecompressor);
        return;
    }

    if (w != 256 || h != 256 ) return;

    if (tjDecompress2(_jpegDecompressor, me->icon, me->icon_size, me->icon_gfx, w, 0, h, TJPF_RGB, TJFLAG_ACCURATEDCT) == -1) {
        free(me->icon_gfx);
        me->icon_gfx = NULL;
        tjDestroy(_jpegDecompressor);
        return;
    }

    me->icon_size = 0;
    free(me->icon);
    me->icon = NULL;

    tjDestroy(_jpegDecompressor);

    me->icon_gfx_small = downscaleImg(me->icon_gfx, 256, 256, 140, 140, IMAGE_MODE_RGB24);
}

uint8_t *downscaleImg(const uint8_t *image, int srcWidth, int srcHeight, int destWidth, int destHeight, ImageMode mode) {
    uint8_t *out;

    switch (mode) {
        case IMAGE_MODE_RGBA32:
            out = (uint8_t*)malloc(destWidth*destHeight*4);
            break;

        default:
            out = (uint8_t*)malloc(destWidth*destHeight*3);
            break;
    }

    if (out == NULL) {
        return NULL;
    }

    int tmpx, tmpy;
    int pos;
    float sourceX, sourceY;
    float xScale = (float)srcWidth / (float)destWidth;
    float yScale = (float)srcHeight / (float)destHeight;
    int pixelX, pixelY;
    uint8_t r1, r2, r3, r4;
    uint8_t g1, g2, g3, g4;
    uint8_t b1, b2, b3, b4;
    uint8_t a1, a2, a3, a4;
    float fx, fy, fx1, fy1;
    int w1, w2, w3, w4;

    for (tmpx=0; tmpx<destWidth; tmpx++) {
        for (tmpy=0; tmpy<destHeight; tmpy++) {
            sourceX = tmpx * xScale;
            sourceY = tmpy * yScale;
            pixelX = (int)sourceX;
            pixelY = (int)sourceY;

            // get colours from four surrounding pixels
            if (mode == IMAGE_MODE_RGBA32) 
                pos = ((pixelY + 0) * srcWidth + pixelX + 0) * 4;
            else
                pos = ((pixelY + 0) * srcWidth + pixelX + 0) * 3;

            r1 = image[pos+0];
            g1 = image[pos+1];
            b1 = image[pos+2];
            
            if (mode == IMAGE_MODE_RGBA32) 
                a1 = image[pos+3];


            if (mode == IMAGE_MODE_RGBA32) 
                pos = ((pixelY + 0) * srcWidth + pixelX + 1) * 4;
            else
                pos = ((pixelY + 0) * srcWidth + pixelX + 1) * 3;

            r2 = image[pos+0];
            g2 = image[pos+1];
            b2 = image[pos+2];

            if (mode == IMAGE_MODE_RGBA32) 
                a2 = image[pos+3];


            if (mode == IMAGE_MODE_RGBA32) 
                pos = ((pixelY + 1) * srcWidth + pixelX + 0) * 4;
            else
                pos = ((pixelY + 1) * srcWidth + pixelX + 0) * 3;

            r3 = image[pos+0];
            g3 = image[pos+1];
            b3 = image[pos+2];

            if (mode == IMAGE_MODE_RGBA32) 
                a3 = image[pos+3];


            if (mode == IMAGE_MODE_RGBA32) 
                pos = ((pixelY + 1) * srcWidth + pixelX + 1) * 4;
            else
                pos = ((pixelY + 1) * srcWidth + pixelX + 1) * 3;

            r4 = image[pos+0];
            g4 = image[pos+1];
            b4 = image[pos+2];

            if (mode == IMAGE_MODE_RGBA32) 
                a4 = image[pos+3];

            // determine weights
            fx = sourceX - pixelX;
            fy = sourceY - pixelY;
            fx1 = 1.0f - fx;
            fy1 = 1.0f - fy;

            w1 = (int)(fx1*fy1*256.0);
            w2 = (int)(fx*fy1*256.0);
            w3 = (int)(fx1*fy*256.0);
            w4 = (int)(fx*fy*256.0);
 
            // set output pixels
            if (mode == IMAGE_MODE_RGBA32) 
                pos = ((tmpy*destWidth) + tmpx) * 4;
            else
                pos = ((tmpy*destWidth) + tmpx) * 3;

            out[pos+0] = (uint8_t)((r1 * w1 + r2 * w2 + r3 * w3 + r4 * w4) >> 8);
            out[pos+1] = (uint8_t)((g1 * w1 + g2 * w2 + g3 * w3 + g4 * w4) >> 8);
            out[pos+2] = (uint8_t)((b1 * w1 + b2 * w2 + b3 * w3 + b4 * w4) >> 8);

            if (mode == IMAGE_MODE_RGBA32) 
                out[pos+3] = (uint8_t)((a1 * w1 + a2 * w2 + a3 * w3 + a4 * w4) >> 8);
        }
    }

    return out;
}

void menuEntryParseNacp(menuEntry_s* me) {
    NacpLanguageEntry *langentry = NULL;

    if (me->nacp==NULL) return;

    strncpy(me->version, me->nacp->version, sizeof(me->version)-1);

    #ifdef __SWITCH__
    Result rc=0;
    rc = nacpGetLanguageEntry(me->nacp, &langentry);

    if (R_SUCCEEDED(rc) && langentry!=NULL) {
    #else
    langentry = &me->nacp->lang[0];
    if (1) {
    #endif
        strncpy(me->name, langentry->name, sizeof(me->name)-1);
        strncpy(me->author, langentry->author, sizeof(me->author)-1);
    }

    free(me->nacp);
    me->nacp = NULL;
}

