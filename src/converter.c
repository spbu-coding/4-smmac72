// TODO: поддержка отрицательной высоты во внешней библиотеке. пока что [WIP]

#include "converter.h"

int processError()
{
    switch (outStatus)
    {
        case STATUS_OK:
            printf("Success!\n");
            return 0;
            break;

        case ERROR_EXTERNAL:
            printf("External Error happened! Oops!\n");
            return 3;
            break;

        case ERROR_IO_ERROR:
            printf("IO Error!\n");
            return -1;
            break;

        case ERROR_CORRUPTED_BMP:
            printf("Input BMP file is corrupted!\n");
            return -2;
            break;

        case ERROR_BAD_OUTPUT:
            printf("Output file creation error!\n");
            return -1;
            break;

        case ERROR_BAD_INPUT:
            printf("Input file open error!\n");
            return -1;
            break;

        case ERROR_OUT_OF_MEMORY:
            printf("Cannot allocate sufficient memory!\n");
            return -1;
            break;

        case ERROR_WRONG_BPP:
            printf("Wrong битность of a BMP!\n");
            return -2;
            break;

        case ERROR_INVALID_ARGUMENTS:
            printf("Invalid arguments! Usage: ./converter [TYPE (--mine or --theirs)] [bmp input] [bmp output]\n");
            return -1;
            break;

        case ERROR_INPUT_NOT_BMP:
            printf("Input file is not a BMP!\n");
            return -1;
            break;

        case ERROR_OUTPUT_NOT_BMP:
            printf("Output file is not a BMP!\n");
            return -1;
            break;
    }
}

int mine8bit(BITMAPFILEHEADER header, BITMAPINFOHEADER bmiHeader, FILE *pFile, char* outputName)
{
    unsigned char *Palette = malloc(sizeof(unsigned char) * 1024);
    if (!Palette)
    {
        outStatus = ERROR_OUT_OF_MEMORY;
        return -1;
    }

    if (fread(Palette, sizeof(unsigned char), 1024, pFile) != 1024)
    {
        outStatus = ERROR_IO_ERROR;
        free(Palette);
        return -1;
    }

    // массив пикселей
    RGBQUAD *pixels = malloc(sizeof(RGBQUAD) * abs(bmiHeader.Height * bmiHeader.Width));
    if (!pixels)
    {
        outStatus = ERROR_OUT_OF_MEMORY;
        free(Palette);
        return -1;
    }

    // байты цветов
    for (int i = 0; i < abs(bmiHeader.Height * bmiHeader.Width); i++)
    {
        pixels[i].rgbBlue = getc(pFile);
        pixels[i].rgbGreen = getc(pFile);
        pixels[i].rgbRed = getc(pFile);
    }

    FILE *oFile = fopen(outputName, "wb");
    if (!oFile)
    {
        outStatus = ERROR_BAD_OUTPUT;
        free(Palette);
        free(pixels);
        return -1;
    }

    write_u16(header.Type, oFile);
    write_u32(header.Size, oFile);
    write_u16(header.Reserved1, oFile);
    write_u16(header.Reserved2, oFile);
    write_u32(header.Offset, oFile);

    write_u32(bmiHeader.Size, oFile);
    write_s32(bmiHeader.Width, oFile);
    write_s32(bmiHeader.Height, oFile);
    write_u16(bmiHeader.Planes, oFile);
    write_u16(bmiHeader.BitCount, oFile);
    write_u32(bmiHeader.CompressionMethod, oFile);
    write_u32(bmiHeader.ImageSize, oFile);
    write_s32(bmiHeader.HorizontalResolution, oFile);
    write_s32(bmiHeader.VerticalResolution, oFile);
    write_u32(bmiHeader.ColorsInPaletteCount, oFile);
    write_u32(bmiHeader.ImportantColorsCount, oFile);

    // инверсия паллетки цветов
    for (int i = 0; i < 1024; i+=4)
    {
        Palette[i] = ~Palette[i];
        Palette[i+1] = ~Palette[i+1];
        Palette[i+2] = ~Palette[i+2];
        if (Palette[i+3] != 0x00)
        {
            outStatus = ERROR_CORRUPTED_BMP;
            free(Palette);
            free(pixels);
            fclose(oFile);
            return -1;
        }
    }
    if (fwrite(Palette, sizeof(unsigned char), 1024, oFile) != 1024)
    {
        outStatus = ERROR_IO_ERROR;
        free(Palette);
        free(pixels);
        fclose(oFile);
        return -1;
    }

    // запись изначальных пикселей
    for (int i = 0; i < abs(bmiHeader.Height * bmiHeader.Width); i++)
    {
        putc(pixels[i].rgbBlue & 0xFF, oFile);
        putc(pixels[i].rgbGreen & 0xFF, oFile);
        putc(pixels[i].rgbRed & 0xFF, oFile);
    }

    free(Palette);
    free(pixels);
    fclose(oFile);
    return 0;
}

