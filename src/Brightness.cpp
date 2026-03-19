#include "Brightness.h"
#include "Image.h"
#include <stdexcept>
#include <string>

Brightness::Brightness(int v)
    : value(v), autoMode(false)
{
}

Brightness::Brightness(const std::string& mode)
    : value(0), autoMode(false)
{
    if (mode == "auto")
    {
        autoMode = true;
    }
    else
    {
        value = std::stoi(mode);
    }
}

/**
 * @brief Apply brightness adjustment to an image.
 *
 * If constructed with a numeric value, this value is added to each pixel.
 * If constructed with "auto", a simple automatic adjustment is computed
 * from the average image intensity so that darker images are brightened
 * and brighter images are darkened slightly toward a mid-level target.
 *
 * The operation is performed in-place.
 *
 * @param image Reference to the Image object to be modified.
 */
void Brightness::apply(Image& image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    int adjustment = value;

    // Auto brightness mode:
    // compute average intensity and shift it toward a target value.
    if (autoMode)
    {
        long long sum = 0;
        int count = width * height * channels;

        if (count == 0)
            return;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                for (int c = 0; c < channels; c++)
                {
                    sum += image.getPixelSimple(x, y, c);
                }
            }
        }

        int average = static_cast<int>(sum / count);

        // Target a mid-level brightness
        const int target = 128;
        adjustment = target - average;
    }

    // Apply brightness adjustment
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (int c = 0; c < channels; c++)
            {
                int pixel = image.getPixelSimple(x, y, c);
                pixel += adjustment;

                if (pixel > 255)
                    pixel = 255;
                if (pixel < 0)
                    pixel = 0;

                image.setPixel(x, y, c, static_cast<unsigned char>(pixel));
            }
        }
    }
}