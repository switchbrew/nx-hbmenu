#include "text.h"

#ifdef __SWITCH__
static int s_textLang = SetLanguage_ENUS;
#else
static int s_textLang = 1;
#endif

void textInit(void) {
    #ifdef __SWITCH__
    //u64 LanguageCode=0;
    //s32 Language=0;

    s_textLang = SetLanguage_ENUS;
    //TODO: Re-enable this once the font supports all used languages.
    /*Result rc = setInitialize();
    if (R_SUCCEEDED(rc)) rc = setGetSystemLanguage(&LanguageCode);
    if (R_SUCCEEDED(rc)) rc = setMakeLanguage(LanguageCode, &Language);
    if (R_SUCCEEDED(rc) && Language < 16) s_textLang = Language;
    setExit();*/
    #else
    s_textLang = 1;
    #endif
}      

int textGetLang(void) {
    return s_textLang;
}

const char* textGetString(StrId id) {
    const char* str = g_strings[id][s_textLang];
    #ifdef __SWITCH__
    if (!str) str = g_strings[id][SetLanguage_ENUS];
    #else                              
    if (!str) str = g_strings[id][1];
    #endif
    return str;
}
