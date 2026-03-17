#include "GaussianBlurFilter.h"

#include "Image.h"

#include <cmath>
#include <stdexcept>
#include <vector>

GaussianBlurFilter::GaussianBlurFilter(int kernelSize, double sigma)
    : kernelSize(kernelSize), sigma(sigma) {}

bool GaussianBlurFilter::isValidKernelSize(int kernelSize) const
{
    return (kernelSize > 1) && (kernelSize % 2 == 1);
}

std::vector<std::vector<double>> GaussianBlurFilter::createGaussianKernel(int kernelSize, double sigma) const
{
    const int radius = kernelSize / 2;
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));
    double sum = 0.0;

    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            const double value = std::exp(-(x * x + y * y) / (2.0 * sigma * sigma));
            kernel[y + radius][x + radius] = value;
            sum += value;
        }
    }

    for (int y = 0; y < kernelSize; ++y) {
        for (int x = 0; x < kernelSize; ++x) {
            kernel[y][x] /= sum;
        }
    }

    return kernel;
}

void GaussianBlurFilter::apply(Image& img)
{
    if (!isValidKernelSize(kernelSize)) {
        throw std::invalid_argument("Kernel size must be odd and greater than 1");
    }

    const int width = img.getWidth();
    const int height = img.getHeight();
    const int channels = img.getChannels();
    const int radius = kernelSize / 2;
    const auto kernel = createGaussianKernel(kernelSize, sigma);

    Image original = img;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                double sum = 0.0;

                for (int ky = -radius; ky <= radius; ++ky) {
                    for (int kx = -radius; kx <= radius; ++kx) {
                        sum += original.getPixel(x + kx, y + ky, c) * kernel[ky + radius][kx + radius];
                    }
                }

                img.setPixel(x, y, c, static_cast<unsigned char>(std::round(sum)));
            }
        }
    }
}
