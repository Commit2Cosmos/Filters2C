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
    SobelFilter();
    void apply(Image& image) override;
};

#endif // SOBEL_FILTER_H
