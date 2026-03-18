#include "Brightness.h"
#include "Image.h"

Brightness::Brightness(int v)
{
    value = v;
}

/**
 * @brief Apply brightness adjustment to an image.
 *
 * This function iterates over every pixel and every channel in the image,
 * adding a constant offset (value) to each pixel intensity.
 *
 * To ensure valid pixel values, the result is clamped to the range [0, 255],
 * which is the standard range for 8-bit image channels.
 *
 * The operation is performed in-place, meaning the input image is modified directly.
 *
 * @param image Reference to the Image object to be modified.
 *
 * Time Complexity: O(width × height × channels)
 */
void Brightness::apply(Image& image)
{
    // Retrieve image properties
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    // Loop over all pixels
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Loop over all colour channels (e.g., RGB)
            for (int c = 0; c < channels; c++)
            {
                // Get current pixel value
                int pixel = image.getPixel(x, y, c);

                // Apply brightness adjustment
                pixel = pixel + value;

                // Clamp result to valid 8-bit range [0, 255]
                if (pixel > 255)
                    pixel = 255;
                if (pixel < 0)
                    pixel = 0;

                // Write adjusted value back to the image
                image.setPixel(x, y, c, pixel);
            }
        }
    }
}