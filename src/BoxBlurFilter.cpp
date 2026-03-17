#include "BoxBlurFilter.h"

#include "Image.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

BoxBlurFilter::BoxBlurFilter(int kernelSize) : kernelSize(kernelSize) {}

bool BoxBlurFilter::isValidKernelSize(int kernelSize) const
{
    return (kernelSize > 1) && (kernelSize % 2 == 1);
}

void BoxBlurFilter::apply(Image& img)
{
    if (!isValidKernelSize(kernelSize)) {
        throw std::invalid_argument("Kernel size must be odd and greater than 1");
    }

    const int width = img.getWidth();
    const int height = img.getHeight();
    const int channels = img.getChannels();
    const int radius = kernelSize / 2;

    Image original = img;
    std::vector<int> horizontalSums(width * height * channels, 0);
    const int pixelCount = kernelSize * kernelSize;

    auto horizontalIndex = [width, channels](int x, int y, int c) {
        return (y * width + x) * channels + c;
    };

    // Pass 1: horizontal sliding window sums
    for (int y = 0; y < height; ++y) {
        for (int c = 0; c < channels; ++c) {
            int runningSum = 0;

            for (int nx = -radius; nx <= radius; ++nx) {
                runningSum += original.getPixel(nx, y, c);
            }

            for (int x = 0; x < width; ++x) {
                horizontalSums[horizontalIndex(x, y, c)] = runningSum;

                const int removeX = x - radius;
                const int addX = x + radius + 1;

                runningSum -= original.getPixel(removeX, y, c);
                runningSum += original.getPixel(addX, y, c);
            }
        }
    }

    // Pass 2: vertical sliding window over horizontal sums
    for (int x = 0; x < width; ++x) {
        for (int c = 0; c < channels; ++c) {
            int runningSum = 0;

            for (int ny = -radius; ny <= radius; ++ny) {
                runningSum += horizontalSums[horizontalIndex(x, std::clamp(ny, 0, height - 1), c)];
            }

            for (int y = 0; y < height; ++y) {
                img.setPixel(x, y, c, static_cast<unsigned char>(runningSum / pixelCount));

                const int removeY = y - radius;
                const int addY = y + radius + 1;

                runningSum -= horizontalSums[horizontalIndex(x, std::clamp(removeY, 0, height - 1), c)];
                runningSum += horizontalSums[horizontalIndex(x, std::clamp(addY, 0, height - 1), c)];
            }
        }
    }
}
