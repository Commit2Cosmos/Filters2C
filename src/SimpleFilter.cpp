#include "SimpleFilter.h"

void SimpleFilter::apply(Image& image)
{
    int width = image.width;
    int height = image.height;
    int channels = image.channels;

    // Only process RGB images
    if (channels < 3) return;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            unsigned char r = image.getPixel(x, y, 0);
            unsigned char g = image.getPixel(x, y, 1);
            unsigned char b = image.getPixel(x, y, 2);

            unsigned char gray = static_cast<unsigned char>(
                0.2126 * r +
                0.7152 * g +
                0.0722 * b
            );

            image.setPixel(x, y, 0, gray);
            image.setPixel(x, y, 1, gray);
            image.setPixel(x, y, 2, gray);
        }
    }
}