#include "comparer.h"

int processError()
{
    switch (outStatus)
    {
        case STATUS_OK:
            printf("Pictures are equal!\n");
            return 0;
            break;

        case STATUS_MISMATCH:
            printf("Pictures are not equal. Different pixels were shown in stderr\n");
            return 0;
            break;

        case ERROR_IO_ERROR:
            printf("IO Error!\n");
            return -1;
            break;

        case ERROR_CORRUPTED_INPUT1:
            printf("First BMP file is corrupted!\n");
            return -1;
            break;

        case ERROR_CORRUPTED_INPUT2:
            printf("Second BMP file is corrupted!\n");
            return -1;
            break;

        case ERROR_BAD_INPUT1:
            printf("Can't open the first BMP file!\n");
            return -1;
            break;

        case ERROR_BAD_INPUT2:
            printf("Can't open the second BMP file!\n");
            return -1;
            break;

        case ERROR_OUT_OF_MEMORY:
            printf("Cannot allocate sufficient memory!\n");
            return -1;
            break;

        case ERROR_WRONG_BPP_INPUT1:
            printf("Wrong битность of a first BMP!\n");
            return -1;
            break;

        case ERROR_WRONG_BPP_INPUT2:
            printf("Wrong битность of a second BMP!\n");
            return -1;
            break;

        case ERROR_INVALID_ARGUMENTS:
            printf("Invalid arguments! Usage: ./comparer [first bmp] [second bmp]\n");
            return -1;
            break;

        case ERROR_INPUT1_NOT_BMP:
            printf("First file is not a BMP!\n");
            return -1;
            break;

        case ERROR_INPUT2_NOT_BMP:
            printf("Second file is not a BMP!\n");
            return -1;
            break;

        case ERROR_BPP_MISMATCH:
            printf("Different битности of files were detected!\n");
            return -1;
            break;

        case ERROR_NOT_EQUAL:
            printf("Files are different in height or width!\n");
            return -1;
            break;
    }
}

int processInput(int argc, char** argv)
{
    if (argc != 3)
    {
        outStatus = ERROR_INVALID_ARGUMENTS;
        return -1;
    }
    if (strcmp(".bmp", strchr(argv[1], '.')) != 0)
    {
        outStatus = ERROR_INPUT1_NOT_BMP;
        return -1;
    }
    if (strcmp(".bmp", strchr(argv[2], '.')) != 0)
    {
        outStatus = ERROR_INPUT2_NOT_BMP;
        return -1;
    }
    return 0;
}

int compareHeaders(BITMAPINFOHEADER firstBMIHeader, BITMAPINFOHEADER secondBMIHeader)
{
    if (firstBMIHeader.BitCount != 32 && firstBMIHeader.BitCount != 24 && firstBMIHeader.BitCount != 8 && firstBMIHeader.BitCount!= 4)
    {
        outStatus = ERROR_WRONG_BPP_INPUT1;
        return -1;
    }
    if (secondBMIHeader.BitCount != 32 && secondBMIHeader.BitCount != 24 && secondBMIHeader.BitCount != 8 && secondBMIHeader.BitCount!= 4)
    {
        outStatus = ERROR_WRONG_BPP_INPUT2;
        return -1;
    }
    if (firstBMIHeader.Width != secondBMIHeader.Width || abs(firstBMIHeader.Height) != abs(secondBMIHeader.Height))
    {
        outStatus = ERROR_NOT_EQUAL;
        return -1;
    }
    if (firstBMIHeader.BitCount != secondBMIHeader.BitCount)
    {
        outStatus = ERROR_BPP_MISMATCH;
        return -1;
    }
    return 0;
}

