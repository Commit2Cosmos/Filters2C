#include "MedianBlurFilter.h"

#include "Image.h"
#include "Volume.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace {

int partition(std::vector<unsigned char>& arr, int left, int right)
{
    const unsigned char pivot = arr[right];
    int i = left;

    for (int j = left; j < right; ++j) {
        if (arr[j] <= pivot) {
            std::swap(arr[i], arr[j]);
            ++i;
        }
    }

    std::swap(arr[i], arr[right]);
    return i;
}

unsigned char quickSelectMedian(std::vector<unsigned char>& arr, int left, int right, int k)
{
    if (left == right) {
        return arr[left];
    }

    const int pivotIndex = partition(arr, left, right);

    if (k == pivotIndex) {
        return arr[k];
    }

    if (k < pivotIndex) {
        return quickSelectMedian(arr, left, pivotIndex - 1, k);
    }

    return quickSelectMedian(arr, pivotIndex + 1, right, k);
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

    Image original = img;
    std::vector<unsigned char> values;
    values.reserve(kernelSize * kernelSize);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int channel = 0; channel < channels; ++channel) {
                values.clear();

                for (int ny = y - radius; ny <= y + radius; ++ny) {
                    for (int nx = x - radius; nx <= x + radius; ++nx) {
                        values.push_back(original.getPixel(nx, ny, channel));
                    }
                }

                const int k = static_cast<int>(values.size()) / 2;
                const unsigned char median =
                    quickSelectMedian(values, 0, static_cast<int>(values.size()) - 1, k);

                img.setPixel(x, y, channel, median);
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

    Volume original = volume;
    std::vector<unsigned char> values;
    values.reserve(kernelSize * kernelSize * kernelSize);
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                for (int channel = 0; channel < channels; ++channel) {
                    values.clear();

                    for (int nz = z - radius; nz <= z + radius; ++nz) {
                        for (int ny = y - radius; ny <= y + radius; ++ny) {
                            for (int nx = x - radius; nx <= x + radius; ++nx) {
                                values.push_back(original.getVoxel(nx, ny, nz, channel));
                            }
                        }
                    }

                    const int k = static_cast<int>(values.size()) / 2;
                    const unsigned char median =
                        quickSelectMedian(values, 0, static_cast<int>(values.size()) - 1, k);

                    volume.setVoxel(x, y, z, channel, median);
                }
            }
        }
    }
}
