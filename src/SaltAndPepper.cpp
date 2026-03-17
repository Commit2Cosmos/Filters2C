#include "SaltAndPepper.h"
#include <random>
#include <cmath>

SaltAndPepper::SaltAndPepper(double noiseRatio)
    : noiseRatio(noiseRatio)
{
}

void SaltAndPepper::apply(Image& image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    if (width <= 0 || height <= 0 || channels < 1)
        return;

    // Clamp noise ratio to [0, 1]
    if (noiseRatio < 0.0)
        noiseRatio = 0.0;
    if (noiseRatio > 1.0)
        noiseRatio = 1.0;

    int totalPixels = width * height;
    int noisyPixels = static_cast<int>(std::round(totalPixels * noiseRatio));

    // Random number generators
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> xDist(0, width - 1);
    std::uniform_int_distribution<int> yDist(0, height - 1);
    std::uniform_int_distribution<int> valueDist(0, 1); // 0 = pepper, 1 = salt

    for (int i = 0; i < noisyPixels; i++)
    {
        int x = xDist(gen);
        int y = yDist(gen);
        unsigned char noiseValue = (valueDist(gen) == 0) ? 0 : 255;

        // Grayscale image
        if (channels == 1)
        {
            image.setPixel(x, y, 0, noiseValue);
        }
        // RGB or RGBA image
        else if (channels >= 3)
        {
            image.setPixel(x, y, 0, noiseValue);
            image.setPixel(x, y, 1, noiseValue);
            image.setPixel(x, y, 2, noiseValue);
            // Leave alpha unchanged if channels == 4
        }
    }
}