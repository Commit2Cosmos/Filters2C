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

#ifndef THRESHOLD_H
#define THRESHOLD_H

#include "Filter.h"
#include <string>

/**
 * @class Threshold
 * @brief Produces a binary image by thresholding grayscale or colour intensity.
 *
 * For RGB images, thresholding is applied in either HSV or HSL space and the
 * output is forced to black and white.
 */
class Threshold : public Filter
{
private:
    /// Threshold value in 8-bit intensity space.
    int thresholdValue;
    /// Colour space used when processing RGB data.
    std::string colourSpace;

public:
    /**
     * @brief Construct a threshold filter.
     * @param value Threshold in the range [0, 255].
     * @param cs Colour space for RGB images, typically "HSV" or "HSL".
     */
    Threshold(int value = 127, const std::string& cs = "HSV");

    /**
     * @brief Apply binary thresholding in-place.
     * @param image Image to modify.
     */
    void apply(Image& image) override;
};

#endif
