//
// Created by mac on 09.11.2020.
//

#ifndef INC_4_SMMAC72_COMPARER_H
#define INC_4_SMMAC72_COMPARER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

typedef struct
{
    int x;
    int y;
} FALSECOORDS;
typedef enum
{
    ERROR_INVALID_ARGUMENTS,
    ERROR_INPUT1_NOT_BMP,
    ERROR_INPUT2_NOT_BMP,
    ERROR_WRONG_BPP_INPUT1,
    ERROR_WRONG_BPP_INPUT2,
    ERROR_BPP_MISMATCH,
    ERROR_BAD_INPUT1,
    ERROR_BAD_INPUT2,
    ERROR_NOT_EQUAL,
    ERROR_OUT_OF_MEMORY,
    ERROR_CORRUPTED_INPUT1,
    ERROR_CORRUPTED_INPUT2,
    ERROR_IO_ERROR,
    STATUS_OK,
    STATUS_MISMATCH
} ERROR;

ERROR outStatus = STATUS_OK;

static unsigned short read_u16(FILE *fp);
static unsigned int   read_u32(FILE *fp);
static int            read_s32(FILE *fp);

static void write_u16(unsigned short input, FILE *fp);
static void write_u32(unsigned int input, FILE *fp);
static void write_s32(int input, FILE *fp);

#endif //INC_4_SMMAC72_COMPARER_H
