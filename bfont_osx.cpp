#include "bfont.h"

int main( int argc, char* argv[] )
{
    bFontInfo font = {};
    font.size = 24;
    font.r = font.g = font.b = 255; // default is white text
    font.paddingx = font.paddingy = 1;

    const char* outputName = NULL;

    if( argc > 1 )
    {
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
    }
    
    return 0;
}
