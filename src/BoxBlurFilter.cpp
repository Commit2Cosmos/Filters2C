#include "BoxBlurFilter.h"

#include "Image.h"

#include <algorithm>
#include <stdexcept>

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

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int xStart = std::max(0, x - radius);
            const int xEnd = std::min(width - 1, x + radius);
            const int yStart = std::max(0, y - radius);
            const int yEnd = std::min(height - 1, y + radius);

            for (int c = 0; c < channels; ++c) {
                int sum = 0;
                int count = 0;

                for (int ny = yStart; ny <= yEnd; ++ny) {
                    for (int nx = xStart; nx <= xEnd; ++nx) {
                        sum += original.getPixel(nx, ny, c);
                        ++count;
                    }
                }

                img.setPixel(x, y, c, static_cast<unsigned char>(sum / count));
            }
        }
    }
}
