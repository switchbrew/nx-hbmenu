#include "text.h"

//TODO: Update this once libnx supports settings get-language.

static int s_textLang = /*CFG_LANGUAGE_EN*/1;

int textGetLang(void) {
    return s_textLang;
}

const char* textGetString(StrId id) {
    const char* str = g_strings[id][s_textLang];
    if (!str) str = g_strings[id][/*CFG_LANGUAGE_EN*/1];
    return str;
}
