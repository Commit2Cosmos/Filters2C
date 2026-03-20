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

#ifndef SOBEL_FILTER_H
#define SOBEL_FILTER_H

#include "ConvFilter.h"

/**
 * @class SobelFilter
 * @brief Applies the Sobel edge detection operator.
 *
 * Uses two 3x3 kernels (Gx, Gy) to detect horizontal and vertical edges.
 * Combines responses using gradient magnitude sqrt(Gx² + Gy²).
 */
class SobelFilter : public ConvFilter {
public:
    /**
     * @brief Construct a Sobel edge detector.
     */
    SobelFilter();
    /**
     * @brief Apply Sobel edge detection to an image.
     * @param image Image to modify.
     */
    void apply(Image& image) override;
};

#endif // SOBEL_FILTER_H
