#include "RobertsCrossFilter.h"

RobertsCrossFilter::RobertsCrossFilter()
    : ConvFilter(
        // G1
        {{ 1,  0 },
         { 0, -1 }},
        // G2
        {{  0, 1 },
         { -1, 0 }})
{}

static constexpr float ROBERTS_AXIS_MAX = 1.f * 255.f;

void RobertsCrossFilter::apply(Image& image) {
    if (image.getChannels() > 1) image.toGrayscale();
    auto results = computeResults(image);
    float theoreticalMax = getTheoreticalMax(ROBERTS_AXIS_MAX);
    normalise(results, theoreticalMax);
    writeResults(image, results);
}
