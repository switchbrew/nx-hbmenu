#pragma once

int netloader_activate(void);
int netloader_deactivate(void);
int netloader_loop(menuEntry_s *me);

bool netloaderInit(void);
void netloaderExit(void);

void netloaderTask(void* arg);

void netloaderGetState(bool *activated, bool *launch_app, menuEntry_s **me, char *errormsg, size_t errormsg_size);
void netloaderSignalExit(void);
