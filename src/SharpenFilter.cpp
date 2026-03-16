#include "SharpenFilter.h"

SharpenFilter::SharpenFilter()
    : ConvFilter(
        // G
        {{  0, -1,  0 },
         { -1,  4, -1 },
         {  0, -1,  0 }}
    )
{}
