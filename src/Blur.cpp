#include "Blur.h"
#include "Image.h"

#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <vector>

// Helper function to check that kernel size is valid (odd and greater than 1)
bool Blur::isValidKernelSize(int kernelSize) const
{
    // Kernel size must be odd and greater than 1
    return (kernelSize > 1) && (kernelSize % 2 == 1);
}

// Helper function to create Gaussian kernel
std::vector<std::vector<double>> Blur::createGaussianKernel(int kernelSize, double sigma)
{
    // Calculate kernel radius
    int radius = kernelSize / 2;

    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));
    double sum = 0.0;

    // Compute Gaussian kernel values
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            double value = std::exp(-(x * x + y * y) / (2.0 * sigma * sigma));
            kernel[y + radius][x + radius] = value;
            sum += value;
        }
    }

    // Normalize kernel values so they sum to 1
    for (int y = 0; y < kernelSize; ++y) {
        for (int x = 0; x < kernelSize; ++x) {
            kernel[y][x] /= sum;
        }
    }

    return kernel;
}

// Helper function to get median value from kernel window for one channel
unsigned char Blur::getMedianValue(const Image& img, int centerX, int centerY, int channel, int kernelSize) const
{
    const int radius = kernelSize / 2;
    std::vector<unsigned char> values;
    values.reserve(kernelSize * kernelSize);

    for (int y = centerY - radius; y <= centerY + radius; ++y) {
        for (int x = centerX - radius; x <= centerX + radius; ++x) {
            values.push_back(img.getPixel(x, y, channel));
        }
    }

    std::sort(values.begin(), values.end());
    return values[values.size() / 2];
}

// Apply Blur Algorithms
void Blur::applyBoxBlur(Image& img, int kernelSize)
{
    // Use helper function to validate kernel size
    if (!isValidKernelSize(kernelSize)) {
        throw std::invalid_argument("Kernel size must be odd and greater than 1");
    }

    // Get image dimensions
    const int width = img.getWidth();
    const int height = img.getHeight();
    const int channels = img.getChannels();

    // Define the radius of the kernel
    const int radius = kernelSize / 2;
    
    // Create a copy of the original image to read pixel values from
    Image original = img;

    // Loop through each pixel in the image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Calculate bounds of the kernel window
            const int xStart = std::max(0, x - radius);
            const int xEnd   = std::min(width - 1, x + radius);
            const int yStart = std::max(0, y - radius);
            const int yEnd   = std::min(height - 1, y + radius);

            // Loop through each channel
            for (int c = 0; c < channels; ++c) {
                int sum = 0;
                int count = 0;

                // Sum up pixel values in the kernel window
                for (int ny = yStart; ny <= yEnd; ++ny) {
                    for (int nx = xStart; nx <= xEnd; ++nx) {
                        sum += original.getPixel(nx, ny, c);
                        ++count;
                    }
                }

                // Calculate the kernel average and set the blurred pixel value
                unsigned char average = static_cast<unsigned char>(sum / count);
                img.setPixel(x, y, c, average);
            }
        }
    }
}


void Blur::applyGaussianBlur(Image& img, int kernelSize, double sigma)
{
    // Use helper function to validate kernel size
    if (!isValidKernelSize(kernelSize)) {
        throw std::invalid_argument("Kernel size must be odd and greater than 1");
    }

    // Get image dimensions
    const int width = img.getWidth();
    const int height = img.getHeight();
    const int channels = img.getChannels();

    // Define radius of the kernel
    const int radius = kernelSize / 2;

    // Create Gaussian kernel using helper function
    auto kernel = createGaussianKernel(kernelSize, sigma);

    // Copy original image to read pixel values from
    Image original = img;

    // Loop through each pixel
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            // Loop through each channel
            for (int c = 0; c < channels; ++c) {

                double sum = 0.0;

                // Apply weighted convolution using the Gaussian kernel
                for (int ky = -radius; ky <= radius; ++ky) {
                    for (int kx = -radius; kx <= radius; ++kx) {

                        int nx = std::max(0, std::min(x + kx, width - 1));
                        int ny = std::max(0, std::min(y + ky, height - 1));

                        double weight = kernel[ky + radius][kx + radius];
                        sum += original.getPixel(nx, ny, c) * weight;
                    }
                }

                img.setPixel(x, y, c, static_cast<unsigned char>(std::round(sum)));
            }
        }
    }
}
    
void Blur::applyMedianBlur(Image& img, int kernelSize)
{
    // Validate kernel size
    if (!isValidKernelSize(kernelSize)) {
        throw std::invalid_argument("Kernel size must be odd and greater than 1");
    }

    // Get image dimensions
    const int width = img.getWidth();
    const int height = img.getHeight();
    const int channels = img.getChannels();

    // Copy original image
    Image original = img;

    // Loop through pixels
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int channel = 0; channel < channels; ++channel) {
                // Collect neighbouring pixel values, sort them, and select the median
                const unsigned char median = getMedianValue(original, x, y, channel, kernelSize);

                // Write median pixel value
                img.setPixel(x, y, channel, median);
            }
        }
    }
}
