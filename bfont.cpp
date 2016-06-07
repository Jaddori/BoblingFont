
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Tunder $
   $Notice: (C) Copyright 2014 by SpaceCat, Inc. All Rights Reserved. $
   ======================================================================== */

#include "bfont.h"
#include <string>
#include <sstream>
#include <fstream>

// get command line arguments
// set flags
// load font
// draw font
// save file
// exit gracefully

// Flags:
// font name        *
// size             *
// weight           *
// rgb              *
// shadow           *
// "mip-map"        *
// italic           *
// underline        *
// strikeout        *
// output name      *
// anti-aliasing    *
// padding          *
// packing          *

int CALLBACK FontEnumCallback( const LOGFONT* lf, const TEXTMETRIC* m,
                               DWORD fontType, LPARAM lparam )
{
    int result = 1; // 1 means continue enumerating
    
    bFontInfo *font = (bFontInfo*)lparam;
    if( strcmp( lf->lfFaceName, font->name ) == 0 )
    {
        font->fontHandle = CreateFont( font->size,
                                       0, 0, 0,
                                       font->weight,
                                       font->flags & BFONT_ITALIC,
                                       font->flags & BFONT_UNDERLINE,
                                       font->flags & BFONT_STRIKEOUT,
                                       m->tmCharSet,
                                       OUT_TT_PRECIS,
                                       CLIP_DEFAULT_PRECIS,
                                       ( font->flags & BFONT_ANTIALIAS ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY ),
                                       FF_DONTCARE,
                                       lf->lfFaceName );
        
        result = 0; // 0 means stop enumerating
    }
    
    return result;
}

static void LoadFont( bFontInfo* font, HDC deviceContext )
{
    LOGFONT logFont = {};
    logFont.lfHeight = font->size;
    logFont.lfWeight = font->weight;
    logFont.lfItalic = font->flags & BFONT_ITALIC;
    logFont.lfUnderline = font->flags & BFONT_UNDERLINE;
    logFont.lfStrikeOut = font->flags & BFONT_STRIKEOUT;
    logFont.lfCharSet = ANSI_CHARSET;
    logFont.lfOutPrecision = OUT_TT_PRECIS;
    logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logFont.lfQuality = ( font->flags & BFONT_ANTIALIAS ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY);
    logFont.lfPitchAndFamily = FF_DONTCARE;
    strncpy( logFont.lfFaceName, font->name, 32 );

    if( font->fontHandle != NULL )
    {
        DeleteObject( font->fontHandle );
        font->fontHandle = NULL;
    }
    
    EnumFontFamiliesEx( deviceContext, &logFont, &FontEnumCallback, (LPARAM)font, 0 );
}

static void RenderFont( bFontInfo* font, HDC imgContext, void* canvasData,
                        int* glyphWidths, int glyphHeight, int bitmapSize )
{
    int x = 0, y = 0;
    char c = BFONT_RANGE_BEG;
    for( int i=0; i<BFONT_RANGE; i++,c++ )
    {
        if( x + glyphWidths[i] + font->shadowx + font->paddingx >= bitmapSize )
        {
            x = 0;
            y += glyphHeight + font->shadowy + font->paddingy;
            i--;
            c--;
        }
        else
        {
            if( font->shadowx > 0 || font->shadowx )
            {
                SetTextColor( imgContext, RGB( 1, 1, 1 ) );
                TextOut( imgContext, x+font->shadowx, y+font->shadowy, &c, 1 );
            }
                            
            // TODO: Figure out why RGB is inverted!
            SetTextColor( imgContext, RGB( font->b, font->g, font->r ) );
            TextOut( imgContext, x, y, &c, 1 );                            
            x += glyphWidths[i] + font->shadowx + font->paddingx;
        }
    }

    // NOTE: Fix alpha channel
    uint8_t* pixel = (uint8_t*)canvasData;
    for( int i=0; i<bitmapSize*bitmapSize; i++ )
    {
        uint8_t* r = pixel++;
        uint8_t* g = pixel++;
        uint8_t* b = pixel++;
        uint8_t* a = pixel++;

        if( *r > 0 || *g > 0 || *b > 0 )
        {
            *a = 255;
        }
    }
}

