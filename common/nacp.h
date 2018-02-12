#pragma once

typedef struct {
    char name[0x200];
    char author[0x100];
} NacpLanguageEntry;

typedef struct {
    NacpLanguageEntry lang[12];
    NacpLanguageEntry lang_unk[4];//?

    u8  x3000_unk[0x24];////Normally all-zero?
    u32 x3024_unk;
    u32 x3028_unk;
    u32 x302C_unk;
    u32 x3030_unk;
    u32 x3034_unk;
    u64 titleid0;

    u8 x3040_unk[0x20];
    char version[0x10];

    u64 titleid_dlcbase;
    u64 titleid1;

    u32 x3080_unk;
    u32 x3084_unk;
    u32 x3088_unk;
    u8 x308C_unk[0x24];//zeros?

    u64 titleid2;
    u64 titleids[7];//"Array of application titleIDs, normally the same as the above app-titleIDs. Only set for game-updates?"

    u32 x30F0_unk;
    u32 x30F4_unk;

    u64 titleid3;//"Application titleID. Only set for game-updates?"

    char bcat_passphrase[0x40];
    u8 x3140_unk[0xEC0];//Normally all-zero?
} NacpStruct;
