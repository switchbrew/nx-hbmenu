#include "../common/common.h"

static const loaderFuncs_s* s_loader;

bool launchInit(void) {
#define ADD_LOADER(_name) do \
    { \
        extern const loaderFuncs_s _name; \
        if (_name.init()) \
        { \
            s_loader = &_name; \
            return 1; \
        } \
    } while(0)

    ADD_LOADER(loader_builtin);

    // Shouldn't happen
    s_loader = NULL;
    return 0;
}

void launchExit(void) {
    if (s_loader) s_loader->deinit();
    s_loader = NULL;
}

const loaderFuncs_s* launchGetLoader(void) {
    return s_loader;
}

bool launchMenuEntryLoadABIRevision(menuEntry_s* me) {
    NroStart start;

    FILE* f = fopen(me->path, "rb");
    if (!f) return false;

    if (fread(&start, sizeof(start), 1, f) != 1)
    {
        fclose(f);
        return false;
    }

    fseek(f, start.mod_offset + 0x34, SEEK_SET);

    u32 magic = 0;

    if (fread(&magic, sizeof(magic), 1, f) != 1
        || magic != NRO_ABI_MAGIC)
    {
        fclose(f);
        return false;
    }

    me->abi_revision = 0;
    if (fread(&me->abi_revision, sizeof(me->abi_revision), 1, f) != 1)
    {
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}

void launchMenuEntry(menuEntry_s* me) {
    /*bool canUseTitles = loaderCanUseTitles();
    if (me->descriptor.numTargetTitles && canUseTitles)
    {
        // Update the list of available titles
        titlesCheckUpdate(false, UI_STATE_NULL);

        int i;
        for (i = 0; i < me->descriptor.numTargetTitles; i ++)
            if (titlesExists(me->descriptor.targetTitles[i].tid, me->descriptor.targetTitles[i].mediatype))
                break;

        if (i == me->descriptor.numTargetTitles)
        {
            errorScreen(s_loader->name, textGetString(StrId_MissingTargetTitle));
            return;
        }

        // Use the title
        s_loader->useTitle(me->descriptor.targetTitles[i].tid, me->descriptor.targetTitles[i].mediatype);
    } else if (me->descriptor.selectTargetProcess)
    {
        if (!canUseTitles)
        {
            errorScreen(s_loader->name, textGetString(StrId_NoTargetTitleSupport));
            return;
        }

        // Launch the title selector
        if (!me->titleSelected)
        {
            titleSelectInit(me);
            return;
        }

        // Use the title
        s_loader->useTitle(me->titleId, me->titleMediatype);
    }*/

    // Scan the executable if needed
    /*if (loaderHasFlag(LOADER_NEED_SCAN))
        descriptorScanFile(&me->descriptor, me->path);*/

    // Launch it
    if (s_loader == NULL) return;
    s_loader->launchFile(me->path, &me->args);
}
