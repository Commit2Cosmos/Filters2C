#ifndef ROBERTS_CROSS_FILTER_H
#define ROBERTS_CROSS_FILTER_H

#include "ConvFilter.h"

/**
 * @class RobertsCrossFilter
 * @brief Applies the Roberts' Cross edge detection operator.
 *
 * Uses two 2x2 kernels detecting diagonal edges. The smallest and fastest
 * edge detector here, but more sensitive to noise than the 3x3 operators.
 */
class RobertsCrossFilter : public ConvFilter {
public:
    RobertsCrossFilter();
};

#endif // ROBERTS_CROSS_FILTER_H
