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

#ifndef PREWITT_FILTER_H
#define PREWITT_FILTER_H

#include "ConvFilter.h"

/**
 * @class PrewittFilter
 * @brief Applies the Prewitt edge detection operator.
 *
 * Similar to Sobel but with uniform weights — less noise suppression,
 * slightly faster to compute by hand.
 */
class PrewittFilter : public ConvFilter {
public:
    /**
     * @brief Construct a Prewitt edge detector.
     */
    PrewittFilter();
    /**
     * @brief Apply Prewitt edge detection to an image.
     * @param image Image to modify.
     */
    void apply(Image& image) override;
};

#endif // PREWITT_FILTER_H
