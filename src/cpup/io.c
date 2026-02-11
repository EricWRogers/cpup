#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL_iostream.h>

#include "vec.h"
#include "opengl.h"

Image LoadImage(const char* _path)
{
    Image image;

    SDL_IOStream* file = SDL_IOFromFile(_path, "r");

    if (file)
    {
        printf("File found! %p\n", file);

        u8 idLength = 0;
        u8 colorMap = 0;
        u8 imageType = 0;

        // color map specification
        u16 entryIndex = 0;
        u16 mapLength = 0;
        u8 mapEntrySize = 0;

        // image specification
        u16 xOrigin = 0;
        u16 yOrigin = 0;
        u16 width = 0;
        u16 height = 0;
        u8  pixelDepth = 0; // bits per pixel
        u8  imageDescriptor = 0;
        // image descriptor:
        // bit 3-0 give the alpha channel depth
        // bit 4 of the image descriptor byte indicates right-to-left pixel ordering if set.
        // bit 5 indicates an ordering of top-to-bottom. Otherwise, pixels are stored in bottom-to-top, left-to-right order.
        // bit 7-6 of the image descriptor byte indicate the data storage interleaving mode.

        // todo read the return value to check for error
        SDL_ReadIO(file, &idLength, sizeof(u8));
        SDL_ReadIO(file, &colorMap, sizeof(u8));
        SDL_ReadIO(file, &imageType, sizeof(u8));
        SDL_ReadIO(file, &entryIndex, sizeof(u16));
        SDL_ReadIO(file, &mapLength, sizeof(u16));
        SDL_ReadIO(file, &mapEntrySize, sizeof(u8));
        SDL_ReadIO(file, &xOrigin, sizeof(u16));
        SDL_ReadIO(file, &yOrigin, sizeof(u16));
        SDL_ReadIO(file, &width, sizeof(u16));
        SDL_ReadIO(file, &height, sizeof(u16));
        SDL_ReadIO(file, &pixelDepth, sizeof(u8));
        SDL_ReadIO(file, &imageDescriptor, sizeof(u8));
        

        printf("idLength %x\n", idLength);
        printf("colorMap %x\n", colorMap);
        printf("imageType %x\n", imageType);
        printf("entryIndex %hu\n", entryIndex);
        printf("mapLength %hu\n", mapLength);
        printf("mapEntrySize %x\n", mapEntrySize);
        printf("xOrigin %hu\n", xOrigin);
        printf("yOrigin %hu\n", yOrigin);
        printf("width %hu\n", width);
        printf("height %hu\n", height);
        //printf("pixelDepth %x\n", pixelDepth);
        printf("pixelDepth %u (0x%02X)\n", (unsigned)pixelDepth, (unsigned)pixelDepth);

        if (imageDescriptor & BIT(0))
            printf("imageDescriptor bit 0\n");
        if (imageDescriptor & BIT(1))
            printf("imageDescriptor bit 1\n");
        if (imageDescriptor & BIT(2))
            printf("imageDescriptor bit 2\n");
        if (imageDescriptor & BIT(3))
            printf("imageDescriptor bit 3\n");
        if (imageDescriptor & BIT(4))
            printf("imageDescriptor bit 4\n");
        if (imageDescriptor & BIT(5))
            printf("imageDescriptor bit 5\n");
        if (imageDescriptor & BIT(6))
            printf("imageDescriptor bit 6\n");
        if (imageDescriptor & BIT(7))
            printf("imageDescriptor bit 7\n");
        
        u32 bytesPerPixel = pixelDepth / 8;
        size_t srcSize = (size_t)width * (size_t)height * bytesPerPixel;

        u8* srcBGRA = (u8*)malloc(srcSize);
        SDL_ReadIO(file, srcBGRA, srcSize); // data is bgra

        bool originTop = (imageDescriptor & BIT(5));

        u8* p = NULL;

        u8 b = 0;
        u8 g = 0;
        u8 r = 0;
        u8 a = 0;

        const size_t dstSize = (size_t)width * (size_t)height;
        for (u32 i = 0; i < dstSize; i++)
        {
            p = srcBGRA + i * 4;

            b = p[0];
            g = p[1];
            r = p[2];
            a = p[3];

            p[0] = r;
            p[1] = g;
            p[2] = b;
            p[3] = a;
        }

        image.width = width;
        image.height = height;
        image.data = (u32*)srcBGRA;


        SDL_CloseIO(file);
    }

    glGenTextures(1, &image.id);
    glBindTexture(GL_TEXTURE_2D, image.id); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return image;
}

char* LoadFile(const char* _path)
{
    size_t fileSize;
    char* data = SDL_LoadFile(_path, &fileSize);

    if (data == NULL)
    {
        printf("Error: loading file %s!", _path);
    }

    return data;
}

void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int get_int()
{
    int x;
    int result;

    result = scanf("%d", &x);

    if (result == EOF)
    {
        exit(1);
    }
    else if (result == 0)
    {
        printf("Invalid input. Please enter an integer: ");
        clear_input_buffer();
        x = get_int(); // Try again recursively
    }
    else
    {
        clear_input_buffer();
    }

    return x;
}

char get_char()
{
    char c;

    int result = scanf("%c", &c);

    if (result == EOF)
    {
        exit(1);
    }
    else if (result == 0)
    {
        printf("Invalid input. Please enter an char: ");
        clear_input_buffer();
        c = get_char();
    }
    else
    {
        clear_input_buffer();
    }

    return c;
}

char *get_string()
{
    char buffer[1024]; // Adjust the buffer size as needed
    char *input = NULL;
    int length;

    if (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        length = strlen(buffer);

        // Remove the newline character if present
        if (length > 0 && buffer[length - 1] == '\n')
        {
            buffer[length - 1] = '\0';
        }

        // Allocate memory for the input string and copy the buffer
        input = (char *)malloc(length);
        if (input != NULL)
        {
            strcpy(input, buffer);
        }

        return input;
    }

    return input;
}

float get_float()
{
    float x;
    int result;

    result = scanf("%f", &x);

    if (result == EOF)
    {
        exit(1);
    }
    if (result == 0)
    {
        printf("Invalid input. Please enter a float: ");
        clear_input_buffer();
        x = get_float(); // Try again recursively
    }
    else
    {
        clear_input_buffer();
    }

    return x;
}

int request_int(const char *_message)
{
    printf("%s : ", _message);
    return get_int();
}

float request_float(const char *_message)
{
    printf("%s : ", _message);
    return get_float();
}

char request_char(const char *_message)
{
    printf("%s : ", _message);
    return get_char();
}

char *request_string(const char *_message)
{
    printf("%s : ", _message);
    return get_string();
}

int random_int(int _min, int _max)
{
    if (_max < _min)
        return _min;
    
    if (_min == _max)
        return _min;
    
    int range = 1 + _max - _min;

    return _min + rand() % range;
}

float random_float(float _min, float _max)
{
    if (_max > _min)
    {
        float random = ((float)rand()) / (float)RAND_MAX;
        float range = _max - _min;
        return (random * range) + _min;
    }

    if (_min == _max)
        return _min;

    return random_float(_max, _min);
}
