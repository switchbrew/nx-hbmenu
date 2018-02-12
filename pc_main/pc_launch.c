#include "../common/common.h"

static const loaderFuncs_s* s_loader;

void launchInit(void) {
    
}

void launchExit(void) {
    //s_loader->deinit();
}

const loaderFuncs_s* launchGetLoader(void) {
    return s_loader;
}

void launchMenuEntry(menuEntry_s* me) {
    
}
