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

#ifndef SCHARR_FILTER_H
#define SCHARR_FILTER_H

#include "ConvFilter.h"

/**
 * @class ScharrFilter
 * @brief Applies the Scharr edge detection operator.
 *
 * A refined Sobel variant with larger weights (3, 10) for better rotational
 * symmetry.
 */
class ScharrFilter : public ConvFilter {
public:
    /**
     * @brief Construct a Scharr edge detector.
     */
    ScharrFilter();
    /**
     * @brief Apply Scharr edge detection to an image.
     * @param image Image to modify.
     */
    void apply(Image& image) override;
};

#endif // SCHARR_FILTER_H
