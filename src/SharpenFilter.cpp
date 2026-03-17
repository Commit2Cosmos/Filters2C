#include "SharpenFilter.h"

SharpenFilter::SharpenFilter()
    : ConvFilter(
        // G
        // central value is 5, effectively adding filtered
        // to the original image
        {{  0, -1,  0 },
         { -1,  5, -1 },
         {  0, -1,  0 }}
    )
{}
