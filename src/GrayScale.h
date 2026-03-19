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
