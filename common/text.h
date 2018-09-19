#pragma once
#include "common.h"
#include "language.h"

void textInit(void);
int textGetLang(void);
const char* textGetString(StrId id);
u64 textGetLanguageCode(void);
uint32_t getXCoordinate(u32 font,uint32_t rX, const char* text,const char align);
uint32_t getYCoordinate(u32 font,uint32_t rY, const char* text,const char align);
