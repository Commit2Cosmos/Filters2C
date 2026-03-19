#ifndef SHARPEN_FILTER_H
#define SHARPEN_FILTER_H

#include "ConvFilter.h"

/**
 * @class SharpenFilter
 * @brief Enhances edge contrast to sharpen the image.
 *
 * Single-kernel filter. Uses a Laplacian-based kernel that amplifies the
 * centre pixel relative to its neighbours.
 */
class SharpenFilter : public ConvFilter {
public:
    /**
     * @brief Construct a sharpening filter.
     */
    SharpenFilter();
};

#endif // SHARPEN_FILTER_H
