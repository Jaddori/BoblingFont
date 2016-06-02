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

#define BL_FONT_NAME_MAX_LEN 32

struct blFontInfo
{
    char name[BL_FONT_NAME_MAX_LEN];
    uint8_t size, weight, r, g, b, shadowx, shadowy;
    uint8_t mipmap, italic, underline, strikeout;
    HFONT fontHandle;
};

#define BOBLINGFONT_H
#endif
