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
{}

void SobelFilter::apply(Image& image) {
    if (image.getChannels() > 1) image.toGrayscale();
    auto results = computeResults(image);
    normalise(results);
    writeResults(image, results);
}
