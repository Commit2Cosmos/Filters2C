#ifndef SHARPEN_FILTER_H
#define SHARPEN_FILTER_H

#include "ConvFilter.h"

/**
 * @class SharpenFilter
 * @brief Enhances edge contrast to sharpen the image.
 *
 * Single-kernel filter. Uses a Laplacian-based kernel that amplifies the
 * centre pixel relative to its neighbours. No overrides needed.
 */
class SharpenFilter : public ConvFilter {
public:
    SharpenFilter();
};

#endif // SHARPEN_FILTER_H
