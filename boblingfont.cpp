/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tunder $
   $Notice: (C) Copyright 2014 by SpaceCat, Inc. All Rights Reserved. $
   ======================================================================== */

#include "boblingfont.h"

// get command line arguments
// set flags
// load font
// draw font
// save file
// exit gracefully

// flags:
// font name
// size
// weight
// color
// shadow
// "mip-map"
// style
// output name
// anti-aliasing

int CALLBACK FontEnumCallback( const LOGFONT* lf, const TEXTMETRIC* m,
                               DWORD fontType, LPARAM lparam )
{
    int result = 1; // 1 means continue enumerating

    std::cout << "Looking at font: " << lf->lfFaceName << std::endl;
    
    blFontInfo *font = (blFontInfo*)lparam;
    if( strcmp( lf->lfFaceName, font->name ) == 0 )
    {
        font->fontHandle = CreateFont( m->tmHeight,
                                       0,0, 0,
                                       m->tmWeight,
                                       m->tmItalic,
                                       m->tmUnderlined,
                                       m->tmStruckOut,
                                       m->tmCharSet,
                                       OUT_TT_PRECIS,
                                       CLIP_DEFAULT_PRECIS,
                                       ANTIALIASED_QUALITY, // TODO: Add as flag
                                       FF_DONTCARE,
                                       lf->lfFaceName );
        
        result = 0; // 0 means stop enumerating
    }
    
    return result;
}

int main( int argc, char* argv[] )
{
    /*const char *inputName, *outputName;
    int fontsize, weight, r, g, b, shadowx, shadowy;
    bool mipmap, italic, underline, strikeout;

    inputName = outputName = nullptr;
    fontsize = weight = r = g = b = shadowx = shadowy = 0;
    mipmap = italic = underline = strikeout = false;*/

    blFontInfo font = {};

    if( argc > 1 )
    {
        // TODO: Make sure inputName is no longer than 32 TCHARs
        strncpy( font.name, argv[1], 32 );
        font.name[31] = 0;

        std::cout << "Trying to load font: " << font.name << std::endl;

        HWND windowHandle = GetConsoleWindow();
        HDC deviceContext = GetDC( windowHandle );

        if( windowHandle == NULL )
            std::cout << "Can't find window handle." << std::endl;
        if( deviceContext == NULL )
            std::cout << "Can't find device context." << std::endl;
        
        LOGFONT logFont = {};
        logFont.lfHeight = font.size;
        logFont.lfWeight = font.weight;
        logFont.lfItalic = font.italic;
        logFont.lfUnderline = font.underline;
        logFont.lfStrikeOut = font.strikeout;
        logFont.lfCharSet = ANSI_CHARSET;
        logFont.lfOutPrecision = OUT_TT_PRECIS;
        logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
        logFont.lfQuality = ANTIALIASED_QUALITY;
        logFont.lfPitchAndFamily = FF_DONTCARE;
        strncpy( logFont.lfFaceName, font.name, 32 );

        EnumFontFamiliesEx( deviceContext, &logFont, &FontEnumCallback, (LPARAM)&font, 0 );

        if( font.fontHandle != NULL )
        {
            std::cout << "Found font: " << font.name << std::endl;

            HDC imgContext = CreateCompatibleDC( deviceContext );
            if( imgContext )
            {
                //SelectObject( imgContext, font.fontHandle );

                //TEXTMETRICS textMetrics;
                //GetTextMetrics( imgContext, &textMetrics );
            }
            else
                std::cout << "Failed to create image context." << std::endl;
        }
        else
            std::cout << "Failed to open font: " << font.name << std::endl;
    }
    
    return 0;
}
