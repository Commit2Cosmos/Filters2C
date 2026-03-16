#include "EmbossFilter.h"

EmbossFilter::EmbossFilter()
    : ConvFilter(
        // Gx
        {{  -3, 0,  3 },
         { -10, 0, 10 },
         {  -3, 0,  3 }},
        // Gy
        {{ -3, -10, -3 },
         {  0,   0,  0 },
         {  3,  10,  3 }})
{}
