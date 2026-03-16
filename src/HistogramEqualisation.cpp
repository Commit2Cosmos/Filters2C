#include "HistogramEqualisation.h"
#include <cmath>
#include <vector>

std::vector<unsigned char> HistogramEqualisation::buildEqualisationLUT(const std::vector<unsigned char>& values) const
{
    std::vector<int> histogram(256, 0);
    std::vector<int> cdf(256, 0);
    std::vector<unsigned char> lut(256, 0);

    if (values.empty())
        return lut;

    // Build histogram
    for (unsigned char value : values)
    {
        histogram[value]++;
    }

    // Build cumulative distribution function
    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++)
    {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    // Find the first non-zero CDF value
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

    // If all pixels have the same value, keep them unchanged
    if (total == cdfMin)
    {
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

        if (mapped < 0.0f)
            mapped = 0.0f;

        lut[i] = static_cast<unsigned char>(std::round(mapped * 255.0f));
    }

    return lut;
}

void HistogramEqualisation::apply(Image& image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    if (width <= 0 || height <= 0)
        return;

    // Grayscale image
    if (channels == 1)
    {
        std::vector<unsigned char> grayValues;
        grayValues.reserve(width * height);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                grayValues.push_back(image.getPixel(x, y, 0));
            }
        }

        std::vector<unsigned char> lut = buildEqualisationLUT(grayValues);

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

    // RGB or RGBA image
    if (channels >= 3)
    {
        std::vector<float> hslData = image.toHSL();
        std::vector<unsigned char> lightnessValues;
        lightnessValues.reserve(width * height);

        // Extract L channel and scale from [0,1] to [0,255]
        for (int i = 2; i < static_cast<int>(hslData.size()); i += 3)
        {
            float l = hslData[i];

            if (l < 0.0f)
                l = 0.0f;
            if (l > 1.0f)
                l = 1.0f;

            lightnessValues.push_back(
                static_cast<unsigned char>(std::round(l * 255.0f))
            );
        }

        std::vector<unsigned char> lut = buildEqualisationLUT(lightnessValues);

        // Write equalised L channel back to HSL data
        int pixelIndex = 0;
        for (int i = 2; i < static_cast<int>(hslData.size()); i += 3)
        {
            unsigned char oldL = lightnessValues[pixelIndex];
            hslData[i] = static_cast<float>(lut[oldL]) / 255.0f;
            pixelIndex++;
        }

        image.updateFromHSL(hslData);
    }
}