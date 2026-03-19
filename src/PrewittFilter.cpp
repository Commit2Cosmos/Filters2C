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
{}

static constexpr float PREWITT_AXIS_MAX = 3.f * 255.f;

void PrewittFilter::apply(Image& image) {
    if (image.getChannels() > 1) image.toGrayscale();
    auto results = computeResults(image);
    float theoreticalMax = getTheoreticalMax(PREWITT_AXIS_MAX);
    normalise(results, theoreticalMax);
    writeResults(image, results);
}
