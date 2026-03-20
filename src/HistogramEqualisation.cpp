/*
Kruskal
Xinying Yang ada-xy325
Oliver Whitehead ada-ow25
Anton Belov ada-ab2125 (ada-85a5785b61)
Hao Liu ada-hl4425
Jiayu Wu ada-jw1125
Yuhang Dai ada-yd1125
Julia Williams ada-jjw25
*/

#include "HistogramEqualisation.h"
#include "Image.h"

#include <cmath>
#include <string>
#include <vector>

/**
 * @brief Construct a histogram equalisation filter with a colour space choice.
 *
 * @param cs Colour space for RGB images: "HSV" or "HSL".
 */
HistogramEqualisation::HistogramEqualisation(const std::string& cs)
    : colourSpace(cs)
{
    if (colourSpace != "HSV" && colourSpace != "HSL")
        colourSpace = "HSV";
}

/**
 * @brief Build a lookup table (LUT) for histogram equalisation.
 *
 * This function computes a mapping from original pixel intensities
 * to equalised intensities using the cumulative distribution function (CDF).
 *
 * @param values Input pixel intensity values (0–255).
 * @return LUT mapping original intensities to equalised ones.
 */
std::vector<unsigned char> HistogramEqualisation::buildEqualisationLUT(
    const std::vector<unsigned char>& values) const
{
    std::vector<int> histogram(256, 0);
    std::vector<int> cdf(256, 0);
    std::vector<unsigned char> lut(256, 0);

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
        for (int i = 0; i < 256; i++)
        {
            lut[i] = static_cast<unsigned char>(i);
        }
        return lut;
    }

    // Step 4: Build LUT
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

/**
 * @brief Apply histogram equalisation to an image.
 *
 * Grayscale images are equalised directly.
 *
 * For RGB images:
 * - if colourSpace == "HSV", equalise the V channel
 * - if colourSpace == "HSL", equalise the L channel
 *
 * @param image Reference to the Image object (modified in-place).
 */
void HistogramEqualisation::apply(Image& image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    if (width <= 0 || height <= 0)
        return;

    // Case 1: Grayscale image
    if (channels == 1)
    {
        std::vector<unsigned char> grayValues;
        grayValues.reserve(width * height);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                grayValues.push_back(image.getPixelSimple(x, y, 0));
            }
        }

        std::vector<unsigned char> lut = buildEqualisationLUT(grayValues);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                unsigned char oldValue = image.getPixelSimple(x, y, 0);
                image.setPixel(x, y, 0, lut[oldValue]);
            }
        }

        return;
    }

    // Case 2: RGB / RGBA image using HSV
    if (channels >= 3 && colourSpace == "HSV")
    {
        std::vector<float> hsvData = image.toHSV();
        std::vector<unsigned char> valueChannel;
        valueChannel.reserve(width * height);

        // Extract V channel and scale to [0,255]
        for (int i = 2; i < static_cast<int>(hsvData.size()); i += 3)
        {
            float v = hsvData[i];

            if (v < 0.0f) v = 0.0f;
            if (v > 1.0f) v = 1.0f;

            valueChannel.push_back(
                static_cast<unsigned char>(std::round(v * 255.0f))
            );
        }

        std::vector<unsigned char> lut = buildEqualisationLUT(valueChannel);

        int pixelIndex = 0;
        for (int i = 2; i < static_cast<int>(hsvData.size()); i += 3)
        {
            unsigned char oldV = valueChannel[pixelIndex];
            hsvData[i] = static_cast<float>(lut[oldV]) / 255.0f;
            pixelIndex++;
        }

        image.updateFromHSV(hsvData);
        return;
    }

    // Case 3: RGB / RGBA image using HSL
    if (channels >= 3 && colourSpace == "HSL")
    {
        std::vector<float> hslData = image.toHSL();
        std::vector<unsigned char> lightnessValues;
        lightnessValues.reserve(width * height);

        // Extract L channel and scale to [0,255]
        for (int i = 2; i < static_cast<int>(hslData.size()); i += 3)
        {
            float l = hslData[i];

            if (l < 0.0f) l = 0.0f;
            if (l > 1.0f) l = 1.0f;

            lightnessValues.push_back(
                static_cast<unsigned char>(std::round(l * 255.0f))
            );
        }

        std::vector<unsigned char> lut = buildEqualisationLUT(lightnessValues);

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