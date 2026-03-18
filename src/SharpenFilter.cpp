#include "SharpenFilter.h"

SharpenFilter::SharpenFilter()
    : ConvFilter(
        // Combined kernel: computes (I_original + G)
        {{  0, -1,  0 },
         { -1,  5, -1 },
         {  0, -1,  0 }}
    )
{}
