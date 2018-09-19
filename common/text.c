#include "text.h"

static u64 s_textLanguageCode = 0;

#ifdef __SWITCH__
static int s_textLang = SetLanguage_ENUS;
#else
static int s_textLang = 1;
#endif

void textInit(void) {
    #ifdef __SWITCH__
    s32 Language=0;

    s_textLang = SetLanguage_ENUS;

    Result rc = setInitialize();
    if (R_SUCCEEDED(rc)) rc = setGetSystemLanguage(&s_textLanguageCode);
    if (R_SUCCEEDED(rc)) rc = setMakeLanguage(s_textLanguageCode, &Language);
    //if (R_SUCCEEDED(rc) && Language < 16) s_textLang = Language;//TODO: Re-enable this once language.c supports all used languages.
    setExit();
    if (R_FAILED(rc)) fatalSimple(-8);
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

u64 textGetLanguageCode(void) {
    return s_textLanguageCode;
}
/*Automatically gives you the desired x-coordinate 
 *based on the string length and desired alignmenr
 *rY=reference point... where to align around
 *align='t','b','c' translates too (top,bottom,center)
 *'t' aligned, top of text aligns with rY, 
 *you get the rest....
 */
uint32_t getYCoordinate(u32 font,uint32_t rY,const char* text, const char align){
    uint32_t height_o,width;
    GetTextDimensions(font,text,&width,&height_o);
    uint32_t height = (uint32_t)height_o;
    uint32_t fC = (rY-height);
    switch(align){
        case 't':
        default:
            return rY;
        case 'c':
            return (rY+(height/2U));
        case 'b':
            if(fC<=0U) return 0U;
            else return fC;
    }
}

/*Automatically gives you the desired x-coordinate 
 *based on the string length and desired alignmenr
 *rX=reference point... where to align around
 *text=string you want to display
 *align='r','l','c' translates too (right,left,center)
 *'r' aligned, rX location = end of string, you get the rest...
 */
uint32_t getXCoordinate(u32 font,uint32_t rX, const char* text ,const char align){
    uint32_t height,width_o;
    GetTextDimensions(font,text,&width_o,&height);
    uint32_t fC = (rX-width_o);
    switch(align){
        case 'r':
            if(fC<0U) return 0U;
            else return fC;
        case 'c':
            return (rX+(width_o/2U));
        case 'l':
        default:
            return rX;
    }
}