int readCompareBMP(BITMAPINFOHEADER firstHeader, BITMAPINFOHEADER secondHeader, FILE* firstFile, FILE* secondFile)
{
    RGBQUAD *firstPixels = malloc(sizeof(RGBQUAD) * abs(firstHeader.Height * firstHeader.Width));
    if (!firstPixels)
    {
        outStatus = ERROR_OUT_OF_MEMORY;
        return -1;
    }

    // байты цветов
    for (int i = 0; i < abs(firstHeader.Height * firstHeader.Width); i++)
    {
        firstPixels[i].rgbBlue = getc(firstFile);
        firstPixels[i].rgbGreen = getc(firstFile);
        firstPixels[i].rgbRed = getc(firstFile);
    }

    RGBQUAD *secondPixels = malloc(sizeof(RGBQUAD) * abs(secondHeader.Height * secondHeader.Width));
    if (!secondPixels)
    {
        outStatus = ERROR_OUT_OF_MEMORY;
        free(firstPixels);
        return -1;
    }

    // байты цветов
    for (int i = 0; i < abs(secondHeader.Height * secondHeader.Width); i++)
    {
        secondPixels[i].rgbBlue = getc(secondFile);
        secondPixels[i].rgbGreen = getc(secondFile);
        secondPixels[i].rgbRed = getc(secondFile);
    }

    ///// сравнение начинается тут
    int equal = 1;
    FALSECOORDS *coordList = malloc(sizeof(coordList) * 100);
    int count = 0;
    int size = pow(2, firstHeader.BitCount);
    for (int i = 0; i < abs(firstHeader.Height * firstHeader.Width); i++) // картинки равны в обоих измерениях
    {
        if (equal)
        {
            if (firstPixels[i].rgbBlue != secondPixels[i].rgbBlue && (firstPixels[i].rgbBlue + size != secondPixels[i].rgbBlue && secondPixels[i].rgbBlue + size != firstPixels[i].rgbBlue)
            || firstPixels[i].rgbGreen != secondPixels[i].rgbGreen && (firstPixels[i].rgbGreen + size != secondPixels[i].rgbGreen && secondPixels[i].rgbGreen + size != firstPixels[i].rgbGreen)
            || firstPixels[i].rgbRed != secondPixels[i].rgbRed && (firstPixels[i].rgbRed + size != secondPixels[i].rgbRed && secondPixels[i].rgbRed + size != firstPixels[i].rgbRed))
            {
                equal = 0;
                coordList[count].x = i % firstHeader.Width;
                coordList[count].y = i / firstHeader.Width;
                count++;
            }
        }
        else if (count < 100)
        {
            coordList[count].x = i % firstHeader.Width;
            coordList[count].y = i / firstHeader.Width;
            count++;
        }
    }
    if (!equal)
    {
        for (int i = 0; i < count; i++)
            fprintf(stderr, "%d %d\n", coordList[i].x, coordList[i].y);
        outStatus = STATUS_MISMATCH;
        return 0;
    }
    return 0;
}

