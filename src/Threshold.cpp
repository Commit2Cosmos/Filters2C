#include "Threshold.h"
#include "Image.h"
#include <vector>

/**
 * @brief Apply binary thresholding to an image.
 *
 * This function converts an image into a binary (black-and-white) image
 * based on a fixed threshold value. Pixels above the threshold are set
 * to white (255 or 1.0), and those below are set to black (0).
 *
 * Two cases are handled:
 *
 * 1. Grayscale images (single channel):
 *    - Threshold is applied directly to pixel intensity.
 *
 * 2. RGB images (three or more channels):
 *    - The image is first converted to HSL colour space.
 *    - Thresholding is applied to the lightness (L) channel.
 *    - Saturation (S) is set to zero to remove colour, producing a
 *      purely black-and-white output.
 *
 * Using HSL allows thresholding based on perceived brightness rather
 * than raw RGB values, which produces more visually meaningful results.
 *
 * @param image Reference to the Image object to be modified in-place.
 *
 * Time Complexity: O(width × height)
 */
void Threshold::apply(Image& image)
{
    // Retrieve image properties
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    // Fixed threshold in 8-bit space
    const int thresholdInt = 127;

    // Case 1: grayscale image
    if (channels == 1)
    {
        // Apply threshold directly to intensity values
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int pixel = image.getPixelSimple(x, y, 0);
                // Binary threshold: above -> white, below -> black
                pixel = (pixel > thresholdInt) ? 255 : 0;
                image.setPixel(x, y, 0, pixel);
            }
        }
        return;
    }

    // Case 2: RGB image
    if (channels >= 3)
    {
        // Convert RGB image to HSL representation
        std::vector<float> hslData = image.toHSL();
        // Normalised threshold (0–1 range for HSL)
        const float threshold = 127.0f / 255.0f;

        // Iterate through each pixel (3 values per pixel: H, S, L)
        for (int i = 0; i < width * height; i++)
        {
            int idx = i * 3;
            float& s = hslData[idx + 1]; // saturation
            float& l = hslData[idx + 2]; // lightness

            // Apply threshold to lightness channel
            l = (l > threshold) ? 1.0f : 0.0f;
            // Remove colour information to produce true binary output
            s = 0.0f; // remove colour so output becomes black/white
        }

        // Convert back from HSL to RGB and update image
        image.updateFromHSL(hslData);
    }
}