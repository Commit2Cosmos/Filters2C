#include "RobertsCrossFilter.h"

RobertsCrossFilter::RobertsCrossFilter()
    : ConvFilter(
        // G1
        {{ 1,  0 },
         { 0, -1 }},
        // G2
        {{  0, 1 },
         { -1, 0 }})
{
    normalise_ = true;
}
