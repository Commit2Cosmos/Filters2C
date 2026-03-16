#ifndef SCHARR_FILTER_H
#define SCHARR_FILTER_H

#include "ConvFilter.h"

/**
 * @class ScharrFilter
 * @brief Applies the Scharr edge detection operator.
 *
 * A refined Sobel variant with larger weights (3, 10) for better rotational
 * symmetry. Produces stronger gradient responses than Sobel or Prewitt.
 */
class ScharrFilter : public ConvFilter {
public:
    ScharrFilter();
};

#endif // SCHARR_FILTER_H
