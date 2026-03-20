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

#ifndef CONV_FILTER_H
#define CONV_FILTER_H

#include "Filter.h"
#include "Image.h"
#include <vector>

/**
 * @struct Kernel
 * @brief Stores a 2-D convolution kernel in flattened row-major form.
 */
struct Kernel {
    /// Flattened kernel coefficients in row-major order.
    std::vector<float> data;
    /// Kernel width in elements.
    int width;
    /// Kernel height in elements.
    int height;

    /**
     * @brief Build a kernel from nested initializer lists.
     * @param list Rows of kernel coefficients.
     */
    Kernel(std::initializer_list<std::initializer_list<float>> list) {
        height = static_cast<int>(list.size());
        width = (height > 0) ? static_cast<int>(list.begin()->size()) : 0;
        
        data.reserve(width * height);
        for (const auto& row : list) {
            for (float val : row) {
                data.push_back(val); // Flattens 2D input into 1D
            }
        }
    }
    
    Kernel(const Kernel&) = default;
    Kernel& operator=(const Kernel&) = default;

    Kernel(Kernel&&) = default;
    Kernel& operator=(Kernel&&) = default;
};

/**
 * @class ConvFilter
 * @brief Base class for filters implemented as one or two convolution kernels.
 *
 * A single kernel is used for direct filtering operations such as sharpening
 * and embossing, while two kernels are combined using gradient magnitude for
 * edge detectors such as Sobel and Prewitt.
 */
class ConvFilter : public Filter {
public:
    /**
     * @brief Construct a single-kernel convolution filter.
     * @param kernel Kernel to apply.
     */
    explicit ConvFilter(Kernel kernel);
    /**
     * @brief Construct a dual-kernel convolution filter.
     * @param kernelX First kernel, typically the horizontal gradient kernel.
     * @param kernelY Second kernel, typically the vertical gradient kernel.
     */
    ConvFilter(Kernel kernelX, Kernel kernelY);
    virtual ~ConvFilter() = default;

    /**
     * @brief Apply the configured convolution filter to an image.
     * @param image Image to modify.
     */
    void apply(Image& image) override;

protected:
    /**
     * @brief Convolve one image channel.
     * @param image Source image.
     * @param ch Channel index to process.
     * @return Floating-point responses for every pixel in the selected channel.
     */
    std::vector<float> convolveChannel(const Image& image, int ch) const;

    /**
     * @brief Compute filter responses for every image channel.
     * @param image Source image.
     * @return One floating-point response buffer per channel.
     */
    std::vector<std::vector<float>> computeResults(const Image& image) const;
    /**
     * @brief Scale filter responses to the 8-bit output range.
     * @param results Response buffers to scale in-place.
     * @param theoreticalMax Maximum response magnitude expected for the filter.
     */
    void normalise(std::vector<std::vector<float>>& results, float theoreticalMax) const;
    /**
     * @brief Clamp response buffers and copy them back into an image.
     * @param image Destination image.
     * @param results Response buffers produced by convolution.
     */
    void writeResults(Image& image, const std::vector<std::vector<float>>& results) const;

    /// One kernel for direct filters or two kernels for gradient filters.
    std::vector<Kernel> kernels_;
    
    /**
     * @brief Convert an axis-wise maximum response into a magnitude bound.
     * @param filterMax Largest expected absolute response for one kernel axis.
     * @return Theoretical maximum gradient magnitude for normalisation.
     */
    virtual float getTheoreticalMax(float filterMax) const;
};

#endif
