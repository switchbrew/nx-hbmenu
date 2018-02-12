#pragma once

#define NROHEADER_MAGICNUM 0x304f524e

#define ASSETHEADER_MAGICNUM 0x54455341
#define ASSETHEADER_VERSION 0

typedef struct {
    u32 FileOff;
    u32 Size;
} NsoSegment;

typedef struct {
    u32 unused;
    u32 modOffset;
    u8 Padding[8];
} NroStart;

typedef struct {
    u32  Magic;
    u32 Unk1;
    u32 size;
    u32 Unk2;
    NsoSegment Segments[3];
    u32 bssSize;
    u32 Unk3;
    u8  BuildId[0x20];
    u8  Padding[0x20];
} NroHeader;

typedef struct {
    u64 offset;
    u64 size;
} AssetSection;

typedef struct {
    u32 magic;
    u32 version;
    AssetSection icon;
    AssetSection nacp;
    AssetSection romfs;
} AssetHeader;

