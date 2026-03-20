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

#include "GaussianBlurFilter.h"

#include "Image.h"
#include "Volume.h"

#include <cmath>
#include <stdexcept>
#include <vector>

GaussianBlurFilter::GaussianBlurFilter(int kernelSize, double sigma)
    : kernelSize(kernelSize), sigma(sigma) {}

bool GaussianBlurFilter::isValidKernelSize(int kernelSize) const
{
    return kernelSize > 1 && kernelSize % 2 == 1;
}

std::vector<double> GaussianBlurFilter::createGaussianKernel1D(int kernelSize, double sigma) const
{
    const int radius = kernelSize / 2;
    std::vector<double> kernel(kernelSize, 0.0);

    double sum = 0.0;
    for (int i = -radius; i <= radius; ++i) {
        const double value = std::exp(-(i * i) / (2.0 * sigma * sigma));
        kernel[i + radius] = value;
        sum += value;
    }

    for (double& v : kernel) {
        v /= sum;
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

    const std::vector<double> kernel = createGaussianKernel1D(kernelSize, sigma);

    Image original = img;
    Image temp = img;

    // Pass 1: horizontal (x)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                double sum = 0.0;

                for (int k = -radius; k <= radius; ++k) {
                    sum += original.getPixel(x + k, y, c) * kernel[k + radius];
                }

                temp.setPixel(x, y, c, static_cast<unsigned char>(std::round(sum)));
            }
        }
    }

    // Pass 2: vertical (y)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                double sum = 0.0;

                for (int k = -radius; k <= radius; ++k) {
                    sum += temp.getPixel(x, y + k, c) * kernel[k + radius];
                }

                img.setPixel(x, y, c, static_cast<unsigned char>(std::round(sum)));
            }
        }
    }
}

void GaussianBlurFilter::apply(Volume& volume)
{
    if (!isValidKernelSize(kernelSize)) {
        throw std::invalid_argument("Kernel size must be odd and greater than 1");
    }

    const int width = volume.getWidth();
    const int height = volume.getHeight();
    const int depth = volume.getDepth();
    const int channels = volume.getChannels();
    const int radius = kernelSize / 2;

    const std::vector<double> kernel = createGaussianKernel1D(kernelSize, sigma);

    Volume original = volume;
    Volume temp1 = volume;
    Volume temp2 = volume;

    // Pass 1: x direction
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                for (int c = 0; c < channels; ++c) {
                    double sum = 0.0;

                    for (int k = -radius; k <= radius; ++k) {
                        sum += original.getVoxel(x + k, y, z, c) * kernel[k + radius];
                    }

                    temp1.setVoxel(x, y, z, c, static_cast<unsigned char>(std::round(sum)));
                }
            }
        }
    }

    // Pass 2: y direction
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                for (int c = 0; c < channels; ++c) {
                    double sum = 0.0;

                    for (int k = -radius; k <= radius; ++k) {
                        sum += temp1.getVoxel(x, y + k, z, c) * kernel[k + radius];
                    }

                    temp2.setVoxel(x, y, z, c, static_cast<unsigned char>(std::round(sum)));
                }
            }
        }
    }

    // Pass 3: z direction
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                for (int c = 0; c < channels; ++c) {
                    double sum = 0.0;

                    for (int k = -radius; k <= radius; ++k) {
                        sum += temp2.getVoxel(x, y, z + k, c) * kernel[k + radius];
                    }

                    volume.setVoxel(x, y, z, c, static_cast<unsigned char>(std::round(sum)));
                }
            }
        }
    }
}