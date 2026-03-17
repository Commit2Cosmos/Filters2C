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

void RobertsCrossFilter::apply(Image& image) {
    if (image.getChannels() > 1) image.toGrayscale();
    auto results = computeResults(image);
    normalise(results);
    writeResults(image, results);
}
