#include "../common/common.h"

static u32 argBuf[ENTRY_ARGBUFSIZE/sizeof(u32)];

static bool init(void)
{
    return envHasNextLoad();
}

static void deinit(void)
{
    
}

static void launchFile(const char* path, argData_s* args)
{
    /*if (strncmp(path, "sdmc:/",6) == 0)
        path += 5;*/
    memcpy(argBuf, args->buf, sizeof(args->buf));
    Result rc = envSetNextLoad(path, (char*)argBuf);
    if(R_FAILED(rc)) fatalSimple(rc);//TODO: How should failing be handled?
    uiExitLoop();
}

const loaderFuncs_s loader_builtin =
{
    .name = "builtin",
    .init = init,
    .deinit = deinit,
    .launchFile = launchFile,
};