int mine24bit(BITMAPFILEHEADER header, BITMAPINFOHEADER bmiHeader, FILE *pFile, char* outputName)
{
    // массив пикселей
    RGBQUAD *pixels = malloc(sizeof(RGBQUAD) * abs(bmiHeader.Height * bmiHeader.Width));
    if (!pixels)
    {
        outStatus = ERROR_OUT_OF_MEMORY;
        return -1;
    }

    // байты цветов
    for (int i = 0; i < abs(bmiHeader.Height * bmiHeader.Width); i++)
    {
        pixels[i].rgbBlue = getc(pFile);
        pixels[i].rgbGreen = getc(pFile);
        pixels[i].rgbRed = getc(pFile);
    }

    FILE *oFile = fopen(outputName, "wb");
    if (!oFile)
    {
        outStatus = ERROR_BAD_OUTPUT;
        free(pixels);
        return -1;
    }

    write_u16(header.Type, oFile);
    write_u32(header.Size, oFile);
    write_u16(header.Reserved1, oFile);
    write_u16(header.Reserved2, oFile);
    write_u32(header.Offset, oFile);

    write_u32(bmiHeader.Size, oFile);
    write_s32(bmiHeader.Width, oFile);
    write_s32(bmiHeader.Height, oFile);
    write_u16(bmiHeader.Planes, oFile);
    write_u16(bmiHeader.BitCount, oFile);
    write_u32(bmiHeader.CompressionMethod, oFile);
    write_u32(bmiHeader.ImageSize, oFile);
    write_s32(bmiHeader.HorizontalResolution, oFile);
    write_s32(bmiHeader.VerticalResolution, oFile);
    write_u32(bmiHeader.ColorsInPaletteCount, oFile);
    write_u32(bmiHeader.ImportantColorsCount, oFile);

    // запись инвертированных пикселей
    for (int i = 0; i < abs(bmiHeader.Height * bmiHeader.Width); i++)
    {
        putc(~pixels[i].rgbBlue & 0xFF, oFile);
        putc(~pixels[i].rgbGreen & 0xFF, oFile);
        putc(~pixels[i].rgbRed & 0xFF, oFile);
    }
    free(pixels);
    fclose(oFile);
    return 0;
}

int processInput(int argc, char** argv)
{
    if (argc != 4)
    {
        outStatus = ERROR_INVALID_ARGUMENTS;
        return -1;
    }
    if (strcmp(".bmp", strchr(argv[2], '.')) != 0)
    {
        outStatus = ERROR_INPUT_NOT_BMP;
        return -1;
    }
    if (strcmp(".bmp", strchr(argv[3], '.')) != 0)
    {
        outStatus = ERROR_OUTPUT_NOT_BMP;
        return -1;
    }

    if (strcmp(argv[1], "--mine") == 0)
    {
        return 0;
    }
    else if (strcmp(argv[1], "--theirs") == 0)
    {
        return 1;
    }
    else
    {
        outStatus = ERROR_INVALID_ARGUMENTS;
        return -1;
    }
}

int mineProcess(BITMAPFILEHEADER header, BITMAPINFOHEADER bmiHeader, FILE *pFile, char* outputName)
{
    if (bmiHeader.BitCount == 8)
    {
        if (mine8bit(header, bmiHeader, pFile, outputName) == -1)
            return -1;
    }
    else if (bmiHeader.BitCount == 24)
    {
        if (mine24bit(header, bmiHeader, pFile, outputName) == -1)
            return -1;
    }

    return 0;
}

int main(int argc, char** argv)
{
    int method = 0; // 0 - свой, 1 - плагиат

    char* inputName;
    char* outputName;

    BITMAPFILEHEADER header;
    BITMAPINFOHEADER bmiHeader;

    method = processInput(argc, argv);
    if (method == -1)
        return processError();
    inputName = argv[2];
    outputName = argv[3];

    FILE *pFile = fopen(inputName, "rb");
    if (!pFile)
    {
        outStatus = ERROR_BAD_INPUT;
        return processError();
    }



    // заголовок файла
    header.Type      = read_u16(pFile);
    if (header.Type != 19778) // проверка на бмпшность
    {
        outStatus = ERROR_CORRUPTED_BMP;
        return -1;
    }
    header.Size      = read_u32(pFile);
    header.Reserved1 = read_u16(pFile);
    header.Reserved2 = read_u16(pFile);
    header.Offset    = read_u32(pFile);

    // заголовок изображения
    bmiHeader.Size                 = read_u32(pFile);
    bmiHeader.Width                = read_s32(pFile);
    bmiHeader.Height               = read_s32(pFile);
    bmiHeader.Planes               = read_u16(pFile);
    bmiHeader.BitCount             = read_u16(pFile);
    bmiHeader.CompressionMethod    = read_u32(pFile);
    bmiHeader.ImageSize            = read_u32(pFile);
    bmiHeader.HorizontalResolution = read_s32(pFile);
    bmiHeader.VerticalResolution   = read_s32(pFile);
    bmiHeader.ColorsInPaletteCount = read_u32(pFile);
    bmiHeader.ImportantColorsCount = read_u32(pFile);

    if (bmiHeader.BitCount != 8 && bmiHeader.BitCount != 24)
    {
        outStatus = ERROR_WRONG_BPP;
        fclose(pFile);
        return processError();
    }



    switch (method)
    {
        case 0:
            if (mineProcess(header, bmiHeader, pFile, outputName) == -1) {
                fclose(pFile);
                return processError();
            }
            break;

        case 1:
            negativeCustom(inputName, outputName);
            if (BMP_GetError() != BMP_OK)
            {
                outStatus = ERROR_EXTERNAL;
                fclose(pFile);
                return processError();
            }
            break;
    }

    fclose(pFile);
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