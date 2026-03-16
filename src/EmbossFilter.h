#ifndef EMBOSS_FILTER_H
#define EMBOSS_FILTER_H

#include "ConvFilter.h"
#include <vector>

/**
 * @class EmbossFilter
 * @brief Applies an embossing effect.
 *
 * Single-kernel filter that highlights edges as raised 3D-style relief.
 * Results are biased by +128 to centre the output around mid-grey,
 * so combineResponses is not used and normalise is overridden.
 */
class EmbossFilter : public ConvFilter {
public:
    EmbossFilter();

protected:
    // Overridden to shift output by +128 (centres result around mid-grey).
    // Without this, flat areas map to 0 (black) instead of 128 (grey).
    std::vector<float> combineResponses(
        const std::vector<float>& respX,
        const std::vector<float>& respY) const override;
};

#endif // EMBOSS_FILTER_H
