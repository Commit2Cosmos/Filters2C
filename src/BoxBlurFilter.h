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

#ifndef BOX_BLUR_FILTER_H
#define BOX_BLUR_FILTER_H

#include "Filter.h"

/**
 * @class BoxBlurFilter
 * @brief Applies a uniform averaging blur to a 2-D image.
 *
 * Each output pixel is replaced by the mean of the pixels in a square
 * neighbourhood centred on that pixel. Border reads use the clamped access
 * policy provided by Image::getPixel().
 */
class BoxBlurFilter : public Filter {
public:
    /**
     * @brief Construct a box blur filter.
     * @param kernelSize Width and height of the square kernel. Must be odd and
     * greater than 1.
     */
    explicit BoxBlurFilter(int kernelSize);

    /**
     * @brief Blur an image in-place with a box filter.
     * @param image Image to modify.
     */
    void apply(Image& image) override;

private:
    /// Width and height of the square averaging kernel.
    int kernelSize;

    /**
     * @brief Check whether the kernel size is usable.
     * @param kernelSize Candidate kernel width and height.
     * @return True when the kernel size is odd and greater than 1.
     */
    bool isValidKernelSize(int kernelSize) const;
};

#endif
