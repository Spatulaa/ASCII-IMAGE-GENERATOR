#include <iostream>
#include <math.h>

// stb libraries
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image_resize.h"
#include "stb_image_write.h"
#include "stb_image.h"

int main(int arg_count, char *arguments[])
{
    // Error Precautions
    if (arg_count < 3 || arg_count > 3)
    {
        std::cout << "[ERROR]: arguments were not in correct format\n";
        std::cout << "[EXAMPLE_FORMAT]: ./main images/example.png output.png size(int)\n";
        exit(1);
    }

    const int expected_channels = 4;
    int width, height, color_channels; // Loading image
    unsigned char *image = stbi_load(arguments[1], &width, &height, &color_channels, expected_channels);
    if (image == NULL)
    {
        std::cout << "[ERROR]: " << arguments[1] << " is not a valid directory\n";
        exit(2);
    }

    // ---------------------------------------- ASCII ART GENERATOR ----------------------------------------

    // Grayscaling the Image
    size_t image_size = width * height * expected_channels;

    const int grey_channels = 2;
    size_t grey_image_size = width * height * grey_channels; // Grey & Alpha Channel

    // Allocated memory for new image
    unsigned char *grey_image = new unsigned char[grey_image_size];
    if (grey_image == NULL)
    {
        std::cout << "[ERROR]: insufficient amount of memory to allocate\n";
        exit(3);
    }

    // Loop thru pixels of each image
    for (unsigned char *pixel = image, *grey_pixel = grey_image; pixel != image + image_size; pixel += expected_channels, grey_pixel += grey_channels)
    {
        // Averaging pixels
        *grey_pixel = (uint8_t) round((*pixel + *(pixel + 1) + *(pixel + 2)) / 3);
        *(grey_pixel + 1) = *(pixel + 3); // Alpha channel
    }
    stbi_image_free(image);

    //Resizing Image
    int final_height = height;
    int final_width = width;
    while (final_height > 100 && final_width > 100)
    {
        final_height /= 2;
        final_width /=2;
    }

    size_t final_image_size = final_width * final_height * grey_channels;
    unsigned char *final_image = new unsigned char[final_image_size];
    if (final_image == NULL)
    {
        std::cout << "[ERROR]: insufficient amount of memory to allocate\n";
        exit(3);
    }

    stbir_resize_uint8(grey_image, width, height, 0, final_image, final_width, final_height, 0, grey_channels);
    stbi_write_png(arguments[2], final_width, final_height, grey_channels, final_image, final_width * grey_channels);
    stbi_image_free(grey_image);

    // Image to ASCII Conversion
    int iteration = 0;
    const char *brightness = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'..";
    for (unsigned char *pixel = final_image; pixel != final_image + final_image_size; pixel += grey_channels)
    {
        // Mapping the image values with the brightness string
        iteration++;
        int index = round((float) *pixel / 3.64285714286 );
        index = 71 - index;
        if (index > 69) index = 69;
        if (*(pixel + 1) == 0) std::cout << ' ';
        else std::cout << brightness[index];

        // Width Reset
        if (iteration >= final_width) 
        {
            std::cout << '\n';
            iteration = 0;
        }
    }
    stbi_image_free(final_image);
}