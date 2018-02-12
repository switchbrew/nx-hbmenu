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
    AssetHeader asset_header;

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
        || asset_header.magic != ASSETHEADER_MAGICNUM
        || asset_header.version > ASSETHEADER_VERSION
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
    AssetHeader asset_header;

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
        || asset_header.magic != ASSETHEADER_MAGICNUM
        || asset_header.version > ASSETHEADER_VERSION
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
        snprintf(tempbuf, sizeof(tempbuf)-1, "%.*s/%.*s.nro", (int)sizeof(tempbuf)/2, me->path, (int)sizeof(tempbuf)/2-7, name);
        bool found = fileExists(tempbuf);

        if (found)
        {
            //isOldAppFolder = true;
            shortcut = false;
            me->type = ENTRY_TYPE_FILE;
            strcpy(me->path, tempbuf);
        } /*else
            strcpy(me->name, textGetString(StrId_Directory));*/
    }

    if (me->type == ENTRY_TYPE_FILE)
    {
        strcpy(me->name, name);
        strcpy(me->author, textGetString(StrId_DefaultPublisher));
        strcpy(me->version, textGetString(StrId_DefaultVersion));

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

    return true;
}

void menuEntryParseIcon(menuEntry_s* me) {
    uint8_t *imageptr = NULL;
    size_t imagesize = 256*256*3;

    if (me->icon_size==0 || me->icon==NULL) return;

    njInit();

    if (njDecode(me->icon, me->icon_size) != NJ_OK) {
        njDone();
        return;
    }

    me->icon_size = 0;
    free(me->icon);
    me->icon = NULL;

    if ((njGetWidth() != 256 || njGetHeight() != 256 || (size_t)njGetImageSize() != imagesize) || njIsColor() != 1) {//The decoded image must be RGB and 256x256.
        njDone();
        return;
    }

    imageptr = njGetImage();
    if (imageptr == NULL) {
        njDone();
        return;
    }

    me->icon_gfx = (uint8_t*)malloc(imagesize);
    if (me->icon_gfx == NULL) {
        njDone();
        return;
    }

    memcpy(me->icon_gfx, imageptr, imagesize);

    njDone();
}

void menuEntryParseNacp(menuEntry_s* me) {
    int lang = 0;//TODO: Update this once libnx supports settings get-language.

    if (me->nacp==NULL) return;

    strncpy(me->name, me->nacp->lang[lang].name, sizeof(me->name)-1);
    strncpy(me->author, me->nacp->lang[lang].author, sizeof(me->author)-1);
    strncpy(me->version, me->nacp->version, sizeof(me->version)-1);

    free(me->nacp);
    me->nacp = NULL;
}

