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

#ifndef GAUSSIAN_BLUR_FILTER_H
#define GAUSSIAN_BLUR_FILTER_H

#include "Filter.h"
#include <vector>

class Image;
class Volume;

/**
 * @class GaussianBlurFilter
 * @brief Applies separable Gaussian smoothing to images and volumes.
 *
 * The filter creates a 1-D Gaussian kernel and applies it along each axis.
 * For images this means two passes; for volumes it means three passes.
 */
class GaussianBlurFilter : public Filter {
private:
    /// Width of the Gaussian kernel. Must be odd and greater than 1.
    int kernelSize;
    /// Standard deviation controlling the spread of the Gaussian.
    double sigma;

    /**
     * @brief Check whether the kernel size is usable.
     * @param kernelSize Candidate kernel width.
     * @return True when the size is odd and greater than 1.
     */
    bool isValidKernelSize(int kernelSize) const;
    /**
     * @brief Build a normalised 1-D Gaussian kernel.
     * @param kernelSize Number of taps in the kernel.
     * @param sigma Standard deviation of the Gaussian.
     * @return Normalised kernel coefficients.
     */
    std::vector<double> createGaussianKernel1D(int kernelSize, double sigma) const;

public:
    /**
     * @brief Construct a Gaussian blur filter.
     * @param kernelSize Width of the 1-D Gaussian kernel.
     * @param sigma Standard deviation of the Gaussian.
     */
    GaussianBlurFilter(int kernelSize, double sigma);

    /**
     * @brief Apply Gaussian blur to a 2-D image.
     * @param image Image to modify.
     */
    void apply(Image& image) override;
    /**
     * @brief Apply Gaussian blur to a 3-D volume.
     * @param volume Volume to modify.
     */
    void apply(Volume& volume) override;
};

#endif // GAUSSIAN_BLUR_FILTER_H
