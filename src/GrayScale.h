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

#ifndef GRAYSCALE_H
#define GRAYSCALE_H

#include "Filter.h"
#include "Image.h"

/**
 * @class GrayScale
 * @brief Converts RGB images to grayscale intensities.
 *
 * The filter computes luminance from the RGB channels and writes the result
 * back into the image in-place.
 */
class GrayScale : public Filter
{
public:
    /**
     * @brief Convert an image to grayscale in-place.
     * @param image Image to modify.
     */
    void apply(Image& image) override;
};

#endif
