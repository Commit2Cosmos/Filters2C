#include "Blur.h"
# include "Image.h"
#include <stdexcept>

// Helper function to check that kernel size is valid (odd and greater than 1)
bool Blur::isValidKernelSize(int kernelSize) const
{
    // Kernel size must be odd and greater than 1
    return (kernelSize > 1) && (kernelSize % 2 == 1);
}

void Blur::applyBoxBlur(Image& img, int kernelSize)
{
    // Use helper function to validate kernel size
    if (!isValidKernelSize(kernelSize)) {
        throw std::invalid_argument("Kernel size must be odd and greater than 1");
    }

    // Get image dimensions
    int width = img.getWidth();
    int height = img.getHeight();

    // Define the radius of the kernel
    int radius = kernelSize / 2;
    
    // Create a copy of the original image to read pixel values from
    Image original = img;

    // Loop through each pixel in the image
        for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Calculate bounds of the kernel window, ensuring we don't go out of image bounds
            int xStart = std::max(0, x - radius);
            int xEnd   = std::min(width - 1, x + radius);
            int yStart = std::max(0, y - radius);
            int yEnd   = std::min(height - 1, y + radius);

            int sum = 0;
            int count = 0;

            for (int ny = yStart; ny <= yEnd; ++ny) {
                for (int nx = xStart; nx <= xEnd; ++nx) {
                    sum += original.getPixel(nx, ny);
                    ++count;
                }
            }

            img.setPixel(x, y, sum / count);
        }
    }
}

/*
    GAUSSIAN BLUR

    Applies a Gaussian-weighted convolution to the image.

    Steps:
    1. Validate kernel size
    2. Compute Gaussian kernel using sigma
    3. Normalize the kernel
    4. Copy original image
    5. Loop through pixels
    6. Apply weighted convolution
    7. Write blurred value back to the image
*/
void Blur::applyGaussianBlur(Image& img, int kernelSize, double sigma)
{
    // Use helper function to validate kernel size
    if (!isValidKernelSize(kernelSize)) {
        throw std::invalid_argument("Kernel size must be odd and greater than 1");
    }

    // Get image dimensions
    int width = img.getWidth();
    int height = img.getHeight();

    // Create Gaussian kernel
    int radius = kernelSize / 2;
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));
    double sum = 0.0;
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {

        }
    }
    // Normalize kernel values

    // Copy original image

    // Loop through pixels

        // Apply convolution with Gaussian weights

        // Store result in output image
}


/*
    MEDIAN BLUR

    Replaces each pixel with the median value
    of neighbouring pixels in the kernel window.

    Steps:
    1. Validate kernel size
    2. Copy original image
    3. Loop through pixels
    4. Collect neighbouring pixel values
    5. Sort the values
    6. Select the median value
    7. Write median back to the image
*/
void Blur::applyMedianBlur(Image& img, int kernelSize)
{
    // Validate kernel size

    // Get image dimensions

    // Copy original image

    // Loop through pixels

        // Collect neighbouring pixel values

        // Sort the values

        // Select median

        // Write median pixel value
}