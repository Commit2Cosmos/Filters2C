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

#ifndef MEDIAN_BLUR_FILTER_H
#define MEDIAN_BLUR_FILTER_H

#include "Filter.h"

/**
 * @class MedianBlurFilter
 * @brief Removes impulsive noise by taking the median of local neighbourhoods.
 *
 * The filter supports both 2-D images and 3-D volumes. It uses histogram-based
 * median extraction for 8-bit data.
 */
class MedianBlurFilter : public Filter {
public:
    /**
     * @brief Construct a median blur filter.
     * @param kernelSize Width of the square or cubic neighbourhood. Must be odd
     * and greater than 1.
     */
    explicit MedianBlurFilter(int kernelSize);

    /**
     * @brief Apply median blur to an image.
     * @param image Image to modify.
     */
    void apply(Image& image) override;
    /**
     * @brief Apply median blur to a volume.
     * @param volume Volume to modify.
     */
    void apply(Volume& volume) override;
private:
    /// Width of the median neighbourhood.
    int kernelSize;

    /**
     * @brief Check whether the kernel size is usable.
     * @param kernelSize Candidate neighbourhood size.
     * @return True when the size is odd and greater than 1.
     */
    bool isValidKernelSize(int kernelSize) const;
};

#endif
