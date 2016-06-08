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
#include "bpng.h"

#ifdef WIN32
#include <windows.h>
#endif

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
    BFONT_ANTIALIAS = 0x10,
    BFONT_PACK = 0x20,
    BFONT_PACK_ALL = 0x40,
};

struct bFontInfo
{
    char name[BFONT_NAME_MAX_LEN];
    uint8_t size, weight;
    uint8_t r, g, b;
    uint8_t shadowx, shadowy;
    uint8_t paddingx, paddingy;
    uint8_t mipBeg, mipEnd, mipAdd;
    //uint8_t mipmap, italic, underline, strikeout;
    uint8_t flags;
    
#ifdef WIN32
    HFONT fontHandle;
#endif
};

#define BOBLINGFONT_H
#endif