int main(int argc, char** argv)
{

    char* firstName;
    char* secondName;

    BITMAPFILEHEADER firstHeader, secondHeader;
    BITMAPINFOHEADER firstBMIHeader, secondBMIHeader;

    if (processInput(argc, argv) == -1)
        return processError();

    firstName = argv[1];
    secondName = argv[2];

    FILE *firstFile = fopen(firstName, "rb");
    if (!firstFile)
    {
        outStatus = ERROR_BAD_INPUT1;
        return processError();
    }

    FILE *secondFile = fopen(secondName, "rb");
    if (!secondFile)
    {
        outStatus = ERROR_BAD_INPUT2;
        fclose(firstFile);
        return processError();
    }

    // заголовок 1 файла
    firstHeader.Type      = read_u16(firstFile);
    if (firstHeader.Type != 19778) // проверка на бмпшность
    {
        outStatus = ERROR_CORRUPTED_INPUT1;
        fclose(firstFile);
        fclose(secondFile);
        return processError();
    }
    firstHeader.Size      = read_u32(firstFile);
    firstHeader.Reserved1 = read_u16(firstFile);
    firstHeader.Reserved2 = read_u16(firstFile);
    firstHeader.Offset    = read_u32(firstFile);

    // заголовок изображения
    firstBMIHeader.Size                 = read_u32(firstFile);
    firstBMIHeader.Width                = read_s32(firstFile);
    firstBMIHeader.Height               = read_s32(firstFile);
    firstBMIHeader.Planes               = read_u16(firstFile);
    firstBMIHeader.BitCount             = read_u16(firstFile);
    firstBMIHeader.CompressionMethod    = read_u32(firstFile);
    firstBMIHeader.ImageSize            = read_u32(firstFile);
    firstBMIHeader.HorizontalResolution = read_s32(firstFile);
    firstBMIHeader.VerticalResolution   = read_s32(firstFile);
    firstBMIHeader.ColorsInPaletteCount = read_u32(firstFile);
    firstBMIHeader.ImportantColorsCount = read_u32(firstFile);

    // заголовок 2 файла
    secondHeader.Type      = read_u16(secondFile);
    if (secondHeader.Type != 19778) // проверка на бмпшность
    {
        outStatus = ERROR_CORRUPTED_INPUT2;
        fclose(firstFile);
        fclose(secondFile);
        return processError();
    }
    secondHeader.Size      = read_u32(secondFile);
    secondHeader.Reserved1 = read_u16(secondFile);
    secondHeader.Reserved2 = read_u16(secondFile);
    secondHeader.Offset    = read_u32(secondFile);

    // заголовок изображения
    secondBMIHeader.Size                 = read_u32(secondFile);
    secondBMIHeader.Width                = read_s32(secondFile);
    secondBMIHeader.Height               = read_s32(secondFile);
    secondBMIHeader.Planes               = read_u16(secondFile);
    secondBMIHeader.BitCount             = read_u16(secondFile);
    secondBMIHeader.CompressionMethod    = read_u32(secondFile);
    secondBMIHeader.ImageSize            = read_u32(secondFile);
    secondBMIHeader.HorizontalResolution = read_s32(secondFile);
    secondBMIHeader.VerticalResolution   = read_s32(secondFile);
    secondBMIHeader.ColorsInPaletteCount = read_u32(secondFile);
    secondBMIHeader.ImportantColorsCount = read_u32(secondFile);

    if (compareHeaders(firstBMIHeader, secondBMIHeader) == -1)
    {
        fclose(firstFile);
        fclose(secondFile);
        return processError();
    }

    if (firstBMIHeader.BitCount == 4 || firstBMIHeader.BitCount == 8)
    {
        unsigned int size = (unsigned int)(pow(2, firstBMIHeader.BitCount) * 4);
        unsigned char *firstPalette = malloc(sizeof(unsigned char) * size);
        if (!firstPalette)
        {
            outStatus = ERROR_OUT_OF_MEMORY;
            return processError();
        }

        if (fread(firstPalette, sizeof(unsigned char), size, firstFile) != size)
        {
            outStatus = ERROR_IO_ERROR;
            free(firstPalette);
            return processError();
        }

        unsigned char *secondPalette = malloc(sizeof(unsigned char) * size);
        if (!secondPalette)
        {
            outStatus = ERROR_OUT_OF_MEMORY;
            return processError();
        }

        if (fread(secondPalette, sizeof(unsigned char), size, secondFile) != size)
        {
            outStatus = ERROR_IO_ERROR;
            free(firstPalette);
            free(secondPalette);
            return processError();
        }
        free(firstPalette);
        free(secondPalette);
    }

    readCompareBMP(firstBMIHeader, secondBMIHeader, firstFile, secondFile);
    fclose(firstFile);
    fclose(secondFile);
    return processError();
}

static unsigned short read_u16(FILE *fp)
{
    unsigned char b0, b1;

    b0 = getc(fp);
    b1 = getc(fp);

    return ((b1 << 8) | b0);
}


static unsigned int read_u32(FILE *fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}

static int read_s32(FILE *fp)
{
    unsigned char b0, b1, b2, b3;

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}

static void write_u16(unsigned short input, FILE *fp)
{
    putc(input, fp);
    putc(input >> 8, fp);
}

static void write_u32(unsigned int input, FILE *fp)
{
    putc(input, fp);
    putc(input >> 8, fp);
    putc(input >> 16, fp);
    putc(input >> 24, fp);
}

static void write_s32(int input, FILE *fp)
{
    putc(input, fp);
    putc(input >> 8, fp);
    putc(input >> 16, fp);
    putc(input >> 24, fp);
}