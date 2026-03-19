#include "Threshold.h"
#include "Image.h"

#include <vector>
#include <string>

/**
 * @brief Construct a Threshold filter with a threshold value and colour space.
 *
 * @param value Threshold in 8-bit intensity space [0,255].
 * @param cs Colour space for RGB images: "HSV" or "HSL".
 */
Threshold::Threshold(int value, const std::string& cs)
    : thresholdValue(value), colourSpace(cs)
{
    // Clamp threshold to valid 8-bit range
    if (thresholdValue < 0)
        thresholdValue = 0;
    if (thresholdValue > 255)
        thresholdValue = 255;

    // Fallback to HSV if invalid string is provided
    if (colourSpace != "HSV" && colourSpace != "HSL")
        colourSpace = "HSV";
}

/**
 * @brief Apply binary thresholding to an image.
 *
 * For grayscale images, thresholding is applied directly to intensity values.
 *
 * For RGB images:
 * - If colourSpace == "HSV", threshold the V channel.
 * - If colourSpace == "HSL", threshold the L channel.
 *
 * In both RGB cases, saturation is set to zero so that the final output
 * is truly black-and-white rather than colour-posterised.
 *
 * @param image Reference to the Image object to be modified in-place.
 *
 * Time Complexity: O(width × height)
 */
void Threshold::apply(Image& image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    // Case 1: grayscale image
    if (channels == 1)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int pixel = image.getPixelSimple(x, y, 0);
                pixel = (pixel > thresholdValue) ? 255 : 0;
                image.setPixel(x, y, 0, static_cast<unsigned char>(pixel));
            }
        }
        return;
    }

    // Normalised threshold for HSV/HSL channels in [0,1]
    const float thresholdNormalised =
        static_cast<float>(thresholdValue) / 255.0f;

    // Case 2: RGB image using HSV
    if (channels >= 3 && colourSpace == "HSV")
    {
        std::vector<float> hsvData = image.toHSV();

        for (int i = 0; i < width * height; i++)
        {
            int idx = i * 3;
            float& s = hsvData[idx + 1]; // saturation
            float& v = hsvData[idx + 2]; // value

            v = (v > thresholdNormalised) ? 1.0f : 0.0f;
            s = 0.0f; // remove colour for black/white output
        }

        image.updateFromHSV(hsvData);
        return;
    }

    // Case 3: RGB image using HSL
    if (channels >= 3 && colourSpace == "HSL")
    {
        std::vector<float> hslData = image.toHSL();

        for (int i = 0; i < width * height; i++)
        {
            int idx = i * 3;
            float& s = hslData[idx + 1]; // saturation
            float& l = hslData[idx + 2]; // lightness

            l = (l > thresholdNormalised) ? 1.0f : 0.0f;
            s = 0.0f; // remove colour for black/white output
        }

        image.updateFromHSL(hslData);
    }
}