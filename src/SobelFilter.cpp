#include "SobelFilter.h"

SobelFilter::SobelFilter()
    : ConvFilter(
        // Gx
        {{ -1, 0, 1 },
         { -2, 0, 2 },
         { -1, 0, 1 }},
        // Gy
        {{ -1, -2, -1 },
         {  0,  0,  0 },
         {  1,  2,  1 }})
{
    normalise_ = true;
}
