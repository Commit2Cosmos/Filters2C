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
