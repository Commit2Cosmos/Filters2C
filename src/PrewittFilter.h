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
    PrewittFilter();
    void apply(Image& image) override;
};

#endif // PREWITT_FILTER_H
