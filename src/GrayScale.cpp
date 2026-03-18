#include "GrayScale.h"
#include "Image.h"

/**
 * @brief Apply grayscale conversion to an image.
 *
 * This function converts an RGB image into a grayscale image by computing
 * a weighted sum of the red, green, and blue channels for each pixel.
 * The weights follow the standard luminance formula used in image processing:
 *
 *      gray = 0.2126 * R + 0.7152 * G + 0.0722 * B
 *
 * These coefficients reflect the human visual system's sensitivity to
 * different colours (green contributes the most, blue the least).
 *
 * The resulting grayscale value is then written back to all three colour
 * channels (R, G, B) to maintain a 3-channel image format.
 *
 * @param image Reference to the Image object to be modified in-place.
 */
void GrayScale::apply(Image& image)
{
    // Retrieve image dimensions and number of channels
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    // This filter only applies to RGB images (at least 3 channels required)
    // If the image is already grayscale or invalid, do nothing
    if (channels < 3)
        return;

    // Loop over every pixel in the image
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
             // Extract RGB components of the current pixel
            unsigned char r = image.getPixel(x, y, 0);
            unsigned char g = image.getPixel(x, y, 1);
            unsigned char b = image.getPixel(x, y, 2);

            // Compute grayscale intensity using perceptual weighting
            unsigned char gray = static_cast<unsigned char>(
                0.2126 * r +
                0.7152 * g +
                0.0722 * b
            );

            // Set all three channels to the grayscale value
            // This keeps the image in RGB format but visually grayscale
            image.setPixel(x, y, 0, gray);
            image.setPixel(x, y, 1, gray);
            image.setPixel(x, y, 2, gray);
        }
    }
}