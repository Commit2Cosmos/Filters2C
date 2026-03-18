#include "HistogramEqualisation.h"
#include <cmath>
#include <vector>

/**
 * @brief Build a lookup table (LUT) for histogram equalisation.
 *
 * This function computes a mapping from original pixel intensities
 * to equalised intensities using the cumulative distribution function (CDF).
 *
 * Steps:
 * 1. Compute histogram of pixel values.
 * 2. Compute cumulative distribution function (CDF).
 * 3. Normalise CDF to span full intensity range [0,255].
 * 4. Store mapping in a lookup table (LUT).
 *
 * Special case:
 * If all pixels have the same value, the LUT becomes identity
 * to avoid division by zero and preserve the image.
 *
 * @param values Input pixel intensity values (0–255).
 * @return LUT mapping original intensities to equalised ones.
 */
std::vector<unsigned char> HistogramEqualisation::buildEqualisationLUT(const std::vector<unsigned char>& values) const
{
    std::vector<int> histogram(256, 0);
    std::vector<int> cdf(256, 0);
    std::vector<unsigned char> lut(256, 0);

    // Handle empty input safely
    if (values.empty())
        return lut;

    // Step 1: Build histogram
    for (unsigned char value : values)
    {
        histogram[value]++;
    }

    // Step 2: Build CDF
    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++)
    {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    // Step 3: Find first non-zero CDF value
    int cdfMin = 0;
    for (int i = 0; i < 256; i++)
    {
        if (cdf[i] > 0)
        {
            cdfMin = cdf[i];
            break;
        }
    }

    int total = static_cast<int>(values.size());

    // Special case: constant image
    if (total == cdfMin)
    {
        // All pixels have the same value → no enhancement needed
        for (int i = 0; i < 256; i++)
        {
            lut[i] = static_cast<unsigned char>(i);
        }
        return lut;
    }

    // Build lookup table
    for (int i = 0; i < 256; i++)
    {
        float mapped = static_cast<float>(cdf[i] - cdfMin) /
                       static_cast<float>(total - cdfMin);

        // Ensure numerical stability
        if (mapped < 0.0f)
            mapped = 0.0f;

        lut[i] = static_cast<unsigned char>(std::round(mapped * 255.0f));
    }

    return lut;
}

/**
 * @brief Apply histogram equalisation to an image.
 *
 * This function enhances image contrast by redistributing intensity values.
 *
 * Two cases are handled:
 *
 * 1. Grayscale images:
 *    - Equalisation is applied directly to pixel intensities.
 *
 * 2. RGB images:
 *    - Image is converted to HSL colour space.
 *    - Equalisation is applied only to the lightness (L) channel.
 *    - The result is converted back to RGB.
 *
 * Using HSL avoids colour distortion and preserves hue and saturation.
 *
 * @param image Reference to the Image object (modified in-place).
 *
 * Time Complexity: O(width × height)
 */
void HistogramEqualisation::apply(Image& image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    // Validate image size
    if (width <= 0 || height <= 0)
        return;

    // Case 1: Grayscale image
    if (channels == 1)
    {
        std::vector<unsigned char> grayValues;
        grayValues.reserve(width * height);

        // Extract pixel intensities
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                grayValues.push_back(image.getPixel(x, y, 0));
            }
        }

        // Build LUT
        std::vector<unsigned char> lut = buildEqualisationLUT(grayValues);

        // Apply LUT
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                unsigned char oldValue = image.getPixel(x, y, 0);
                image.setPixel(x, y, 0, lut[oldValue]);
            }
        }

        return;
    }

    // Case 2: RGB / RGBA image
    if (channels >= 3)
    {
        // Convert to HSL
        std::vector<float> hslData = image.toHSL();
        std::vector<unsigned char> lightnessValues;
        lightnessValues.reserve(width * height);

        // Extract lightness channel and scale to [0,255]
        for (int i = 2; i < static_cast<int>(hslData.size()); i += 3)
        {
            float l = hslData[i];

            // Clamp to valid range [0,1]
            if (l < 0.0f)
                l = 0.0f;
            if (l > 1.0f)
                l = 1.0f;

            lightnessValues.push_back(
                static_cast<unsigned char>(std::round(l * 255.0f))
            );
        }

        // Build LUT
        std::vector<unsigned char> lut = buildEqualisationLUT(lightnessValues);

        // Apply equalised values back to HSL
        int pixelIndex = 0;
        for (int i = 2; i < static_cast<int>(hslData.size()); i += 3)
        {
            unsigned char oldL = lightnessValues[pixelIndex];
            hslData[i] = static_cast<float>(lut[oldL]) / 255.0f;
            pixelIndex++;
        }

        // Convert back to RGB
        image.updateFromHSL(hslData);
    }
}