#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "negative.h"
#include "qdbmp.h"

typedef struct
{
    unsigned int    Type;
    unsigned long   Size;
    unsigned int    Reserved1;
    unsigned int    Reserved2;
    unsigned long   Offset;
} BITMAPFILEHEADER;

typedef struct
{
    unsigned int    Size;
    int             Width;
    int             Height;
    unsigned short  Planes;
    unsigned short  BitCount;
    unsigned int    CompressionMethod;
    unsigned int    ImageSize;
    int             HorizontalResolution;
    int             VerticalResolution;
    unsigned int    ColorsInPaletteCount;
    unsigned int    ImportantColorsCount;
} BITMAPINFOHEADER;

typedef struct
{
    int   rgbBlue;
    int   rgbGreen;
    int   rgbRed;
    int   rgbReserved;
} RGBQUAD;

typedef enum
{
    ERROR_INVALID_ARGUMENTS,
    ERROR_INPUT_NOT_BMP,
    ERROR_OUTPUT_NOT_BMP,
    ERROR_WRONG_BPP,
    ERROR_BAD_INPUT,
    ERROR_BAD_OUTPUT,
    ERROR_OUT_OF_MEMORY,
    ERROR_CORRUPTED_BMP,
    ERROR_IO_ERROR,
    ERROR_EXTERNAL,
    STATUS_OK
} ERROR;

ERROR outStatus = STATUS_OK;


static unsigned short read_u16(FILE *fp);
static unsigned int   read_u32(FILE *fp);
static int            read_s32(FILE *fp);

static void write_u16(unsigned short input, FILE *fp);
static void write_u32(unsigned int input, FILE *fp);
static void write_s32(int input, FILE *fp);
#endif