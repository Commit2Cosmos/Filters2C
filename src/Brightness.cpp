#include "Brightness.h"
#include "Image.h"

Brightness::Brightness(int v)
{
    value = v;
}

void Brightness::apply(Image& image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (int c = 0; c < channels; c++)
            {
                int pixel = image.getPixel(x, y, c);

                pixel = pixel + value;

                // clamp to [0,255]
                if (pixel > 255)
                    pixel = 255;
                if (pixel < 0)
                    pixel = 0;

                image.setPixel(x, y, c, pixel);
            }
        }
    }
}