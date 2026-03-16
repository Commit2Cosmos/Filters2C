#include "PrewittFilter.h"

PrewittFilter::PrewittFilter()
    : ConvFilter(
        // Gx
        {{ -1, 0, 1 },
         { -1, 0, 1 },
         { -1, 0, 1 }},
        // Gy
        {{ -1, -1, -1 },
         {  0,  0,  0 },
         {  1,  1,  1 }})
{
    normalise_ = true;
}
