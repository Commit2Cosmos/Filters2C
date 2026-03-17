#include "ScharrFilter.h"

ScharrFilter::ScharrFilter()
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

void ScharrFilter::apply(Image& image) {
    if (image.getChannels() > 1) image.toGrayscale();
    auto results = computeResults(image);
    normalise(results);
    writeResults(image, results);
}