static void CompileFont( bFontInfo* font, HDC imgContext, FILE *imgFile, FILE *infoFile )
{
    HFONT oldFont = (HFONT)SelectObject( imgContext, font->fontHandle );

    TEXTMETRIC textMetrics;
    GetTextMetrics( imgContext, &textMetrics );

    int glyphHeight = textMetrics.tmHeight;

    int glyphWidths[BFONT_RANGE] = {};
    SIZE tempSize;
    for( int i=0; i<BFONT_RANGE; i++ )
    {
        char c = BFONT_RANGE_BEG+i;
        GetTextExtentPoint32( imgContext, &c, 1, &tempSize );
        glyphWidths[i] = tempSize.cx;
    }

    int bitmapSize = 16;

    int glyphsAccounted = 0;
    while( glyphsAccounted < BFONT_RANGE )
    {
        glyphsAccounted = 0;
        bitmapSize *= 2;

        int x = 0, y = glyphHeight + font->shadowy;
        for( int i=0; i<BFONT_RANGE; i++ )
        {
            x += glyphWidths[i] + font->shadowx + font->paddingx;
            if( x >= bitmapSize )
            {
                x = 0;
                y += glyphHeight + font->shadowy + font->paddingy;
                if( y >= bitmapSize )
                    break;

                i--;
            }
            else
                glyphsAccounted++;
        }
    }

    void* canvasData = NULL;

    BITMAPINFO canvasInfo = { { sizeof(BITMAPINFOHEADER),
                                bitmapSize, -bitmapSize, 1, 32,
                                BI_RGB, 0, 0, 0, 0, 0, },
                              { 0, 0, 0, 0 } };

    HBITMAP canvas = CreateDIBSection( imgContext,
                                       &canvasInfo,
                                       DIB_RGB_COLORS,
                                       &canvasData,
                                       NULL, NULL );

    if( canvas && canvasData )
    {
        // clear canvas transparent
        uint32_t* pixel = (uint32_t*)canvasData;
        for( int i=0; i<bitmapSize*bitmapSize; i++ )
            *(pixel++) = 0; // rgba

        HBITMAP oldBitmap = (HBITMAP)SelectObject( imgContext, canvas );
        SetBkMode( imgContext, TRANSPARENT );

        RenderFont( font, imgContext, canvasData,
                    glyphWidths, glyphHeight, bitmapSize );

        if( imgFile )
        {
            bImage image = { canvasData, 0, bitmapSize, bitmapSize };
            bWritePNG( imgFile, &image );
        }
        if( infoFile )
        {
            uint8_t buf[BFONT_RANGE+1] = { glyphHeight };
            for( int i=0; i<BFONT_RANGE; i++ )
                buf[i] = (uint8_t)glyphWidths[i];

            fwrite( buf, 1, BFONT_RANGE+1, infoFile );
        }

        SelectObject( imgContext, oldBitmap );
        DeleteObject( canvas );
    }

    SelectObject( imgContext, oldFont );
}

