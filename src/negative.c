/**************************************************************

	The program reads an BMP image file and creates a new
	image that is the negative of the input file.

**************************************************************/

#include "qdbmp.h"
#include <stdio.h>

/* Creates a negative image of the input bitmap file */
int negativeCustom(char* inputFile, char* outputFile)
{
	UCHAR	r, g, b;
	UINT	width, height;
	UINT	x, y;
	BMP*	bmp;

    /* НЕ ИСПОЛЬЗУЕТСЯ В ДАННОЙ ЗАДАЧЕ, МЕТОД ВВОДА ИНОЙ
	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: %s <input file> <output file>\n", argv[ 0 ] );
		return 0;
	}

	/* Read an image file */

	bmp = BMP_ReadFile(inputFile);
	BMP_CHECK_ERROR( stdout, -1 );

	if (BMP_GetDepth(bmp) == 8)
	{
        for (int i = 0; i <= 256; i++)
        {
            BMP_GetPaletteColor(bmp, i, &r, &g, &b);
            r = ~r & 0xFF;
            g = ~g & 0xFF;
            b = ~b & 0xFF;
            BMP_SetPaletteColor(bmp, i, r, g, b);
        }
    }
	else if (BMP_GetDepth(bmp) == 24)
    {
	    /* Get image's dimensions */
        width = BMP_GetWidth(bmp);
        height = BMP_GetHeight(bmp);

        /* Iterate through all the image's pixels */
        for (x = 0; x < width; ++x)
        {
            for (y = 0; y < height; ++y)
            {
                /* Get pixel's RGB values */
                BMP_GetPixelRGB(bmp, x, y, &r, &g, &b);

                /* Invert RGB values */
                BMP_SetPixelRGB(bmp, x, y, 255 - r, 255 - g, 255 - b);
            }
        }
    }
	/* Save result */
	BMP_WriteFile( bmp, outputFile);
	BMP_CHECK_ERROR( stdout, -2 );


	/* Free all memory allocated for the image */
	BMP_Free( bmp );
	return 0;
}

