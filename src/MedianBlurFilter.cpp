#include "MedianBlurFilter.h"

#include "Image.h"
#include "Volume.h"

#include <array>
#include <stdexcept>

namespace {

unsigned char findMedianFromHistogram(const std::array<int, 256>& histogram, int targetCount)
{
    int cumulativeCount = 0;

    for (int value = 0; value < 256; ++value) {
        cumulativeCount += histogram[value];
        if (cumulativeCount >= targetCount) {
            return static_cast<unsigned char>(value);
        }
    }

    return 255;
}

} // namespace

MedianBlurFilter::MedianBlurFilter(int kernelSize) : kernelSize(kernelSize) {}

bool MedianBlurFilter::isValidKernelSize(int kernelSize) const
{
    return (kernelSize > 1) && (kernelSize % 2 == 1);
}

void MedianBlurFilter::apply(Image& img)
{
    if (!isValidKernelSize(kernelSize)) {
        throw std::invalid_argument("Kernel size must be odd and greater than 1");
    }

    const int width = img.getWidth();
    const int height = img.getHeight();
    const int channels = img.getChannels();
    const int radius = kernelSize / 2;
    const int targetCount = (kernelSize * kernelSize) / 2 + 1;

    Image original = img;

    for (int y = 0; y < height; ++y) {
        for (int channel = 0; channel < channels; ++channel) {
            std::array<int, 256> histogram{};

            for (int ny = y - radius; ny <= y + radius; ++ny) {
                for (int nx = -radius; nx <= radius; ++nx) {
                    ++histogram[original.getPixel(nx, ny, channel)];
                }
            }

            for (int x = 0; x < width; ++x) {
                img.setPixel(x, y, channel, findMedianFromHistogram(histogram, targetCount));

                if (x == width - 1) {
                    continue;
                }

                const int removeX = x - radius;
                const int addX = x + radius + 1;

                for (int ny = y - radius; ny <= y + radius; ++ny) {
                    --histogram[original.getPixel(removeX, ny, channel)];
                    ++histogram[original.getPixel(addX, ny, channel)];
                }
            }
        }
    }
}

void MedianBlurFilter::apply(Volume& volume)
{
    if (!isValidKernelSize(kernelSize)) {
        throw std::invalid_argument("Kernel size must be odd and greater than 1");
    }

    const int width = volume.getWidth();
    const int height = volume.getHeight();
    const int depth = volume.getDepth();
    const int channels = volume.getChannels();
    const int radius = kernelSize / 2;
    const int targetCount = (kernelSize * kernelSize * kernelSize) / 2 + 1;

    Volume original = volume;

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int channel = 0; channel < channels; ++channel) {
                std::array<int, 256> histogram{};

                for (int nz = z - radius; nz <= z + radius; ++nz) {
                    for (int ny = y - radius; ny <= y + radius; ++ny) {
                        for (int nx = -radius; nx <= radius; ++nx) {
                            ++histogram[original.getVoxel(nx, ny, nz, channel)];
                        }
                    }
                }

                for (int x = 0; x < width; ++x) {
                    volume.setVoxel(x, y, z, channel, findMedianFromHistogram(histogram, targetCount));

                    if (x == width - 1) {
                        continue;
                    }

                    const int removeX = x - radius;
                    const int addX = x + radius + 1;

                    for (int nz = z - radius; nz <= z + radius; ++nz) {
                        for (int ny = y - radius; ny <= y + radius; ++ny) {
                            --histogram[original.getVoxel(removeX, ny, nz, channel)];
                            ++histogram[original.getVoxel(addX, ny, nz, channel)];
                        }
                    }
                }
            }
        }
    }
}
