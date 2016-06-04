#if !defined(BOBLINGFONT_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tunder $
   $Notice: (C) Copyright 2014 by SpaceCat, Inc. All Rights Reserved. $
   ======================================================================== */

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <windows.h>
#include "bpng.h"

#define BFONT_NAME_MAX_LEN 32
#define BFONT_RANGE_BEG 32 // ASCII for space
#define BFONT_RANGE_END 127 // ASCII range
#define BFONT_RANGE (BFONT_RANGE_END-BFONT_RANGE_BEG)

enum
{
    BFONT_MIPMAP = 0x1,
    BFONT_ITALIC = 0x2,
    BFONT_UNDERLINE = 0x4,
    BFONT_STRIKEOUT = 0x8,
    BFONT_ANTIALIAS = 0x16,
    BFONT_PACK = 0x32,
};

struct bFontInfo
{
    char name[BFONT_NAME_MAX_LEN];
    uint8_t size, weight, r, g, b, shadowx, shadowy, paddingx, paddingy;
    //uint8_t mipmap, italic, underline, strikeout;
    uint8_t flags;
    HFONT fontHandle;
};

#define BOBLINGFONT_H
#endif