int main( int argc, char* argv[] )
{
    bFontInfo font = {};
    font.size = 24;
    font.r = font.g = font.b = 255; // default is white text
    font.paddingx = font.paddingy = 1;

    const char* outputName = NULL;

    if( argc > 1 )
    {
        // TODO: Make sure inputName is no longer than 32 TCHARs
        strncpy( font.name, argv[1], 32 );
        font.name[31] = 0;

        // read flags
        for( int i=2; i<argc; i++ )
        {
            if( strcmp( argv[i], "-size" ) == 0 )
            {
                if( argc >= i+1 )
                {
                    font.size = atoi( argv[i+1] );
                    i++;
                }
            }
            else if( strcmp( argv[i], "-weight" ) == 0 )
            {
                if( argc >= i+1 )
                {
                    font.weight = atoi( argv[i+1] );
                    i++;
                }
            }
            else if( strcmp( argv[i], "-rgb" ) == 0 )
            {
                if( argc >= i+3 )
                {
                    font.r = atoi( argv[i+1] );
                    font.g = atoi( argv[i+2] );
                    font.b = atoi( argv[i+3] );
                    i += 3;
                }
            }
            else if( strcmp( argv[i], "-shadow" ) == 0 )
            {
                if( argc >= i+2 )
                {
                    font.shadowx = atoi( argv[i+1] );
                    font.shadowy = atoi( argv[i+2] );
                    i += 2;
                }
            }
            /*else if( strcmp( argv[i], "-mipmap" ) == 0 )
              font.flags |= BFONT_MIPMAP;*/
            else if( strcmp( argv[i], "-mipmap" ) == 0 )
            {
                if( argc >= i+3 )
                {
                    font.mipBeg = atoi( argv[i+1] );
                    font.mipEnd = atoi( argv[i+2] );
                    font.mipAdd = atoi( argv[i+3] );

                    if( font.mipBeg < 1 ) font.mipBeg = 1;
                    if( font.mipAdd < 1 ) font.mipAdd = 1;
                    
                    i += 3;

                    font.flags |= BFONT_MIPMAP;
                }
            }
            else if( strcmp( argv[i], "-italic" ) == 0 )
                font.flags |= BFONT_ITALIC;
            else if( strcmp( argv[i], "-underline" ) == 0 )
                font.flags |= BFONT_UNDERLINE;
            else if( strcmp( argv[i], "-strikeout" ) == 0 )
                font.flags |= BFONT_STRIKEOUT;
            else if( strcmp( argv[i], "-antialias" ) == 0 )
                font.flags |= BFONT_ANTIALIAS;
            else if( strcmp( argv[i], "-pack" ) == 0 )
                font.flags |= BFONT_PACK;
            else if( strcmp( argv[i], "-packall" ) == 0 )
                font.flags |= BFONT_PACK_ALL | BFONT_PACK;
            else if( strcmp( argv[i], "-padding" ) == 0 )
            {
                if( argc >= i+2 )
                {
                    font.paddingx = atoi( argv[i+1] );
                    font.paddingy = atoi( argv[i+2] );
                    i += 2;
                }
            }
            else if( strcmp( argv[i], "-output" ) == 0 )
            {
                if( argc >= i+1 )
                    outputName = argv[i+1];
            }
        }

        std::string outputStr = ( outputName ? outputName : font.name );

        HWND windowHandle = GetConsoleWindow();
        HDC deviceContext = GetDC( windowHandle );
        HDC imgContext = CreateCompatibleDC( deviceContext );

        if( imgContext )
        {
            if( font.flags & BFONT_PACK )
            {
                if( font.flags & BFONT_MIPMAP )
                {
                    if( font.flags & BFONT_PACK_ALL )
                    {
                        FILE *file = fopen( ( outputStr + std::string(".bfont") ).c_str(), "wb" );
                        for( int i=font.mipBeg; i<=font.mipEnd; i+=font.mipAdd )
                        {
                            font.size = i;
                            LoadFont( &font, deviceContext );
                            if( font.fontHandle )
                            {
                                CompileFont( &font, imgContext, file, file );
                            }
                        }

                        fclose( file );
                    }
                    else
                    {
                        std::stringstream ss;

                        for( int i=font.mipBeg; i<=font.mipEnd; i+=font.mipAdd )
                        {
                            font.size = i;
                            LoadFont( &font, deviceContext );
                            if( font.fontHandle )
                            {
                                ss << outputStr << i << ".bfont";
                                FILE *file = fopen( ss.str().c_str(), "wb" );
                                ss.str("");

                                CompileFont( &font, imgContext, file, file );

                                fclose( file );
                            }
                        }
                    }
                }
                else
                {
                    LoadFont( &font, deviceContext );
                    if( font.fontHandle && imgContext )
                    {
                        std::string packName = outputStr + std::string( ".bfont" );
                        FILE *file = fopen( packName.c_str(), "wb" );
                        if( file )
                        {
                            CompileFont( &font, imgContext, file, file );
                            fclose( file );
                        }
                    }
                }
            }
            else
            {
                if( font.flags & BFONT_MIPMAP )
                {
                    std::stringstream ss;
            
                    for( int i=font.mipBeg; i<=font.mipEnd; i += font.mipAdd )
                    {
                        font.size = i;                
                        LoadFont( &font, deviceContext );
                        if( font.fontHandle )
                        {
                            ss << outputStr << i << ".png";
                            FILE *imgFile = fopen( ss.str().c_str(), "wb" );
                            ss.str("");

                            ss << outputStr << i << ".txt";
                            FILE *infoFile = fopen( ss.str().c_str(), "wb" );
                            ss.str("");

                            CompileFont( &font, imgContext, imgFile, infoFile );

                            fclose( imgFile );
                            fclose( infoFile );
                        }
                    }
                }
                else
                {
                    LoadFont( &font, deviceContext );
                    if( font.fontHandle )
                    {
                        FILE *imgFile = fopen( (outputStr + std::string(".png")).c_str(), "wb" );
                        FILE *infoFile = fopen( (outputStr + std::string(".txt")).c_str(), "wb" );

                        CompileFont( &font, imgContext, imgFile, infoFile );

                        fclose( imgFile );
                        fclose( infoFile );
                    }
                }
            }
        }
    }
    
    return 0;
}
