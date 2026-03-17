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

void PrewittFilter::apply(Image& image) {
    if (image.getChannels() > 1) image.toGrayscale();
    auto results = computeResults(image);
    normalise(results);
    writeResults(image, results);
}
