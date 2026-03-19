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

// Sobel: Gx positive weights = 1+2+1 = 4
static constexpr float SOBEL_AXIS_MAX = 4.f * 255.f;

void SobelFilter::apply(Image& image) {
    if (image.getChannels() > 1) image.toGrayscale();
    auto results = computeResults(image);
    float theoreticalMax = getTheoreticalMax(SOBEL_AXIS_MAX);
    normalise(results, theoreticalMax);
    writeResults(image, results);
}
