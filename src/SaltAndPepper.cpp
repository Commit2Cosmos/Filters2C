#include "SaltAndPepper.h"
#include <random>
#include <cmath>

/**
 * @brief Construct a Salt-and-Pepper noise filter.
 *
 * @param noiseRatio Proportion of pixels to be corrupted by noise (range [0,1]).
 *                   A value of 0 means no noise, while 1 means all pixels are affected.
 */
SaltAndPepper::SaltAndPepper(double noiseRatio)
    : noiseRatio(noiseRatio)
{
}

/**
 * @brief Apply salt-and-pepper noise to an image.
 *
 * This function randomly selects a subset of pixels and replaces their values
 * with either black (0, "pepper") or white (255, "salt"), simulating impulsive noise.
 *
 * Steps:
 * 1. Compute the number of pixels to corrupt based on the noise ratio.
 * 2. Randomly select pixel positions.
 * 3. Assign each selected pixel either 0 or 255 with equal probability.
 *
 * Two cases are handled:
 *
 * - Grayscale images:
 *     Noise is applied directly to the single intensity channel.
 *
 * - RGB images:
 *     The same noise value is applied to all colour channels to preserve
 *     consistency (i.e., avoid introducing coloured noise).
 *
 * The operation is performed in-place.
 *
 * @param image Reference to the Image object to be modified.
 *
 * Time Complexity: O(noisyPixels)
 */
void SaltAndPepper::apply(Image& image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    // Validate image dimensions
    if (width <= 0 || height <= 0 || channels < 1)
        return;

    // Clamp noise ratio to valid range [0,1]
    if (noiseRatio < 0.0)
        noiseRatio = 0.0;
    if (noiseRatio > 1.0)
        noiseRatio = 1.0;

    // Compute number of pixels to corrupt
    int totalPixels = width * height;
    int noisyPixels = static_cast<int>(std::round(totalPixels * noiseRatio));

    // Random number generators
    std::random_device rd;
    std::mt19937 gen(rd());
    // Uniform distributions for pixel coordinates
    std::uniform_int_distribution<int> xDist(0, width - 1);
    std::uniform_int_distribution<int> yDist(0, height - 1);
    // Binary distribution: 0 = pepper (black), 1 = salt (white)
    std::uniform_int_distribution<int> valueDist(0, 1); // 0 = pepper, 1 = salt

    // Apply noise
    for (int i = 0; i < noisyPixels; i++)
    {
        // Random pixel location
        int x = xDist(gen);
        int y = yDist(gen);
        // Randomly choose salt (255) or pepper (0)
        unsigned char noiseValue = (valueDist(gen) == 0) ? 0 : 255;

        // Grayscale image
        if (channels == 1)
        {
            image.setPixel(x, y, 0, noiseValue);
        }
        // RGB or RGBA image
        else if (channels >= 3)
        {
            // Apply same noise to all colour channels
            // This avoids introducing coloured artefacts
            image.setPixel(x, y, 0, noiseValue);
            image.setPixel(x, y, 1, noiseValue);
            image.setPixel(x, y, 2, noiseValue);
            // Leave alpha unchanged if channels == 4
        }
    }
}