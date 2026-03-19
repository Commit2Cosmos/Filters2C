#ifndef HISTOGRAMEQUALISATION_H
#define HISTOGRAMEQUALISATION_H

#include "Filter.h"
#include <string>
#include <vector>

/**
 * @class HistogramEqualisation
 * @brief Improves image contrast by redistributing intensity values.
 *
 * Grayscale images are equalised directly. For RGB images, equalisation is
 * performed on either the HSV value channel or HSL lightness channel.
 */
class HistogramEqualisation : public Filter
{
private:
    /// Colour space used when processing multi-channel images.
    std::string colourSpace;

    /**
     * @brief Build the cumulative-distribution lookup table for equalisation.
     * @param values Input intensity samples.
     * @return Mapping from original intensities to equalised intensities.
     */
    std::vector<unsigned char> buildEqualisationLUT(
        const std::vector<unsigned char>& values) const;

public:
    /**
     * @brief Construct a histogram equalisation filter.
     * @param cs Colour space for RGB images, typically "HSV" or "HSL".
     */
    HistogramEqualisation(const std::string& cs = "HSV");

    /**
     * @brief Apply histogram equalisation to an image in-place.
     * @param image Image to modify.
     */
    void apply(Image& image) override;
};

#endif
