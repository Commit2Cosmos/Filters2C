#ifndef SALTANDPEPPER_H
#define SALTANDPEPPER_H

#include "Filter.h"
#include "Image.h"

/**
 * @class SaltAndPepper
 * @brief Injects impulsive black-and-white noise into an image.
 */
class SaltAndPepper : public Filter
{
public:
    /**
     * @brief Construct a salt-and-pepper noise filter.
     * @param noiseRatio Fraction of pixels to corrupt, typically in [0, 1].
     */
    explicit SaltAndPepper(double noiseRatio);

    /**
     * @brief Apply salt-and-pepper noise in-place.
     * @param image Image to modify.
     */
    void apply(Image& image) override;

private:
    /// Fraction of pixels selected for corruption.
    double noiseRatio;
};

#endif
